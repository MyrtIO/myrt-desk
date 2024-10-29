#pragma once

#include <PubSubClient.h>

typedef void (*DataHandler_t)(PubSubClient* client, byte* payload, uint length);
typedef void (*ReportHandler_t)(PubSubClient* client);

struct TopicHandler {
	const char* topic = nullptr;
	DataHandler_t callback = nullptr;
};

struct TopicReporter {
	ReportHandler_t callback = nullptr;
	unsigned long interval = 0;
	unsigned long lastExecution = 0;
};
