#include "io_benchmark.h"
#include <logger/io_logger.h>

const char* kBenchmarkName = "Benchmark";
// One second
const uint32_t kBenchmarkInterval = 1000 * 1000;

IOLogger benchmarkLogger(kBenchmarkName, &Serial);

void IOBenchmark::setup() {
	nextCheck_.start(kBenchmarkInterval);
}

void IOBenchmark::loop() {
	if (!nextCheck_.finished()) {
		loopsCount_++;
		return;
	}
	benchmarkLogger.builder()
	    ->append(loopsCount_)
	    ->append("")
	    ->append(" lps, ")
		->append(1000000 / loopsCount_)
		->append(" us")
	    ->flush();
	loopsCount_ = 0;
	nextCheck_.start(kBenchmarkInterval);
}

const char* IOBenchmark::name() {
	return kBenchmarkName;
}
