// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#include "Strip.h"

void Strip::mirror() {
  for (uint8_t i = 0; i < (length / 2); i++) {
    leds[length - i] = leds[i];
  }
}

void Strip::set(CRGB color, uint8_t idx) {
  leds[idx] = color;
}

void Strip::set(CRGB color, uint8_t from, uint8_t to) {
  for (uint8_t i = from; i < to; i++) {
    this->set(color, i);
  }
}

void Strip::set(CRGB color) {
  this->set(color, 0, length);
}
