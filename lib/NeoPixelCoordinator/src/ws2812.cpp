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
  uint offset = pio_add_program(pio1, &ws2812_program);
  ws2812_program_init(pio1, 1, offset, pin_, 800000, false);
  delay(1);
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
  RGB pixel;
  for (uint16_t i = 0; i < count_; i++) {
    pixel = buffer_[i];
    pixel.applyBrightness(brightness_);
    putPixel_(pixel.u32());
  }
}

void PioWS2812::putPixel_(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio1, 1, pixel_grb << 8u);
}
