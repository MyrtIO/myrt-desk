#pragma once

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"

#include "rgb.h"
#include "ws2812.pio.h"


class PioWS2812 {
  public:
    PioWS2812(uint8_t pin, PIO pio, uint sm);
    virtual ~PioWS2812(){};

    void setup(RGB* buffer, uint16_t count);
    void set(uint16_t pixel_number, RGB color);
    void setBrightness(uint8_t brightness);
    void show();

  private:
    PIO pio_;
    uint sm_;
    uint16_t count_;
    RGB* buffer_;
    uint8_t pin_;
    uint8_t brightness_;

    void init_(byte pinNumber, uint16_t pixelsCount);
    void putPixel_(uint32_t pixel_grb);
};
