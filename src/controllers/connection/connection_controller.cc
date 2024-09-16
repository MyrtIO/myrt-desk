#include "connection_controller.h"

const char* kWiFiControllerName = "Connection";

IOLogger connLog(kWiFiControllerName, &Serial);

ConnectionController_ ConnectionController;

void ConnectionController_::setup() {
	wereConnected_ = true;
}

void ConnectionController_::loop() {
	if (wifi_->connected()) {
		if (!wereConnected_) {
			connLog.print("connected!");
			wereConnected_ = true;
			light_->setColor(CONFIG_LIGHT_COLOR_CONNECTED);
		}
		return;
	}

	if (wereConnected_) {
		connLog.print("connecting...");
		wereConnected_ = false;
	}
}

const char* ConnectionController_::name() {
	return kWiFiControllerName;
}
