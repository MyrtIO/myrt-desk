#include "mqtt_controller.h"

const char* kMQTTName = "MQTT";

io::Logger mqttLog(kMQTTName);

const char* MQTTController::name() {
	return kMQTTName;
}

void MQTTController::setup() {
	registerLightTopics(&server_);
	registerHeightTopics(&server_);
	registerLogTopics(&server_, logStream());
	server_.start(params_.host, params_.port);
}

void MQTTController::loop() {
	server_.loop();
}

void MQTTController::onConnect() {
	mqttLog.print("connected");
}

void MQTTController::onDisconnect() {
	mqttLog.print("disconnected");
}

void MQTTController::onMessage(char* topic, uint8_t* payload, uint16_t length) {
#ifdef IO_DEBUG
	mqttLog.builder()
		->append("message received: ")
		->append(topic)
		->append(" - ")
		->append(length)
		->append(" bytes");
	mqttLog.flush();
#endif
}
