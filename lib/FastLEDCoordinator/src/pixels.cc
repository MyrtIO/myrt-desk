#include "pixels.h"

void Pixels::mirror() {
  uint8_t center = length_ / 2;
  for (uint8_t i = 0; i < center; i++) {
    colors_[length_ - i] = colors_[i];
  }
  if (length_ % 2 == 0) {
    colors_[center] = colors_[center-1];
  }
}

void Pixels::set(CRGB color, uint8_t idx) {
  colors_[idx] = color;
}

void Pixels::set(CRGB color, uint8_t from, uint8_t to) {
  for (uint8_t i = from; i < to; i++) {
    this->set(color, i);
  }
}

void Pixels::set(CRGB color) {
  this->set(color, 0, length_);
}

CRGB Pixels::get(uint8_t idx) {
  return colors_[idx];
}

CRGB* Pixels::colors() {
  return colors_;
}

uint8_t Pixels::length() {
  return length_;
}
