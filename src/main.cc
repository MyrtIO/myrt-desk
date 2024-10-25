#include <MyrtIO.h>
#include <TopicStream.h>
#include "controllers/controllers.h"
#include "platforms/platforms.h"

IODevice desk;
IOLogger mainLog("Main");

void setup() {
	IOLogger::setOutput(&mqttLogStream);
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
