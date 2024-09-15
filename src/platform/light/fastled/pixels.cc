#include "pixels.h"
#include <config.h>

void Pixels::mirror() {
    uint8_t center = count_ / 2;
    for (uint8_t i = 0; i < center; i++) {
        colors_[count_ - i] = colors_[i];
    }
    if (count_ % 2 == 0) {
        colors_[center] = colors_[center - 1];
    }
}

void Pixels::setup(uint8_t count) {
    count_ = count;
    this->set(CRGB::Black);
    FastLED.addLeds<WS2812B, CONFIG_PIN_LED_CTL, GRB>(colors_, count_);
    FastLED.setCorrection(CONFIG_LIGHT_COLOR_CORRECTION);
    FastLED.show();
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
    this->set(color, 0, count_);
}

CRGB* Pixels::colors() {
    return colors_;
}

uint8_t Pixels::length() {
    return count_;
}
