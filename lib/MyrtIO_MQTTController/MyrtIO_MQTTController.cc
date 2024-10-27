#include "MyrtIO_MQTTController.h"
#include <WiFi.h>

const char* kMQTTName = "MQTT";

io::Logger mqttLog(kMQTTName);

io::MQTTController::MQTTController(const io::MQTTControllerParams& params):
	params_(params),
	server_(PubSubServer(client_, params.clientID)),
	client_(WiFiClient()) {
		server_.setListener(this);
	}

const char* io::MQTTController::name() {
	return kMQTTName;
}

void io::MQTTController::setup() {
	setupHandlers(&server_);
	server_.start(params_.host, params_.port);
}

void io::MQTTController::loop() {
	server_.loop();
}

void io::MQTTController::onConnect() {
	mqttLog.print("Connected");
}

void io::MQTTController::onDisconnect() {
	mqttLog.print("Disconnected");
}

void io::MQTTController::onMessage(char* topic, uint8_t* payload, uint16_t length) {
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
