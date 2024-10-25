#pragma once

#include <FastLED.h>
#include "../rgb_color.h"

RGBColor rgbFromFastLED(CRGB color);
CRGB rgbToFastLED(RGBColor color);
