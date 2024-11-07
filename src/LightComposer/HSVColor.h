#pragma once

#include <stdint.h>

struct HSVColor {
    uint8_t hue;
    uint8_t saturation;
	uint8_t value;

	constexpr HSVColor(
		uint8_t iHue,
		uint8_t iSaturation,
		uint8_t iValue
	) __attribute__((always_inline)) :
		hue(iHue),
		saturation(iSaturation),
		value(iValue) {}
};
