#pragma once

#include "blend.h"

static inline uint8_t blend8(uint8_t a, uint8_t b, fract_t amountOfB) {
    uint16_t partial;
    uint8_t result;

    partial = (a << 8) | b; // A*256 + B

    // on many platforms this compiles to a single multiply of (B-A) * amountOfB
    partial += (b * amountOfB);
    partial -= (a * amountOfB);

    result = partial >> 8;

    return result;
}

static inline RGBColor blendColors(const RGBColor &a, const RGBColor &b, fract_t frac) {
	return RGBColor(blend8(a.r, b.r, frac), blend8(a.g, b.g, frac), blend8(a.b, b.b, frac));
}
