// **********************************************************
// this file needs to include your wifi credentials. They should be found at:
#include "../../../config/wificredentials.h"
//
// to create yours, just copy everything below the line to a file
// <project_root>/config/wificredentials.h
//
//***********************************************************

#ifndef __WIFICREDENTIALS_H__
#define __WIFICREDENTIALS_H__

#include <ESP8266WiFi.h>

namespace wificredentials{
	const String 	ssid = 			"SSID";
	const String	password = 		"Password";
	const IPAddress mqttServerIp = 	IPAddress(1,1,1,1);
	const String	deviceName = 	"unnamedESP";
}

#endif // __WIFICREDENTIALS_H__