#include "static_effect.h"
#include "LightComposer/utils/blend.h"

bool StaticEffect::handleFrame(StaticEffectState& state, IPixels& pixels) {
	if (forceUpdate_) {
		pixels.set(state.targetColor);
		state.currentColor = state.targetColor;
		forceUpdate_ = false;
		return true;
	}
	if (state.currentColor == state.targetColor) {
		return false;
	}

	state.currentColor = blendColors(state.previousColor, state.targetColor, progress_.get());
	pixels.set(state.currentColor);
	if (progress_.finished()) {
		state.currentColor = state.targetColor;
	}

	return true;
}

void StaticEffect::onColorUpdate(StaticEffectState& state) {
	progress_.start(state.transitionMs);
}

void StaticEffect::onActivate(StaticEffectState& state) {
	forceUpdate_ = true;
}

StaticEffect StaticFx;
