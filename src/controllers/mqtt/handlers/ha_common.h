#pragma once

#include <HomeAssistant_MQTT.h>

#ifndef MQTT_BUFFER_SIZE
	#define MQTT_BUFFER_SIZE 512
#endif

#ifndef MQTT_CONFIG_REPORT_INTERVAL
	#define MQTT_CONFIG_REPORT_INTERVAL 60000
#endif

#ifndef MQTT_STATE_REPORT_INTERVAL
	#define MQTT_STATE_REPORT_INTERVAL 30000
#endif

extern const HomeAssistant::Device haDesk;
extern char responseBuffer[MQTT_BUFFER_SIZE];
