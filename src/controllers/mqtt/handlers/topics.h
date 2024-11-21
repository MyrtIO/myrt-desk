#pragma once

#include <PubSubServer.h>

#define MQTT_TOPICS_PREFIX "myrt/desk"

void registerLightTopics(PubSubServer* server);
void registerHeightTopics(PubSubServer* server);
void registerLogTopics(PubSubServer* server, Stream* stream);
