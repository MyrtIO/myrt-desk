#pragma once

#include "MyrtIO.h"
#include "median.h"
#include <Attotime.h>

#define IO_BENCHMARK_MEASUREMENTS 128
#define IO_BENCHMARK_INTERVAL 5000

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
