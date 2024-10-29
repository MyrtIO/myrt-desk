#include <PubSubServer.h>
#include <NumberEntity.h>
#include <platforms.h>
#include <config.h>
#include "ha_common.h"

HomeAssistant::NumberEntity heightEntity({
	.name = "height",
	.identifier = "height",
	.deviceClass = "distance",
	.icon = "mdi:desk",
	.unit = "mm",
	.writable = true,
	.min = CONFIG_BEKANT_HEIGHT_MIN,
	.max = CONFIG_BEKANT_HEIGHT_MAX
}, haDesk);

IHeightPlatform* height;
uint16_t value;

void updateHeightState(PubSubClient* client, byte* payload, uint length) {
	heightEntity.parseValue(value, payload);
	height->set(value);
}

void reportHeightState(PubSubClient* client) {
	value = height->get();
	heightEntity.serializeValue(responseBuffer, value);
	client->publish(heightEntity.stateTopic(), responseBuffer);
}

void reportHeightConfig(PubSubClient* client) {
	heightEntity.serializeConfig(responseBuffer);
	client->publish(heightEntity.configTopic(), responseBuffer);
}

void registerHeightTopics(PubSubServer* server) {
	height = IO_INJECT(IHeightPlatform);
	server
		->report(reportHeightConfig, MQTT_CONFIG_REPORT_INTERVAL)
		->report(reportHeightState, MQTT_STATE_REPORT_INTERVAL)
		->on(heightEntity.commandTopic(), updateHeightState);
}
