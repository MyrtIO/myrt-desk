#include "static_effect.h"

bool StaticEffect::handleFrame(LightState *state, Pixels *pixels) {
  if (forceUpdate_) {
    pixels->set(state->targetColor);
    state->selectedColor = state->targetColor;
    state->currentColor = state->targetColor;
    forceUpdate_ = false;
    return true;
  }
  if (state->selectedColor == state->targetColor) {
    return false;
  }
  state->currentColor = blendColors(
    state->selectedColor,
    state->targetColor,
    progress_.get()
  );
  pixels->set(state->currentColor);
  if (progress_.finished()) {
    state->selectedColor = state->targetColor;
  }
  return true;
}

void StaticEffect::onColorUpdate(LightState *state) {
  state->selectedColor = state->currentColor;
  progress_.start(state->colorTransitionMs);
}

void StaticEffect::onEffectUpdate(LightState *state) {
  forceUpdate_ = true;
}

StaticEffect StaticFx;
