#pragma once

#include <stdint.h>
#include "Entity.h"

namespace HomeAssistant {
	struct LightConfig {
		const char* name;
		const char* identifier;
		const char* icon = nullptr;
		const char** effects;
		const uint16_t effectCount;
		const bool writable = false;
		const uint16_t maxMireds = 400;
		const uint16_t minMireds = 100;
	};

	struct RGBColor {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	enum ColorMode : uint8_t {
		RGBMode = 0,
		ColorTempMode = 1
	};

	struct LightState {
		bool enabled;
		uint16_t brightness;
		uint16_t colorTemp;
		uint8_t effect;
		RGBColor color;
		ColorMode colorMode;
	};

	class LightEntity: public Entity {
	public:
		LightEntity(
			const LightConfig& config,
			const Device& device
		) :	config_(config),
			Entity(
				config.identifier,
				config.name,
				config.icon,
				config.writable,
				device
			) {}

		const char* component() {
			return "light";
		}

		void fillConfig() {
			// TODO: add supports check
			json["schema"] = "json";
			json["brightness"] = true;
			json["effect"] = true;
			json["max_mireds"] = config_.maxMireds;
			json["min_mireds"] = config_.minMireds;
			auto effects = json.createNestedArray("effect_list");
			for (uint16_t i = 0; i < config_.effectCount; i++) {
				effects.add(config_.effects[i]);
			}
			auto colorModes = json.createNestedArray("color_modes");
			colorModes.add("color_temp");
			colorModes.add("rgb");
		}

		void parseValue(LightState& state, byte* payload) {
			json.clear();
			deserializeJson(json, payload);
			state.enabled = json["state"] == "ON";
			state.brightness = json["brightness"];
			auto colorProp = json["color"].as<JsonObject>();
			if (colorProp != nullptr) {
				state.color = {
					.r = colorProp["r"],
					.g = colorProp["g"],
					.b = colorProp["b"]
				};
				state.colorMode = ColorMode::RGBMode;
			} else {
				state.colorMode = ColorMode::ColorTempMode;
				state.colorTemp = json["color_temp"];
			}
			state.effect = 255;
			auto effectProp = json["effect"];
			if (effectProp != nullptr) {
				for (uint8_t i = 0; i < config_.effectCount; i++) {
					if (strcmp(effectProp, config_.effects[i]) == 0) {
						state.effect = i;
						break;
					}
				}
			}
		}

		template <typename TDestination>
		void serializeValue(TDestination& buffer, const LightState& state) {
			json.clear();
			if (state.colorMode == ColorMode::RGBMode) {
				json["color_mode"] = "rgb";
				auto colorProp = json["color"].to<JsonObject>();
				colorProp["r"] = state.color.r;
				colorProp["g"] = state.color.g;
				colorProp["b"] = state.color.b;
			} else {
				json["color_mode"] = "color_temp";
				json["color_temp"] = state.colorTemp;
			}
			json["effect"] = config_.effects[state.effect];
			json["brightness"] = state.brightness;
			json["state"] = state.enabled ? "ON" : "OFF";
			serializeJson(json, buffer);
		}

	private:
		LightConfig config_;
	};
}
