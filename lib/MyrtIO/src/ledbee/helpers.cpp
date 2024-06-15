#include "helpers.h"

CRGB blend_colors(CRGB first, CRGB second, uint8_t ratio) {
  return CRGB(
    blend8(first.r, second.r, ratio),
    blend8(first.g, second.g, ratio),
    blend8(first.b, second.b, ratio)
  );
}
