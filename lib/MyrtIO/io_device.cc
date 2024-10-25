#include "io_device.h"
#include "logger/io_logger.h"

#ifdef IO_BENCHMARK
	#include "benchmark/io_benchmark.h"
	#warning "Benchmarking enabled"
	io::Benchmark ioBenchmark;
#endif

io::Device* io::Device::setup() {
	log_.print("initializing...");
#if IO_BENCHMARK
	controllers(&ioBenchmark);
#endif
	return this;
}

io::Logger* io::Device::log() {
	return &log_;
}

bool io::Device::addUnit_(io::Unit* u, io::Unit** units, uint8_t* count) {
	log_.builder()
	    ->append("adding unit: ")
	    ->append(u->name());
	log_.flush();
	if (*count >= IO_DEVICE_MAX_CONTROLLERS) {
		log_.print("too many units of this type");
		return false;
	}
	log_.print("setup unit...");
	u->setup();
	units[*count] = u;
	*count = *count + 1;
	log_.print("unit successfully added");
	return true;
}
