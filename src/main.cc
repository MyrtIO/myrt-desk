#include <MyrtIO.h>
#include "controllers/controllers.h"
#include "platforms/platforms.h"

io::Device desk("Desk");

void setup() {
	auto mqttController = IO_INJECT_INSTANCE(MQTTController);
	io::Logger::setOutput(mqttController->logStream());
	// io::Logger::setOutput(&Serial);
	desk.setup()
	    ->platforms(
			IO_INJECT_UNIT(IWiFiPlatform),
			IO_INJECT_UNIT(IHeightPlatform),
			IO_INJECT_UNIT(ILightPlatform)
		)
	    ->controllers(
			IO_INJECT_UNIT(BootController),
			IO_INJECT_UNIT(MQTTController),
			IO_INJECT_UNIT(OTAController)
		);
	desk.log()->print("initialized");
}

void loop() {
	desk.loop();
}
