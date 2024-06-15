#pragma once

#include <FastLED.h>
#include "strip.h"

struct EffectState {
  bool enabled = false;
  uint8_t brightness = 255;
  uint8_t fps = 50;
  size_t transition = 400;
  CRGB currentColor = CRGB::Black;
  CRGB targetColor = CRGB::Blue;
};

class StripEffect {
 public:
  virtual bool handleFrame(EffectState *state, Strip *strip) = 0;
  virtual void finalize(EffectState *state) = 0;
  virtual void onColorUpdate(EffectState *state) = 0;;
  virtual void prepare(EffectState *state, Strip *strip) = 0;
};
