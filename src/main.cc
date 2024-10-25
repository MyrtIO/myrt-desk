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
	desk.setup()
	    ->platforms(
			DI_INJECT_UNIT(IOWiFi),
			DI_INJECT_UNIT(IOHeight),
			DI_INJECT_UNIT(IOLight)
		)
	    ->controllers(
			DI_INJECT_UNIT(BootController),
			DI_INJECT_UNIT(MQTTController),
			DI_INJECT_UNIT(OTAController)
		);
	mainLog.print("desk is initialized");
}

void loop() {
	desk.loop();
}
