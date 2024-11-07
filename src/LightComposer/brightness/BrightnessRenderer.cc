#include "BrightnessRenderer.h"
#include "../utils/scale.h"

const unsigned int kBrightnessTransitionDuration = 800;

void BrightnessRenderer::attachComposer(IEffectChanger* composer, ILightHAL* hal) {
	composer_ = composer;
	hal_ = hal;
}

bool BrightnessRenderer::renderFrame() {
	if (transition_.finished() && currentValue_ == targetValue_) {
		return false;
	}
	uint8_t progress = transition_.get();
	if (reason_ == BrightnessChangeReason::Effect) {
		if (progress < 128) {
			progress = map(progress, 0, 127, 0, 255);
			previousValue_ = selectedValue_;
			targetValue_ = 0;
		} else {
			if (!effectSwitched_) {
				composer_->onEffectChange();
				effectSwitched_ = true;
			}
			progress = map(progress, 128, 255, 0, 255);
			previousValue_ = 0;
			targetValue_ = selectedValue_;
		}
	}
	currentValue_ = lerp8by8(previousValue_, targetValue_, progress);
	hal_->setBrightness(currentValue_);
	return true;
}

void BrightnessRenderer::setBrightness(uint8_t value) {
	if (selectedValue_ == value) {
		return;
	}

	transitioning_ = true;
	previousValue_ = selectedValue_;
	selectedValue_ = value;
	targetValue_ = value;
	reason_ = BrightnessChangeReason::Brightness;
	transition_.start(kBrightnessTransitionDuration);
}

void BrightnessRenderer::setPower(bool enabled) {
	if (enabled_ == enabled) {
		return;
	}

	enabled_ = enabled;
	transitioning_ = true;
	if (enabled) {
		previousValue_ = 0;
		targetValue_ = selectedValue_;
	} else {
		previousValue_ = selectedValue_;
		targetValue_ = 0;
	}
	reason_ = BrightnessChangeReason::Power;
	transition_.start(kBrightnessTransitionDuration);
}

void BrightnessRenderer::requestEffectChange() {
	effectSwitched_ = false;
	reason_ = BrightnessChangeReason::Effect;
	transition_.start(kBrightnessTransitionDuration);
}

uint8_t BrightnessRenderer::getBrightness() {
	return selectedValue_;
}

bool BrightnessRenderer::getPower() {
	return enabled_;
}
