#include "brightness.h"
#include <FastLED.h>

constexpr size_t kBrightnessTransitionDuration = 300;

void SmoothBrightness::setup(EffectState *state) {
  state_ = state;
}

// Implementation of the public method to handle frame and update brightness gradually.
bool SmoothBrightness::handleFrame() {
  if (current_ == state_->targetBrightness) {
    return false;
  }
  current_ = lerp8by8(
    state_->currentBrightness,
    state_->targetBrightness,
    transition_.get()
  );
  if (current_ == state_->targetBrightness) {
    state_->currentBrightness = state_->targetBrightness;
  }
  LEDS.setBrightness(current_);
  return true;
}

void SmoothBrightness::handleStateUpdate() {
  if (state_->targetBrightness == current_) {
    return;
  }
  transition_.start(state_->transitionTime);
}
