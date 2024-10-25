#include <MyrtIO.h>
#include <TopicStream.h>
#include "controllers/controllers.h"
#include "platforms/platforms.h"

io::Device desk("Desk");

void setup() {
	io::Logger::setOutput(&mqttLogStream);
	desk.setup()
	    ->platforms(
			DI_INJECT_UNIT(IWiFiPlatform),
			DI_INJECT_UNIT(IOHeight),
			DI_INJECT_UNIT(IOLight)
		)
	    ->controllers(
			DI_INJECT_UNIT(BootController),
			DI_INJECT_UNIT(MQTTController),
			DI_INJECT_UNIT(OTAController)
		);
	desk.log()->print("initialized");
}

void loop() {
	desk.loop();
}
