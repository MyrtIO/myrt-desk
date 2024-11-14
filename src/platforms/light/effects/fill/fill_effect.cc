#include "fill_effect.h"
#include <LightComposer/math/scale.h>

const uint8_t kGlowSize = 10;

bool FillEffect::handleFrame(FillEffectState& state, IPixels& pixels) {
	if (state.currentColor == state.targetColor) {
		return false;
	}

	if (progress_.finished()) {
		state.currentColor = state.targetColor;
		pixels.set(state.currentColor);
		return true;
	}

	uint8_t center = pixels.center();
	uint8_t fillSize = scale8(center, progress_.get());

	for (uint8_t i = 0; i < fillSize; i++) {
		pixels.set(state.targetColor, center - i);
	}

	pixels.mirror();
	return true;
}

void FillEffect::onActivate(FillEffectState& state, IPixels& pixels) {}

void FillEffect::onColorUpdate(FillEffectState& state) {
	progress_.start(state.transitionMs);
}

FillEffect FillFx;
