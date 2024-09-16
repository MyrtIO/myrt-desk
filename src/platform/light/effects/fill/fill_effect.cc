#include "fill_effect.h"

const uint8_t kGlowSize = 10;

bool FillEffect::handleFrame(LightState* state, Pixels* pixels) {
	if (state->currentColor == state->targetColor) {
		return false;
	}

	if (progress_.finished()) {
		state->currentColor = state->targetColor;
		pixels->set(state->currentColor);
		return true;
	}

	uint8_t fillSize = scale8(center_, progress_.get());

	for (uint8_t i = 0; i < fillSize; i++) {
		pixels->set(state->targetColor, center_ - i);
	}

	pixels->mirror();

	return true;
}

void FillEffect::onActivate(LightState* state, Pixels* pixels) {
	center_ = pixels->length() / 2;
	progress_.start(state->colorTransitionMs);
}

void FillEffect::onColorUpdate(LightState* state) {
	state->selectedColor = state->targetColor;
	progress_.start(state->colorTransitionMs);
}

FillEffect FillFx;
