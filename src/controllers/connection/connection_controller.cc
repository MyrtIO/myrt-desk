#include "connection_controller.h"

const char* kWiFiControllerName = "Connection";

IOLogger connLog(kWiFiControllerName, &Serial);

ConnectionController_ ConnectionController;

void ConnectionController_::setup() {
	firstConnect_ = true;
}

void ConnectionController_::loop() {
	if (wifi_->connected()) {
		if (firstConnect_) {
			connLog.print("Demonstrating the connection");
			firstConnect_ = false;
			light_->setEffect(LightEffect::Fill, true);
			light_->setColor(CONFIG_LIGHT_COLOR_CONNECTED);
		}
		return;
	}
}

const char* ConnectionController_::name() {
	return kWiFiControllerName;
}
