#pragma once

#include <PubSubServer.h>
#include <PrintBuffer.h>

void registerLightTopics(PubSubServer* server);
void registerHeightTopics(PubSubServer* server);
void registerLogTopics(PubSubServer* server);

extern PrintBuffer<2048> mqttLogStream;
