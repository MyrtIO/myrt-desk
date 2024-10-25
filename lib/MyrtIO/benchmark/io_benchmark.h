#pragma once

#include <io_unit.h>
#include <Attotime.h>

namespace io {
	class Benchmark : public Unit {
	public:
		void setup();
		void loop();
		const char* name();

	private:
		Timer nextCheck_;
		uint32_t loopsCount_ = 0;
	};
}

