# MqttClient
A library implementing a Mqtt client for ESPs that can connect to your home mqtt server

It handles wifi connection and mqtt broker connection, the respective configuration is found in wificredentials.h


To use it, in your main cpp file you should have the setup routine call
```cpp
MqttClient.startWiFi();
MqttClient.setInputCallback(ProgramManager::input);
MqttClient.setFocusCallback(ProgramManager::focus);
MqttClient.setRebootCallback(0);
MqttClient.init();
```
if you are using my program manager, which this thing is kind of designed to work with

# Dependencies
This library was built using platform.io and the libraries ESP8266WiFi and PubSubClient
