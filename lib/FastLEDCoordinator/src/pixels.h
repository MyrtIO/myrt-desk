#pragma once

#include <FastLED.h>

class Pixels {
public:
  explicit Pixels(uint8_t length) {
    colors_   = new struct CRGB[length];
    length_ = length;
    set(CRGB::Black);
  }
  void mirror();
  void set(CRGB color);
  void set(CRGB color, uint8_t idx);
  void set(CRGB color, uint8_t from, uint8_t to);
  CRGB get(uint8_t idx);
  CRGB* colors();
  uint8_t length();

protected:
  struct CRGB* colors_;
  uint8_t length_;
};
