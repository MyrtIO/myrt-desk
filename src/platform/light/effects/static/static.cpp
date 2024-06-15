// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#include "static.h"
#include <FastLED.h>

bool StaticEffect::handleFrame(EffectState *state, Strip *strip) {
  if (_transitioned) {
    return false;
  }
  CRGB color = blend_colors(
    state->currentColor,
    state->targetColor,
    _progress.get()
  );
  strip->set(color);
  if (_progress.get() == 255) {
    state->currentColor = state->targetColor;
    _transitioned = true;
  }
  // Serial.printf("Color: %02X%02X%02X\n", color.r, color.g, color.b);
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
  Serial.println("StaticEffect::onColorUpdate");
  _progress.start(state->transition);
  _transitioned = false;
}

void StaticEffect::prepare(EffectState *state, Strip *strip) {
  onColorUpdate(state);
}

StaticEffect Static;
