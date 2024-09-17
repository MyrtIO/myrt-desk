#include "ota_controller.h"
#include <ArduinoOTA.h>

const char* kOTAControllerName = "OTA";

IOLogger otaLog(kOTAControllerName);

OTAController_ OTAController = OTAController_();

void OTAController_::setup() {
	ArduinoOTA.setHostname("MyrtDeskOTA");
	ArduinoOTA.onStart([]() {
		otaLog.print("Starting update...");
	});
	ArduinoOTA.onEnd([]() {
		otaLog.print("Done!");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		otaLog.debugBuilder()
		    ->append("Progress: ")
		    ->append((uint8_t)(progress / (total / 100)))
		    ->append("%")
		    ->flush();
	});
	ArduinoOTA.onError([](ota_error_t error) {
		otaLog.print("Update error");
		if (error == OTA_AUTH_ERROR) {
			otaLog.print("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			otaLog.print("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			otaLog.print("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			otaLog.print("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			otaLog.print("End Failed");
		}
	});
}
void OTAController_::loop() {
	if (firstConnect_) {
		firstConnect_ = false;
		ArduinoOTA.begin();
	}
	if (wifi_->connected()) {
		ArduinoOTA.handle();
	}
}

const char* OTAController_::name() {
	return kOTAControllerName;
}
