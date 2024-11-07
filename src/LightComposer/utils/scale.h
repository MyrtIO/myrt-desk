#pragma once

#include <stdint.h>

typedef uint8_t fract_t;

static inline uint8_t scale8(uint8_t x, fract_t scale) {
	return (((uint16_t)x) * (1 + (uint16_t)(scale))) >> 8;
}

static inline uint8_t scale8_video(uint8_t x, fract_t scale) {
	return (((int)x * (int)scale) >> 8) + ((x && scale) ? 1 : 0);
}

static inline uint8_t lerp8by8(uint8_t a, uint8_t b, fract_t frac) {
    uint8_t result;
    if( b > a) {
        uint8_t delta = b - a;
        uint8_t scaled = scale8( delta, frac);
        result = a + scaled;
    } else {
        uint8_t delta = a - b;
        uint8_t scaled = scale8( delta, frac);
        result = a - scaled;
    }
    return result;
}
