#pragma once

#include <stdint.h>
#include "entity.h"

namespace HomeAssistant {
	struct NumberConfig {
		const char* name;
		const char* identifier;
		const char* deviceClass = "None";
		const char* icon = nullptr;
		const char* unit = nullptr;
		bool writable = false;
		uint16_t min = 0;
		uint16_t max = 100;
	};

	class NumberEntity: public Entity {
	public:
		NumberEntity(
			const NumberConfig& config,
			const Device& device
		) :	config_(config),
			Entity(
				config.identifier,
				config.name,
				config.icon,
				config.writable,
				device
			) {}

		void fillConfig() {
			json["device_class"] = config_.deviceClass;
			json["min"] = config_.min;
			json["max"] = config_.max;
			if (config_.unit != nullptr) {
				json["unit_of_measurement"] = config_.unit;
			}
		}

		void parseValue(uint16_t& value, byte* payload) {
			value = atoi((char*)payload);
		}

		template <typename TDestination>
		void serializeValue(TDestination& buffer, uint16_t value) {
			itoa(value, buffer, 10);
		}

		const char* component() {
			return "number";
		}
	private:
		NumberConfig config_;
	};
};
