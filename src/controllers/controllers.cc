#include "controllers.h"
#include <config.h>

IO_CREATE_INSTANCE(BootController, CONFIG_LIGHT_COLOR_CONNECTED);
IO_CREATE_INSTANCE(MQTTController, {
	.clientID = CONFIG_DEVICE_NAME,
	.host = CONFIG_MQTT_HOST,
	.port = CONFIG_MQTT_PORT
});
IO_CREATE_INSTANCE(OTAController, {
	.hostname = CONFIG_DEVICE_NAME,
	.port = CONFIG_OTA_PORT
});
