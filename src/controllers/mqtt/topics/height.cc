#include <PubSubServer.h>
#include <platform.h>
#include "config_data.h"

// TODO: add generation of config topic
const char* kHeightOutTopic = "myrt/desk/height";
const char* kHightInTopic = "myrt/desk/height/set";
const char* kHeightConfigTopic = "homeassistant/number/myrt_desk_height/config";

const size_t kHeightReportInterval = 60000;
const size_t kHeightConfigReportInterval = 60000 * 5;

void updateHeightState(PubSubClient* client, byte* payload, unsigned int length) {
	auto height = DI_INJECT(IOHeight);
	auto value = atoi((char*)payload);
	height->set(value);
}

void reportHeightState(PubSubClient* client) {
	auto height = DI_INJECT(IOHeight);
	auto currentHeight = height->get();
	char* respBuffer = new char[4];
	itoa(currentHeight, respBuffer, 10);
	client->publish(kHeightOutTopic, respBuffer);
	delete[] respBuffer;
}

void reportHeightConfig(PubSubClient* client) {
	client->publish(kHeightConfigTopic, kHeightConfig);
}

void registerHeightTopics(PubSubServer* server) {
	server
		->report(reportHeightConfig, kHeightConfigReportInterval)
		->report(reportHeightState, kHeightReportInterval)
		->on(kHightInTopic, updateHeightState);
}
