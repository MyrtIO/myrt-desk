#include "ota_controller.h"
#include <ArduinoOTA.h>

const char* kOTAControllerName = "OTA";

io::Logger otaLog(kOTAControllerName);

OTAController::OTAController(const OTAControllerParams& params) {
	params_ = params;
}

void OTAController::setup() {
	ArduinoOTA.setHostname(params_.hostname);
	ArduinoOTA.setPort(params_.port);
	ArduinoOTA.onStart([]() {
		otaLog.print("Starting update...");
	});
	ArduinoOTA.onEnd([]() {
		otaLog.print("Done!");
	});
#ifdef IO_DEBUG
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		otaLog.builder()
		    ->append("Progress: ")
		    ->append((uint8_t)(progress / (total / 100)))
		    ->append("%");
		otaLog.flush();
	});
#endif
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
void OTAController::loop() {
	if (!wifi_->connected()) {
		return;
	}
	if (firstConnect_) {
		firstConnect_ = false;
		ArduinoOTA.begin();
		return;
	}
	ArduinoOTA.handle();
}

const char* OTAController::name() {
	return kOTAControllerName;
}
