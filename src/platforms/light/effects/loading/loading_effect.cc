#include "loading_effect.h"
#include <LightComposer/utils/scale.h>

const size_t kLoadingCycleDuration = 1000;
const fract_t kLoadingFillFract = 200;

void LoadingEffect::onActivate(LoadingEffectState& state, IPixels& pixels) {
	isReverse_ = false;
	progress_.start(kLoadingCycleDuration);
	fillSize_ = scale8(pixels.count() - 1, kLoadingFillFract);
	maxOffset_ = pixels.count() - 1 - fillSize_;
}

bool LoadingEffect::handleFrame(LoadingEffectState& state, IPixels& pixels) {
	uint8_t shift = scale8(maxOffset_, progress_.get());
	if (state.currentColor != state.targetColor) {
		state.currentColor = state.targetColor;
	}

	pixels.set(RGBColor::Black);
	uint8_t startOffset = isReverse_ ? maxOffset_ - shift : shift;
	for (uint8_t i = 0; i < fillSize_; i++) {
		pixels.set(state.currentColor, startOffset + i);
	}

	if (progress_.finished()) {
		isReverse_ = !isReverse_;
		progress_.start(kLoadingCycleDuration);
	}
	return true;
}

void LoadingEffect::onColorUpdate(LoadingEffectState& state) {
	state.currentColor = state.targetColor;
	progress_.start(state.transitionMs);
}

LoadingEffect LoadingFx;
