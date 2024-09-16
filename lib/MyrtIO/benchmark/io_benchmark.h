#pragma once

#include <io_unit.h>
#include <Attotime.h>

class IOBenchmark : public IOUnit {
  public:
	void setup();
	void loop();
	const char* name();

  private:
	Timer nextCheck_;
	uint32_t loopsCount_ = 0;
};
