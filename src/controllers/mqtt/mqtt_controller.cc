#include "mqtt_controller.h"

void MQTTController::setupHandlers(PubSubServer* server) {
	registerLightTopics(server);
	registerHeightTopics(server);
	registerLogTopics(server);
}
