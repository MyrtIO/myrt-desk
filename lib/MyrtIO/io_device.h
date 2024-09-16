#pragma once

#include <stdint.h>
#include "io_unit.h"

#define IO_DEVICE_MAX_CONTROLLERS 8
#define IO_DEVICE_MAX_PLATFORMS 8

class IODevice {
  public:
	IODevice* setup();

	void inline __attribute__((always_inline)) loop() {
		// First loop over platforms, then controllers.
		// Changes will be applied on next loop.
		for (int i = 0; i < platformsCount_; i++) {
			platforms_[i]->loop();
		}
		for (int i = 0; i < controllersCount_; i++) {
			controllers_[i]->loop();
		}
	}

	template <typename... Args> IODevice* controllers(IOUnit* last) {
		addUnit_(last, controllers_, &controllersCount_);
		return this;
	}

	template <typename... Args>
	IODevice* controllers(IOUnit* first, Args... args) {
		addUnit_(first, controllers_, &controllersCount_);
		return controllers(args...);
	}

	template <typename... Args> IODevice* platform(IOUnit* last) {
		addUnit_(last, platforms_, &platformsCount_);
		return this;
	}

	template <typename... Args>
	IODevice* platform(IOUnit* first, Args... args) {
		addUnit_(first, platforms_, &platformsCount_);
		return platform(args...);
	}

  private:
	uint8_t controllersCount_ = 0;
	uint8_t platformsCount_ = 0;
	IOUnit* controllers_[IO_DEVICE_MAX_CONTROLLERS];
	IOUnit* platforms_[IO_DEVICE_MAX_PLATFORMS];

	bool addUnit_(IOUnit* u, IOUnit** units, uint8_t* count);
};
