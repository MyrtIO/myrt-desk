#pragma once

#include "crgb_color.h"

RGBColor rgbFromFastLED(CRGB color) {
	return RGBColor(color.r, color.g, color.b);
}

CRGB rgbToFastLED(RGBColor color) {
	return CRGB(color.r, color.g, color.b);
}
