#include "static_effect.h"
#include <FastLED.h>

bool StaticEffect::handleFrame(EffectState *state, LEDStrip *strip) {
  if (forceUpdate_) {
    strip->set(state->targetColor);
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
  strip->set(state->currentColor);
  if (progress_.finished()) {
    state->selectedColor = state->targetColor;
  }
  return true;
}

void StaticEffect::onColorUpdate(EffectState *state) {
  state->selectedColor = state->currentColor;
  progress_.start(state->transitionTime);
}

void StaticEffect::onEffectUpdate(EffectState *state) {
  forceUpdate_ = true;
}

StaticEffect StaticFx;
