#pragma once

#include "../HSVColor.h"
#include "../RGBColor.h"

// Convert an HSV value to RGB using a visually balanced rainbow.
// This "rainbow" yields better yellow and orange than a straight
// mathematical "spectrum".
void hsv2rgb_rainbow(const struct HSVColor& hsv, struct RGBColor& rgb);
