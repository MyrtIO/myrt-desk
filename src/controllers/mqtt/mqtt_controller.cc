#include "mqtt_controller.h"
#include <WiFi.h>

const char* kMQTTName = "MQTT";

IOLogger mqttLog(kMQTTName);

MQTTController::MQTTController(const MQTTControllerParams& params):
	params_(params),
	server_(PubSubServer(client_, params.clientID)) {
		server_.setListener(this);
	}

const char* MQTTController::name() {
	return kMQTTName;
}

void MQTTController::setup() {
	registerLightTopics(&server_);
	registerHeightTopics(&server_);
	registerLogTopics(&server_);
	server_.start(params_.host, params_.port);
}

void MQTTController::loop() {
	server_.loop();
}

void MQTTController::onConnect() {
	mqttLog.print("Connected");
}

void MQTTController::onDisconnect() {
	mqttLog.print("Disconnected");
}

void MQTTController::onMessage(char* topic, uint8_t* payload, uint16_t length) {
#ifdef IO_DEBUG
	mqttLog.builder()
		->append("Message received: ")
		->append(topic)
		->append(" - ")
		->append(length)
		->append(" bytes");
	mqttLog.flush();
#endif
}
