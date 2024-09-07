#pragma once

#include "rgb.h"

typedef uint8_t fract8;

RGB blendColors(RGB first, RGB second, uint8_t ratio);

uint8_t blend8(uint8_t first, uint8_t second, uint8_t ratio);

uint8_t lerp8by8(uint8_t a, uint8_t b, fract8 frac);
