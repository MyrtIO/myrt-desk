#include "static.h"
#include <FastLED.h>

bool StaticEffect::handleFrame(EffectState *state, LEDStrip *strip) {
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

StaticEffect StaticFx;
