/**
 * Ported from FastLED (https://github.com/FastLED/FastLED/blob/master/src/colorutils.h)
 */

#pragma once

#include "../HSVColor.h"
#include "../RGBColor.h"
#include "hsv2rgb.h"

/**
 * Hue direction for calculating fill gradients.
 * Since "hue" is a value around a color wheel, there are always two directions
 * to sweep from one hue to another.
 */
typedef enum {
    FORWARD_HUES,   // Hue always goes clockwise around the color wheel
    BACKWARD_HUES,  // Hue always goes counter-clockwise around the color wheel
    SHORTEST_HUES,  // Hue goes whichever way is shortest
    LONGEST_HUES    // Hue goes whichever way is longest
} gradient_direction_t;

// inline void fillGradient(

inline void fillGradient(
	RGBColor* targetArray,
    uint16_t startPos,
	HSVColor startColor,
    uint16_t endPos,
	HSVColor endColor,
    gradient_direction_t directionCode  = SHORTEST_HUES )
{
    // if the points are in the wrong order, straighten them
    if (endPos < startPos) {
        uint16_t t = endPos;
        HSVColor tc = endColor;
        endColor = startColor;
        endPos = startPos;
        startPos = t;
        startColor = tc;
    }

    if (endColor.value == 0 || endColor.saturation == 0 ) {
        endColor.hue = startColor.hue;
    }

    // Similarly, if we're fading in from black (val=0) or white (sat=0)
    // then set the startHue to the endHue.
    // This lets us ramp smoothly up from black or white, regardless
    // of what 'hue' was set in the startColor (since it doesn't matter)
    if (startColor.value == 0 || startColor.saturation == 0) {
        startColor.hue = endColor.hue;
    }

    int16_t hueDistance87;
    int16_t satDistance87;
    int16_t valDistance87;

    satDistance87 = (endColor.saturation - startColor.saturation) << 7;
    valDistance87 = (endColor.value - startColor.value) << 7;

    uint8_t hueDelta = endColor.hue - startColor.hue;

    if (directionCode == SHORTEST_HUES) {
        directionCode = FORWARD_HUES;
        if (hueDelta > 127) {
            directionCode = BACKWARD_HUES;
        }
    }

    if (directionCode == LONGEST_HUES ) {
        directionCode = FORWARD_HUES;
        if (hueDelta < 128) {
            directionCode = BACKWARD_HUES;
        }
    }

    if (directionCode == FORWARD_HUES) {
        hueDistance87 = hueDelta << 7;
    }
    else {
        hueDistance87 = (uint8_t)(256 - hueDelta) << 7;
        hueDistance87 = -hueDistance87;
    }

    uint16_t pixelDistance = endPos - startPos;
    int16_t divisor = pixelDistance ? pixelDistance : 1;

    // Use 8-bit math for older micros.
    int16_t hueDelta7 = hueDistance87 / divisor;
    int16_t satDelta87 = satDistance87 / divisor;
    int16_t valDelta87 = valDistance87 / divisor;

    hueDelta7 *= 2;
    satDelta87 *= 2;
    valDelta87 *= 2;

    int32_t hueDelta823 = (hueDistance87 * 65536) / divisor;
    int32_t satDelta823 = (satDistance87 * 65536) / divisor;
    int32_t valDelta823 = (valDistance87 * 65536) / divisor;

    hueDelta823 *= 2;
    satDelta823 *= 2;
    valDelta823 *= 2;
    uint32_t hue824 = static_cast<uint32_t>(startColor.hue) << 24;
    uint32_t sat824 = static_cast<uint32_t>(startColor.saturation) << 24;
    uint32_t val824 = static_cast<uint32_t>(startColor.value) << 24;
    for(uint16_t i = startPos; i <= endPos; ++i) {
		hsv2rgb_rainbow(
			HSVColor( hue824 >> 24, sat824 >> 24, val824 >> 24),
			targetArray[i]
		);
        hue824 += hueDelta823;
        sat824 += satDelta823;
        val824 += valDelta823;
    }
}

inline void fillGradient(
	RGBColor* targetArray,
	uint16_t numLeds,
	const HSVColor& c1,
	const HSVColor& c2,
	const HSVColor& c3,
	gradient_direction_t directionCode = SHORTEST_HUES
) {
    uint16_t half = (numLeds / 2);
    uint16_t last = numLeds - 1;
    fillGradient(targetArray,    0, c1, half, c2, directionCode);
    fillGradient(targetArray, half, c2, last, c3, directionCode);
}

inline void fillGradient(
	RGBColor* targetArray,
	uint16_t numLeds,
	const HSVColor& c1,
	const HSVColor& c2,
	const HSVColor& c3,
	const HSVColor& c4,
	gradient_direction_t directionCode = SHORTEST_HUES
) {
    uint16_t oneThird = (numLeds / 3);
    uint16_t twoThirds = ((numLeds * 2) / 3);
    uint16_t last = numLeds - 1;
    fillGradient(targetArray,         0, c1,  oneThird, c2, directionCode);
    fillGradient(targetArray,  oneThird, c2, twoThirds, c3, directionCode);
    fillGradient(targetArray, twoThirds, c3,      last, c4, directionCode);
}
