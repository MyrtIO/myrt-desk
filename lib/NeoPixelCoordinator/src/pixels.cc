#include "pixels.h"

void Pixels::mirror() {
  uint8_t center = count_ / 2;
  for (uint8_t i = 0; i < center; i++) {
    colors_[count_ - i] = colors_[i];
  }
  if (count_ % 2 == 0) {
    colors_[center] = colors_[center-1];
  }
}

void Pixels::setup(PioWS2812* ws2812, uint8_t count) {
  ws2812_ = ws2812;
  count_ = count;
  set(RGB::Black);
  ws2812_->setup(colors_, count_);
  ws2812_->show();
}

void Pixels::set(RGB color, uint8_t idx) {
  colors_[idx] = color;
}

void Pixels::set(RGB color, uint8_t from, uint8_t to) {
  for (uint8_t i = from; i < to; i++) {
    this->set(color, i);
  }
}

void Pixels::set(RGB color) {
  this->set(color, 0, count_);
}

RGB* Pixels::colors() {
  return colors_;
}

uint8_t Pixels::length() {
  return count_;
}
