#pragma once

#include "LightComposer/interfaces/LightHAL.h"
#include <config.h>

class WS2812HAL : public ILightHAL {
public:
	WS2812HAL() {};

	void setup(const RGBColor correction);
	uint16_t count();

	void setPixel(const uint16_t index, const RGBColor& color);
	RGBColor getPixel(const uint16_t index);

	void setBrightness(const uint8_t brightness);

	void apply();

	RGBColor* pixels() { return leds_; }

private:
	RGBColor leds_[CONFIG_LIGHT_LED_COUNT];
};
