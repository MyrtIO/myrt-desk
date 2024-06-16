#include "brightness.h"
#include <FastLED.h>

constexpr size_t kBrightnessTransitionDuration = 300;

void SmoothBrightness::setup(EffectState *state) {
  state_ = state;
  enabled_ = state_->enabled;
}

// Implementation of the public method to handle frame and update brightness gradually.
bool SmoothBrightness::handleFrame() {
  if (transition_.finished()) {
    return false;
  }
  uint8_t progress = transition_.get();
  current_ = lerp8by8(
    previous_,
    target_,
    progress
  );
  if (progress == 255) {
    if (reason_ == BrightnessChangeReason::Power) {
      enabled_ = state_->enabled;
    } else if (reason_ == BrightnessChangeReason::Brightness) {
      state_->currentBrightness = state_->targetBrightness;
    }
  }
  LEDS.setBrightness(current_);
  return true;
}

void SmoothBrightness::handleBrightnessUpdate() {
  if (state_->targetBrightness == current_) {
    return;
  }
  previous_ = state_->currentBrightness;
  target_ = state_->targetBrightness;
  reason_ = BrightnessChangeReason::Brightness;
  transition_.start(state_->transitionTime);
}

void SmoothBrightness::handlePowerUpdate() {
  if (state_->enabled == current_) {
    return;
  }
  previous_ = current_;
  target_ = state_->enabled ? state_->currentBrightness : 0;
  reason_ = BrightnessChangeReason::Power;
  transition_.start(state_->transitionTime);
}
