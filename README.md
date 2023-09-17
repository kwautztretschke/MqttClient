# MqttClient
A library implementing a Mqtt client for ESPs that can connect to your home mqtt server. It also handles wifi connection. Configuration, see below.

To use it, in your main cpp file you should have the setup routine call
```cpp
MqttClient.startWiFi();
MqttClient.setInputCallback(ProgramManager::input);
MqttClient.setFocusCallback(ProgramManager::focus);
MqttClient.setRebootCallback(0);
MqttClient.init();
```
if you are using my program manager, which this thing is kind of designed to work with

# Configuration
You'd obviously want to use your own wifi credentials and mqtt broker. 
For this, you simply need to create a header file "wificredentials.h" in a /config/ directory in your project root folder.
(Note: for this to work, a certain folder structure is required. The library folder should be located at /*path/to/your/project*/lib/MqttClient)

An example configuration file can be found in MqttClient/include/wificredentials.h
Just follow the instructions in the comments there.

# Dependencies
This library was built using platform.io and the libraries ESP8266WiFi and PubSubClient
