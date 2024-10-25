#include "boot_controller.h"

const char* kBootControllerName = "Boot";

IOLogger connLog(kBootControllerName, &Serial);

void BootController::setup() {
	firstConnect_ = true;
}

void BootController::loop() {
	if (!firstConnect_) {
		return;
	}
	if (wifi_->connected()) {
		connLog.print("Animating connection to wifi");
		firstConnect_ = false;
		light_->setEffect(LightEffect::Fill, true);
		light_->setColor(connectionColor_);
	}
}

const char* BootController::name() {
	return kBootControllerName;
}
