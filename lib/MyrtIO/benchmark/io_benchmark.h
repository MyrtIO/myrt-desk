#pragma once

#include "MyrtIO.h"
#include <Attotime.h>

#ifndef IO_BENCHMARK_MEASUREMENTS
	#define IO_BENCHMARK_MEASUREMENTS 128
#endif

#ifndef IO_BENCHMARK_INTERVAL
	#define IO_BENCHMARK_INTERVAL 5000
#endif

class IOBenchmark : public IOUnit {
  public:
	void setup();
	void loop();
	const char* name();

  private:
	Timer nextCheck_;
	uint8_t count_ = 0;
	uint32_t measurements_[IO_BENCHMARK_MEASUREMENTS];
};
