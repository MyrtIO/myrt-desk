#include "color_utils.h"

CRGB blendColors(CRGB first, CRGB second, uint8_t ratio) {
  uint8_t r = blend8(first.r, second.r, ratio);
  uint8_t g = blend8(first.g, second.g, ratio);
  uint8_t b = blend8(first.b, second.b, ratio);
  return CRGB(r, g, b);
}
