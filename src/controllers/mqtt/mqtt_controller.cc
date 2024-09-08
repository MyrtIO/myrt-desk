#include <config.h>
#include "mqtt_controller.h"
#include "topics.h"

const char *kMQTTName = "MQTT";

IOLogger mqttLog(kMQTTName, &Serial);

MQTTController_ MQTTController = MQTTController_();

void handleTopicMessage(char* topic, byte* payload, unsigned int length) {
  MQTTController.handleMessage(topic, payload, length);
}

const char *MQTTController_::name() {
  return kMQTTName;
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

bool MQTTController_::connected_() {
  if (client_.connected()) {
    return true;
  }

  if (!wifi_->connected()) {
    return false;
  }

  if (client_.connect(CONFIG_DEVICE_NAME)) {
    mqttLog.debug("connected to broker");
    mqtt_.onConnect();
  }

  mqttLog.debug("failed to connect");
  return false;
}
