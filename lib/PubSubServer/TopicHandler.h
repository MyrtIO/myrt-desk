#pragma once

#include <PubSubClient.h>

typedef void (*DataCallback)(PubSubClient* client, byte* payload, unsigned int length);
typedef void (*ReportCallback)(PubSubClient* client);

struct TopicHandler {
	const char* topic = nullptr;
	DataCallback callback = nullptr;
};

struct TopicReporter {
	ReportCallback callback = nullptr;
	unsigned long interval = 0;
	unsigned long lastExecution = 0;
};
