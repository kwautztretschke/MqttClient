#ifndef __MQTTCLIENT_H__
#define __MQTTCLIENT_H__

#include "wificredentials.h"

class WiFiClient;
class PubSubClient;
class String;

class MqttClientClass{
private:
	WiFiClient		m_WifiClient;
	PubSubClient 	m_Client;
	const String	m_DeviceName = wificredentials::deviceName;
	char			m_plBuffer[128] = {0};
	void		 	handleMessage(char* topic, uint8_t* payload, unsigned int length);
	void 			connectMqtt(); 
	// callbacks
	int				(*p_input)(char* key, char* value);
	int				(*p_focus)(char* program);
	void			(*p_reboot)();
public:
	const String 	getDeviceName() {return m_DeviceName;}
	void 			startWiFi();
	void 			init();
	void 			loop(); 
	// callbacks
	void			setInputCallback(int (*input)(char* key, char* value)) {p_input = input;}
	void			setFocusCallback(int (*focus)(char* program)) {p_focus = focus;}
	void			setRebootCallback(void (*reboot)()) {p_reboot = reboot;}
};

extern MqttClientClass MqttClient;

#endif //__MQTTCLIENT_H__