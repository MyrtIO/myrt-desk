#pragma once

#include <PubSubServer.h>
#include <PrintBuffer.h>

void registerLightTopics(PubSubServer* server);
void registerHeightTopics(PubSubServer* server);
void registerLogTopics(PubSubServer* server);

extern PrintBuffer<1024> mqttLogStream;
