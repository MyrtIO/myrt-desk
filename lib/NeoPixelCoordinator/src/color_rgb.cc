#include "color_rgb.h"

uint8_t scale8_video(uint8_t i, fract8 scale) {
    return (((int)i * (int)scale) >> 8) + ((i && scale) ? 1 : 0);
}

uint8_t scale8(uint8_t i, fract8 scale) {
    return (((uint16_t)i) * (1 + (uint16_t)(scale))) >> 8;
}

uint8_t blend8(uint8_t first, uint8_t second, uint8_t ratio) {
    return first + ((second - first) * ratio) / 255;
}

RGB blendColors(RGB first, RGB second, uint8_t ratio) {
    uint8_t r = blend8(first.r, second.r, ratio);
    uint8_t g = blend8(first.g, second.g, ratio);
    uint8_t b = blend8(first.b, second.b, ratio);
    return RGB(r, g, b);
}

uint8_t lerp8by8(uint8_t a, uint8_t b, fract8 frac) {
    uint8_t result;
    if (b > a) {
        uint8_t delta  = b - a;
        uint8_t scaled = scale8(delta, frac);
        result         = a + scaled;
    } else {
        uint8_t delta  = a - b;
        uint8_t scaled = scale8(delta, frac);
        result         = a - scaled;
    }
    return result;
}

void hsv2rgb_rainbow(const HSV& hsv, RGB& rgb) {
}

void fill_gradient(RGB* targetArray, uint16_t startpos, HSV startcolor, uint16_t endpos, HSV endcolor, TGradientDirectionCode directionCode) {
    // if the points are in the wrong order, straighten them
    if (endpos < startpos) {
        uint16_t t = endpos;
        HSV tc     = endcolor;
        endcolor   = startcolor;
        endpos     = startpos;
        startpos   = t;
        startcolor = tc;
    }

    if (endcolor.value == 0 || endcolor.saturation == 0) {
        endcolor.hue = startcolor.hue;
    }

    // Similarly, if we're fading in from black (val=0) or white (sat=0)
    // then set the starthue to the endhue.
    // This lets us ramp smoothly up from black or white, regardless
    // of what 'hue' was set in the startcolor (since it doesn't matter)
    if (startcolor.value == 0 || startcolor.saturation == 0) {
        startcolor.hue = endcolor.hue;
    }

    saccum87 huedistance87;
    saccum87 satdistance87;
    saccum87 valdistance87;

    satdistance87 = (endcolor.saturation - startcolor.saturation) << 7;
    valdistance87 = (endcolor.value - startcolor.value) << 7;

    uint8_t huedelta8 = endcolor.hue - startcolor.hue;

    if (directionCode == SHORTEST_HUES) {
        directionCode = FORWARD_HUES;
        if (huedelta8 > 127) {
            directionCode = BACKWARD_HUES;
        }
    }

    if (directionCode == LONGEST_HUES) {
        directionCode = FORWARD_HUES;
        if (huedelta8 < 128) {
            directionCode = BACKWARD_HUES;
        }
    }

    if (directionCode == FORWARD_HUES) {
        huedistance87 = huedelta8 << 7;
    } else {
        huedistance87 = (uint8_t)(256 - huedelta8) << 7;
        huedistance87 = -huedistance87;
    }

    uint16_t pixeldistance = endpos - startpos;
    int16_t divisor        = pixeldistance ? pixeldistance : 1;

// #define FASTLED_USE_32_BIT_GRADIENT_FILL true
#if FASTLED_USE_32_BIT_GRADIENT_FILL
    // Use higher precision 32 bit math for new micros.
    int32_t huedelta823 = (huedistance87 * 65536) / divisor;
    int32_t satdelta823 = (satdistance87 * 65536) / divisor;
    int32_t valdelta823 = (valdistance87 * 65536) / divisor;

    huedelta823 *= 2;
    satdelta823 *= 2;
    valdelta823 *= 2;
    uint32_t hue824 = static_cast<uint32_t>(startcolor.hue) << 24;
    uint32_t sat824 = static_cast<uint32_t>(startcolor.saturation) << 24;
    uint32_t val824 = static_cast<uint32_t>(startcolor.value) << 24;
    for (uint16_t i = startpos; i <= endpos; ++i) {
        targetArray[i] = HSV(hue824 >> 24, sat824 >> 24, val824 >> 24);
        hue824 += huedelta823;
        sat824 += satdelta823;
        val824 += valdelta823;
    }
#else
    // Use 8-bit math for older micros.
    saccum87 huedelta87 = huedistance87 / divisor;
    saccum87 satdelta87 = satdistance87 / divisor;
    saccum87 valdelta87 = valdistance87 / divisor;

    huedelta87 *= 2;
    satdelta87 *= 2;
    valdelta87 *= 2;

    accum88 hue88 = startcolor.hue << 8;
    accum88 sat88 = startcolor.saturation << 8;
    accum88 val88 = startcolor.value << 8;
    for (uint16_t i = startpos; i <= endpos; ++i) {
        targetArray[i] = HSV(hue88 >> 8, sat88 >> 8, val88 >> 8);
        hue88 += huedelta87;
        sat88 += satdelta87;
        val88 += valdelta87;
    }
#endif // defined(__AVR__)
}

void fill_gradient(RGB* targetArray, uint16_t numLeds, const HSV& c1, const HSV& c2, TGradientDirectionCode directionCode) {
    uint16_t last = numLeds - 1;
    fill_gradient(targetArray, 0, c1, last, c2, directionCode);
}

void fill_gradient(RGB* targetArray, uint16_t numLeds, const HSV& c1, const HSV& c2, const HSV& c3, TGradientDirectionCode directionCode) {
    uint16_t half = (numLeds / 2);
    uint16_t last = numLeds - 1;
    fill_gradient(targetArray, 0, c1, half, c2, directionCode);
    fill_gradient(targetArray, half, c2, last, c3, directionCode);
}

void fill_gradient(
RGB* targetArray,
uint16_t numLeds,
const HSV& c1,
const HSV& c2,
const HSV& c3,
const HSV& c4,
TGradientDirectionCode directionCode
) {
    uint16_t onethird  = (numLeds / 3);
    uint16_t twothirds = ((numLeds * 2) / 3);
    uint16_t last      = numLeds - 1;
    fill_gradient(targetArray, 0, c1, onethird, c2, directionCode);
    fill_gradient(targetArray, onethird, c2, twothirds, c3, directionCode);
    fill_gradient(targetArray, twothirds, c3, last, c4, directionCode);
}
