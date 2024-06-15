#include "brightness.h"
#include <FastLED.h>

constexpr size_t kBrightnessTransitionDuration = 300;

// Implementation of the public method to handle frame and update brightness gradually.
bool SmoothBrightness::handleFrame() {
  if (current_ == target_) {
    return false;
  }
  current_ = lerp8by8(previous_, target_, transition_.get());
  LEDS.setBrightness(current_);
  return true;
}

// Implementation of the public method to set the target brightness level and initiate a gradual transition.
void SmoothBrightness::setBrightness(uint8_t value) {
  if (value == current_) {
    return;
  }
  transition_.start(kBrightnessTransitionDuration);
  previous_ = current_;
  target_ = value;
}

// Implementation of the public method to set the target brightness level immediately without transition.
void SmoothBrightness::setBrightnessImmediately(uint8_t value) {
  previous_ = value;
  target_ = value;
  current_ = value;
  mustRender_ = true;
  LEDS.setBrightness(value);
}

uint8_t SmoothBrightness::current() {
  return current_;
}
