#include <MyrtIO/device/device.h>
#include "controllers/controllers.h"
#include "platforms/platforms.h"

#ifdef IO_BENCHMARK
#include <MyrtIO/benchmarking/lps.h>
#endif

io::Device desk("Desk");

void setup() {
	auto mqttController = IO_INJECT_INSTANCE(MQTTController);
	io::Logger::setOutput(mqttController->logStream());
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
#ifdef IO_BENCHMARK
	io::LPSBenchmark lpsBenchmark;
	desk.controllers(&lpsBenchmark);
#endif
	desk.log()->print("initialized");
}

void loop() {
	desk.loop();
}
