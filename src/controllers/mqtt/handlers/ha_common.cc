#include "ha_common.h"
#include "topics.h"
#include <config.h>

const HomeAssistant::Device haDesk(
	CONFIG_DEVICE_NAME,
	CONFIG_DEVICE_ID,
	MQTT_TOPICS_PREFIX
);
char responseBuffer[MQTT_BUFFER_SIZE];
