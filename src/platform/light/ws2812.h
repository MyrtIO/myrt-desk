// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#pragma once

#include <Arduino.h>
#include <MyrtIO.h>

template <uint8_t DATA_PIN, uint8_t LENGTH>
class WS2812Strip : public Strip {
 public:
  explicit WS2812Strip(const struct CRGB & correction) {
    this->leds = new struct CRGB[LENGTH];
    this->length = LENGTH;
    LEDS
      .addLeds<WS2811, DATA_PIN, GRB>(leds, length)
      .setCorrection(correction);
    LEDS.setBrightness(0);
  }
};
