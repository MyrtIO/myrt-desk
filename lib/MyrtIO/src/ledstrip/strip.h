// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#pragma once

#include <FastLED.h>

class Strip {
 public:
  void mirror();
  void set(CRGB color);
  void set(CRGB color, uint8_t idx);
  void set(CRGB color, uint8_t from, uint8_t to);
  struct CRGB *leds;
  uint8_t length;
};
