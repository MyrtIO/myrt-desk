#include "brightness_handler.h"
#include <FastLED.h>

constexpr size_t kBrightnessTransitionDuration = 300;

void SmoothBrightness::setup(LightState* state, EffectSwitcher* switcher) {
	state_ = state;
	switcher_ = switcher;
	enabled_ = state_->enabled;
}

// Implementation of the public method to handle frame and update brightness
// gradually.
bool SmoothBrightness::handleFrame() {
	if (!transitioning_) {
		return false;
	}
	if (reason_ == BrightnessChangeReason::Effect) {
		return handleEffectChangeFrame_();
	}

	current_ = lerp8by8(previous_, target_, transition_.get());
	if (transition_.finished()) {
		switch (reason_) {
		case BrightnessChangeReason::Brightness:
			state_->currentBrightness = state_->targetBrightness;
			break;

		case BrightnessChangeReason::Power:
			enabled_ = state_->enabled;
			break;
		}

		transitioning_ = false;
	}

	FastLED.setBrightness(current_);
	return true;
}

void SmoothBrightness::handleBrightnessUpdate() {
	if (state_->targetBrightness == current_) {
		return;
	}

	transitioning_ = true;
	previous_ = state_->currentBrightness;
	target_ = state_->targetBrightness;
	reason_ = BrightnessChangeReason::Brightness;
	transition_.start(state_->colorTransitionMs);
}

void SmoothBrightness::handlePowerUpdate() {
	if (state_->enabled == current_) {
		return;
	}

	transitioning_ = true;
	previous_ = current_;
	target_ = state_->enabled ? state_->currentBrightness : 0;
	reason_ = BrightnessChangeReason::Power;
	transition_.start(state_->colorTransitionMs);
}

void SmoothBrightness::handleEffectUpdate() {
	previous_ = current_;
	target_ = state_->currentBrightness;
	reason_ = BrightnessChangeReason::Effect;
	effectSwitched_ = false;
	transitioning_ = true;
	transition_.start(state_->effectTransitionMs);
}

bool SmoothBrightness::handleEffectChangeFrame_() {
	uint8_t progress = transition_.get();

	if (progress < 128) {
		previous_ = state_->currentBrightness;
		target_ = 0;
		progress = map(progress, 0, 127, 0, 255);
	} else {
		if (!effectSwitched_) {
			switcher_->onEffectSwitch();
			effectSwitched_ = true;
		}

		progress = map(progress, 128, 255, 0, 255);
		previous_ = 0;
		target_ = state_->currentBrightness;
	}

	current_ = lerp8by8(previous_, target_, progress);
	FastLED.setBrightness(current_);
	return true;
}
