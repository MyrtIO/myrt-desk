#pragma once

#include <stdint.h>
#include "io_unit.h"
#include "logger/io_logger.h"

#define IO_DEVICE_MAX_CONTROLLERS 8
#define IO_DEVICE_MAX_PLATFORMS 8

namespace io {
	class Device {
	public:
		Device(const char* name) :
			name_(name),
			log_(Logger(name)) {};

		Device* setup();

		Logger* log();

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

		template <typename... Args> Device* controllers(Unit* last) {
			addUnit_(last, &controllers_[0], &controllersCount_);
			return this;
		}

		template <typename... Args>
		Device* controllers(Unit* first, Args... args) {
			addUnit_(first, &controllers_[0], &controllersCount_);
			return controllers(args...);
		}

		template <typename... Args> Device* platforms(Unit* last) {
			addUnit_(last, &platforms_[0], &platformsCount_);
			return this;
		}

		template <typename... Args>
		Device* platforms(Unit* first, Args... args) {
			addUnit_(first, &platforms_[0], &platformsCount_);
			return platforms(args...);
		}

	private:
		const char* name_;
		uint8_t controllersCount_ = 0;
		uint8_t platformsCount_ = 0;
		Unit* controllers_[IO_DEVICE_MAX_CONTROLLERS];
		Unit* platforms_[IO_DEVICE_MAX_PLATFORMS];
		Logger log_;

		bool addUnit_(Unit* u, Unit** units, uint8_t* count);
	};

}

