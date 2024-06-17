#include "static.h"
#include <FastLED.h>

bool StaticEffect::handleFrame(EffectState *state, LEDStrip *strip) {
  if (_transitioned) {
    return false;
  }
  CRGB color = blendColors(
    state->currentColor,
    state->targetColor,
    _progress.get()
  );
  strip->set(color);
  if (_progress.get() == 255) {
    state->currentColor = state->targetColor;
    _transitioned = true;
  }
  return true;
}

void StaticEffect::finalize(EffectState *state) {
}

bool StaticEffect::setArguments(EffectState *state, uint8_t *args, uint8_t length) {
  return false;
}

bool StaticEffect::setData(EffectState *state, uint8_t *data, uint8_t length) {
  return false;
}

void StaticEffect::onColorUpdate(EffectState *state) {
  _progress.start(state->transitionTime);
  _transitioned = false;
}

StaticEffect StaticFx;
