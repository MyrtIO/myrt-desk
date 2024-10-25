#include "io_benchmark.h"
#include <logger/io_logger.h>

const char* kBenchmarkName = "Benchmark";
// One second
const uint32_t kBenchmarkInterval = 1000 * 1000;

io::Logger benchmarkLogger(kBenchmarkName);

void io::Benchmark::setup() {
	nextCheck_.start(kBenchmarkInterval);
}

void io::Benchmark::loop() {
	if (!nextCheck_.finished()) {
		loopsCount_++;
		return;
	}
	benchmarkLogger.builder()
	    ->append(loopsCount_)
	    ->append("")
	    ->append(" lps, ")
	    ->append(kBenchmarkInterval / loopsCount_)
	    ->append(" us");
	benchmarkLogger.flush();
	loopsCount_ = 0;
	nextCheck_.start(kBenchmarkInterval);
}

const char* io::Benchmark::name() {
	return kBenchmarkName;
}
