#include <config.h>
#include "mqtt_controller.h"
#include "topics.h"

MQTTController_ MQTTController = MQTTController_();

void handleTopicMessage(char* topic, byte* payload, unsigned int length) {
  MQTTController.handleMessage(topic, payload, length);
}

const char *MQTTController_::name() {
  return "MQTT";
}

void MQTTController_::setup() {
  client_.setCallback(handleTopicMessage);
  mqtt_.setServer(CONFIG_MQTT_HOST, CONFIG_MQTT_PORT);
  registerLightTopics(&mqtt_);
  registerHeightTopics(&mqtt_);
}

void MQTTController_::loop() {
  mqtt_.loop();
}

void MQTTController_::handleMessage(char* topic, byte* payload, unsigned int length) {
  mqtt_.handleMessage(topic, payload, length);
}
