#include "config_data.h"
#include "topics.h"
#include <platform.h>

const char* kHeightOutTopic = "myrt/desk/height";
const char* kHightInTopic = "myrt/desk/height/set";
const char* kHeightConfigTopic = "homeassistant/number/myrt_desk_height/config";

const size_t kHeightReportInterval = 60000;

void updateHeightState(PubSubClient* client, byte* payload, unsigned int length) {
	auto platform = IO_INJECT(IHeightPlatform);
	auto value = atoi((char*)payload);
	platform->setHeight(value);
}

void reportHeightState(PubSubClient* client) {
	auto platform = IO_INJECT(IHeightPlatform);
	auto height = platform->getHeight();
	char* heightBuffer = new char[4];
	itoa(height, heightBuffer, 10);
	client->publish(kHeightOutTopic, heightBuffer);
	delete[] heightBuffer;
}

void reportHeightConfig(PubSubClient* client) {
	client->publish(kHeightConfigTopic, kHeightConfig);
}

void registerHeightTopics(MyrtQTT* server) {
	server->reportConfig(reportHeightConfig)
	->report(reportHeightState, kHeightReportInterval)
	->on(kHightInTopic, updateHeightState);
}
