#include <MyrtIO.h>
#include <WiFi.h>
#include "config.h"
#include "controllers/controllers.h"
#include "platform/platform.h"

IODevice desk;

IOLogger mainLog("Main", &Serial);

void setup() {
	Serial.begin();
#ifdef IO_DEBUG
	while (!Serial) {
		// Wait for serial port to connect
	}
#endif
	mainLog.print("starting desk...");
	auto light = IO_INJECT_INSTANCE(LightPlatform);
	auto height = IO_INJECT_INSTANCE(HeightPlatform);
	auto wifi = IO_INJECT_INSTANCE(WiFiPlatform);
	desk.setup()
	    ->platform(light, height, wifi)
	    ->controllers(&ConnectionController, &MQTTController, &OTAController);
	mainLog.print("desk is initialized");
}

void loop() {
	desk.loop();
}
