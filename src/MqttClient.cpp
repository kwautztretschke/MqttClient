#include "MqttClient.h"


void MqttClientClass::handleMessage(char* topic, uint8_t* payload, unsigned int plen){
	//************************* Receiving Package ***********************************
	// copy payload into empty buffer
	if(plen>=127){
		Serial.println("Error, payload too large");
		return;
	}
	memset(m_plBuffer, 0, 128*sizeof(char));
	strncpy(m_plBuffer, (char*)payload, plen);
	Serial.printf("Message l=%d arrived [%s]: \"%s\"\n", plen, topic, m_plBuffer);

	// decode topic and strip "reactor/devicename/" part
	unsigned int i = String("reactor/" + m_DeviceName + "/").length();
	unsigned int tlen = strlen(topic);
	if (i >= tlen){
		Serial.printf("Error, topic too short, i=%d, tlen=%d\n", i, tlen);
		return;
	}
	topic 	+= i;		// topic pointer now points to first character
	tlen 	-= i;		// topic length is now accurate

	Serial.printf("Decoded topic: %s\n", topic);

	//********************** Topic Handling ***************************************
	// depending on which topic was received, do things like invoke callbacks
	if (strncmp(topic, "reboot", tlen) == 0) {
		p_reboot();
	} else if (strncmp(topic, "power", tlen) == 0) {
		if (strncmp(m_plBuffer, "on", plen) == 0)
			p_power(true);
		else
			p_power(false);
	} else if (strncmp(topic, "brightness", tlen) == 0) {
		int brightness = strtol(m_plBuffer, NULL, 10);
		if (brightness > 255)
			brightness = 255;
		else if (brightness < 0)
			brightness = 0;
		p_brightness((uint8_t) brightness);
	// zylProgManager related stuff
	} else if (strncmp(topic, "color", tlen) == 0) {
		uint8_t c[3] = {0};
		char buf[3] = {0};
		for (int i=0; i<3; i++){
			buf[0] = m_plBuffer[2*i];
			buf[1] = m_plBuffer[2*i+1];
			c[i] = strtol(buf, NULL, 16);
		}
		p_color(c);
	} else if (strncmp(topic, "focus", tlen) == 0) {
		if(!plen) {
			Serial.println("Error, no payload");
			return;
		}
		p_focus(m_plBuffer);
	} else if (strncmp(topic, "input", 5) == 0) {
		if (tlen <= 6 /* == strlen("input/") */){
			Serial.println("Error: no string after input/ given");
			return;
		}
		p_input(topic + 6, m_plBuffer);
	} else
		Serial.printf("Error: command \"%s\" not recognized\n", topic);

}


void MqttClientClass::connectMqtt(){
	// Loop until we're reconnected
	while (!m_Client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (m_Client.connect(m_DeviceName.c_str())) {
			Serial.println("connected");
			// Once connected, publish an announcement...
			String status_topic = "reactor/" + m_DeviceName + "/status";
			m_Client.publish((const char*)status_topic.c_str(), "connected");
		} else {
			Serial.print("failed, rc=");
			Serial.print(m_Client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}


void MqttClientClass::startWiFi(){
	String mac_address = WiFi.macAddress();
	Serial.println();
	Serial.println(	"MAC-Address: " + mac_address + 
					", device name" + m_DeviceName);
	Serial.println("Connecting to " + wificredentials::ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(wificredentials::ssid, wificredentials::password);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(500);
	}
	Serial.println("WiFi connected! IP address: " + WiFi.localIP().toString());
}


void MqttClientClass::init(){
	// connect to mqtt broker
	m_Client.setClient(m_WifiClient);
	m_Client.setServer(wificredentials::mqttServerIp, 1883);
	m_Client.setCallback( // lambda instead of function pointer because handleMessage is non static
		[this](char* topic, uint8_t* payload, unsigned int length)
		{handleMessage(topic, payload, length);}
	);
	connectMqtt();
	// Subscribe to all topics belonging to this reactor via
	// reactor/<name>/#
	String reactor_topic = "reactor/" + m_DeviceName + "/#";
	m_Client.subscribe((const char*)reactor_topic.c_str());
	Serial.println("Subscribed to " + reactor_topic);
}


void MqttClientClass::loop(){
	m_Client.loop();
	if(!m_Client.connected()){
		Serial.println("Lost Connection to MQTT Broker!");
		connectMqtt();
	}
}

MqttClientClass MqttClient;