#include "MqttClient.h"
#include <cstring>


void MqttClientClass::handleMessage(char* topic, uint8_t* payload, unsigned int plen){
	//************************* Receiving Package ***********************************
	// copy payload into buffer
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
	topic 	+= i;		// topic pointer now points to correct beginning
	tlen 	-= i;		// tlen is the length of the topic string

	Serial.printf("Decoded topic: %s\n", topic);

	//********************** Topic Handling ***************************************
	// depending on which topic was received, do things like invoke callbacks
	if (strncmp(topic, "reboot", tlen) == 0) {
		p_reboot();
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
	while (!m_client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Create a random client ID
		String clientId = "ESP8266Client-";
		clientId += mac_address;
		// Attempt to connect
		if (m_Client.connect(m_DeviceName.c_str())) {
			Serial.println("connected");
			// Once connected, publish an announcement...
			String status_topic = "reactor/" + m_DeviceName + "/status";
			m_Client.publish((const char*)status_topic.c_str(), "connected");
		} else {
		Serial.print("failed, rc=");
		Serial.print(m_client.state());
		Serial.println(" try again in 5 seconds");
		// Wait 5 seconds before retrying
		delay(5000);
		}
	}
}


void MqttClientClass::startWiFi(){
	m_mac_address = WiFi.macAddress();
	Serial.println();
	Serial.print("MAC-Address: ");
	Serial.println(m_mac_address);
	Serial.print("Connecting to ");
	Serial.println(wificredentials::ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(wificredentials::ssid, wificredentials::password);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(500);
	}
	Serial.print("Connected! IP address: ");
	Serial.println(WiFi.localIP());
	m_client.setClient(m_wifi);
	m_client.setServer(wificredentials::mqttServerIp, 1883);
	reconnect(m_mac_address);
}

String MqttClientClass::receiveNameFromMacAddress(){
	char name[32] = {0};
	bool name_received = false;
	// set a temporary callback
	m_client.setCallback(
		[&name, &name_received](char* topic, uint8_t* payload, unsigned int length){
			strncpy(name, (char*)payload, length);
			name_received = true;
		}
	);
	// declare our status as searching
	String mac_status_topic = "mac/" + m_mac_address + "/status";
	m_client.publish((const char*)mac_status_topic.c_str(), "searching_name");
	// subscribe to the appropriate topic
	String mac_name_topic = "mac/" + m_mac_address + "/name";
	m_client.subscribe((const char*)mac_name_topic.c_str());
	Serial.print("Waiting to receive device name via ");
	Serial.print(mac_name_topic);
	// wait until a name is received
	while(!name_received){
		m_client.loop();
		Serial.print(".");
		delay(10);
	}
	// name received!
	m_client.publish((const char*)mac_status_topic.c_str(), "ready");
	Serial.println();
	Serial.print("My name is ");
	Serial.println(name);
	return String(name);
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
	if(!m_Client.connected())
		connectMqtt();
}

MqttClientClass MqttClient;
