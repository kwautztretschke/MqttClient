#ifndef __MQTTCLIENT_H__
#define __MQTTCLIENT_H__

#include "wificredentials.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


class MqttClientClass{
private:
	WiFiClient		m_WifiClient;
	PubSubClient 	m_Client;
	const String	m_DeviceName = wificredentials::deviceName;
	char			m_plBuffer[128] = {0}; 
	uint8_t			m_Hour = 0;
	uint8_t			m_Minute = 0;
	void		 	handleMessage(char* topic, uint8_t* payload, unsigned int plen);
	void 			keepAlive(char* topic, uint8_t* payload, unsigned int plen);
	void 			connectMqtt(); 
	// callbacks
	void			(*p_power)(bool power);
	void 			(*p_brightness)(uint8_t brightness);
	void 			(*p_color)(uint8_t* c);
	int				(*p_input)(char* key, char* value);
	int				(*p_focus)(char* program);
	void			(*p_reboot)();
public:
	const String 	getDeviceName() {return m_DeviceName;}
	void 			startWiFi();
	void 			init();
	void 			loop(); 
	// callbacks
	void			setPowerCallback(void (*power)(bool power)) {p_power = power;}
	void			setBrightnessCallback(void (*brightness)(uint8_t c)) {p_brightness = brightness;}
	void			setColorCallback(void (*color)(uint8_t* c)) {p_color = color;}
	void			setInputCallback(int (*input)(char* key, char* value)) {p_input = input;}
	void			setFocusCallback(int (*focus)(char* program)) {p_focus = focus;}
	void			setRebootCallback(void (*reboot)()) {p_reboot = reboot;}
};

extern MqttClientClass MqttClient;

#endif //__MQTTCLIENT_H__