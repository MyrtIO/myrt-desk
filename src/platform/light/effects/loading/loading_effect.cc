#include "loading_effect.h"

const size_t kLoadingCycleDuration = 1000;
const fract8 kLoadingFillFract = 200;

void LoadingEffect::onActivate(LightState* state, Pixels* pixels) {
	isReverse_ = false;
	progress_.start(kLoadingCycleDuration);
	fillSize_ = scale8(pixels->length(), kLoadingFillFract);
	maxOffset = pixels->length() - fillSize_;
}

bool LoadingEffect::handleFrame(LightState* state, Pixels* pixels) {
	uint8_t shift = scale8(maxOffset, progress_.get());
	state->currentColor = state->targetColor;
	fill_(pixels, state, shift, fillSize_);

	if (progress_.finished()) {
		isReverse_ = !isReverse_;
		progress_.start(kLoadingCycleDuration);
	}

	return true;
}

void LoadingEffect::onColorUpdate(LightState* state) {
	state->selectedColor = state->targetColor;
	progress_.start(state->colorTransitionMs);
}

void LoadingEffect::fill_(Pixels* pixels, LightState* state, uint8_t start,
    uint8_t length) {
	pixels->set(CRGB::Black);
	uint8_t startOffset = isReverse_ ? maxOffset - start : start;

	for (uint8_t i = 0; i < length; i++) {
		pixels->set(state->currentColor, startOffset + i);
	}
}

LoadingEffect LoadingFx;
