#include "color_utils.h"

uint8_t blend8(uint8_t first, uint8_t second, uint8_t ratio) {
  return first + ((second - first) * ratio) / 255;
}


RGB blendColors(RGB first, RGB second, uint8_t ratio) {
  uint8_t r = blend8(first.r, second.r, ratio);
  uint8_t g = blend8(first.g, second.g, ratio);
  uint8_t b = blend8(first.b, second.b, ratio);
  return RGB(r, g, b);
}

uint8_t scale8(uint8_t i, fract8 scale) {
    return (((uint16_t)i) * (1 + (uint16_t)(scale))) >> 8;
}

uint8_t lerp8by8(uint8_t a, uint8_t b, fract8 frac) {
  uint8_t result;
  if(b > a) {
    uint8_t delta = b - a;
    uint8_t scaled = scale8(delta, frac);
    result = a + scaled;
  } else {
    uint8_t delta = a - b;
    uint8_t scaled = scale8(delta, frac);
    result = a - scaled;
  }
  return result;
}
