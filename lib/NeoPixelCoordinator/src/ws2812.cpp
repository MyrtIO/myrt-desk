#include "ws2812.h"

PioWS2812::PioWS2812(uint8_t pin, PIO pio, uint sm) {
  sm_ = sm;
  pio_ = pio;
  pin_ = pin;
}

void PioWS2812::setup(RGB* buffer, uint16_t count) {
  buffer_ = buffer;
  count_ = count;
  brightness_ = 255;

  Serial.println("WS2812 setup");
  if (pio_can_add_program(pio1, &ws2812_program)) {
    Serial.println("WS2812 can add program");
  } else {
    Serial.println("WS2812 cannot add program");
  }
  uint offset = pio_add_program(pio1, &ws2812_program);
  ws2812_program_init(pio1, 1, offset, pin_, 800000, false);
  delay(1);
  Serial.println("WS2812 ready");
}

void PioWS2812::set(uint16_t pixelNumber, RGB color) {
  buffer_[pixelNumber] = color;
}

void PioWS2812::setBrightness(uint8_t brightness) {
  brightness_ = brightness;
}

void PioWS2812::show() {
  uint8_t r, g, b;
  uint16_t scale_fixed;
  for (uint16_t i = 0; i < count_; i++) {
    scale_fixed = brightness_ + 1;
    r = (((uint16_t)buffer_[i].red) * scale_fixed) >> 8;
    g = (((uint16_t)buffer_[i].green) * scale_fixed) >> 8;
    b = (((uint16_t)buffer_[i].blue) * scale_fixed) >> 8;
    putPixel_(
      ((uint32_t)(r) << 8) |
      ((uint32_t)(g) << 16) |
      (uint32_t)(b)
    );
  }
}

void PioWS2812::putPixel_(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio1, 1, pixel_grb << 8u);
}
