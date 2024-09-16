#include "io_device.h"
#include "logger/io_logger.h"

#if IO_BENCHMARK
	#include "benchmark/io_benchmark.h"
	#warning "Benchmarking enabled"
	IOBenchmark ioBenchmark;
#endif

IOLogger deviceLog("IODevice", &Serial);

IODevice* IODevice::setup() {
	deviceLog.print("initializing...");
#if IO_BENCHMARK
	controllers(&ioBenchmark);
#endif
	return this;
}

bool IODevice::addUnit_(IOUnit* u, IOUnit** units, uint8_t* count) {
	deviceLog.builder()
	    ->append("adding unit: ")
	    ->append(u->name())
	    ->flush();
	if (*count >= IO_DEVICE_MAX_CONTROLLERS) {
		deviceLog.print("too many units of this type");
		return false;
	}
	deviceLog.print("setup unit...");
	u->setup();
	units[*count] = u;
	*count = *count + 1;
	deviceLog.print("unit successfully added");
	return false;
}
