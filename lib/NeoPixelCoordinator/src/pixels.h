#pragma once

#include "ws2812.h"
#include <stdint.h>

#define PIXELS_MAX_COUNT 128

class Pixels {
  public:
    void setup(PioWS2812* ws2812, uint8_t count);
    void mirror();
    void set(RGB color);
    void set(RGB color, uint8_t idx);
    void set(RGB color, uint8_t from, uint8_t to);
    RGB* colors();
    uint8_t length();

  protected:
    PioWS2812* ws2812_;
    RGB colors_[PIXELS_MAX_COUNT];
    uint8_t count_;
};
