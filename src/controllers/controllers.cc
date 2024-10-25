#include "controllers.h"
#include <config.h>

DI_REGISTER_INSTANCE(BootController, CONFIG_LIGHT_COLOR_CONNECTED);
DI_REGISTER_INSTANCE(MQTTController, {
	.clientID = CONFIG_DEVICE_NAME,
	.host = CONFIG_MQTT_HOST,
	.port = CONFIG_MQTT_PORT
});
DI_REGISTER_INSTANCE(OTAController);
