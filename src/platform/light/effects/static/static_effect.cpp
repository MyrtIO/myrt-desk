#include "static_effect.h"
#include <FastLED.h>

bool StaticEffect::handleFrame(EffectState *state, LEDStrip *strip) {
  if (forceUpdate_) {
    strip->set(state->targetColor);
    forceUpdate_ = false;
    return true;
  }
  if (state->currentColor == state->targetColor) {
    return false;
  }
  CRGB color = blendColors(
    state->currentColor,
    state->targetColor,
    progress_.get()
  );
  strip->set(color);
  if (progress_.finished()) {
    state->currentColor = state->targetColor;
  }
  return true;
}

void StaticEffect::onColorUpdate(EffectState *state) {
  progress_.start(state->transitionTime);
}

void StaticEffect::onEffectUpdate(EffectState *state) {
  forceUpdate_ = true;
}

StaticEffect StaticFx;
