#pragma once

#include <stdint.h>
#include "HSVColor.h"

struct RGBColor {
	uint8_t r;
	uint8_t g;
	uint8_t b;

	constexpr RGBColor(uint8_t ir, uint8_t ig, uint8_t ib) __attribute__((always_inline))
    : r(ir), g(ig), b(ib)
	{}
    constexpr RGBColor(uint32_t color_code) __attribute__((always_inline))
    :
		r((color_code >> 16) & 0xFF),
		g((color_code >> 8) & 0xFF),
		b((color_code >> 0) & 0xFF) {}
	constexpr RGBColor() __attribute__((always_inline))
    : r(0), g(0), b(0) {}

	inline bool operator== (const RGBColor& other) {
		return (r == other.r) && (g == other.g) && (b == other.b);
	}

	inline bool operator!= (const RGBColor& other) {
		return !(*this == other);
	}

    inline RGBColor& operator= (const RGBColor& other) = default;

    inline RGBColor& operator= (const uint32_t other) {
        r = (other >> 16) & 0xFF;
        g = (other >>  8) & 0xFF;
        b = (other >>  0) & 0xFF;
        return *this;
    }

	enum {
		Black = 0,
		White = 0xFFFFFF
	};
};
