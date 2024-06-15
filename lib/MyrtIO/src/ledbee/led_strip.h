#pragma once

#include <FastLED.h>

class LEDStrip {
 public:
  void mirror();
  void set(CRGB color);
  void set(CRGB color, uint8_t idx);
  void set(CRGB color, uint8_t from, uint8_t to);
  struct CRGB *leds;
  uint8_t length;
};
