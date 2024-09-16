#include "io_benchmark.h"
#include "median.h"

const char* kBenchmarkName = "Benchmark";

IOLogger benchmarkLogger(kBenchmarkName, &Serial);

void IOBenchmark::setup() {
	count_ = 1;
	measurements_[0] = micros();
	nextCheck_.start(IO_BENCHMARK_INTERVAL);
}

void IOBenchmark::loop() {
	if (count_ < IO_BENCHMARK_MEASUREMENTS) {
		measurements_[count_] = micros() - measurements_[count_ - 1];
		count_++;
		return;
	}

	if (!nextCheck_.finished()) {
		return;
	}
	uint32_t result = median(measurements_, IO_BENCHMARK_MEASUREMENTS);
	benchmarkLogger.builder()
	    ->append("Loop time: ")
	    ->append(result)
	    ->append(" us")
	    ->flush();
	count_ = 1;
	measurements_[0] = result;
	nextCheck_.start(IO_BENCHMARK_INTERVAL);
}

const char* IOBenchmark::name() {
	return kBenchmarkName;
}
