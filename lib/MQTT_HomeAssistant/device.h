#pragma once

#include <stdint.h>
#include "json_document.h"

namespace HomeAssistant {
	class Device {
	public:
		const char* name;
		const char* id;
		const char* mqttNamespace;

		Device(
			const char* name,
			const char* id,
			const char* mqttNamespace
		) :	name(name),
			id(id),
			mqttNamespace(mqttNamespace) {}

		void fillConfig() {
			auto deviceObj = json.createNestedObject("device");
			deviceObj["name"] = name;
			auto identifiers = deviceObj.createNestedArray("identifiers");
			identifiers.add(id);
		}
	};
};
