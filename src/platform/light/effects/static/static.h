#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>
#include <Timing.h>

class StaticEffect : public LEDEffect {
 public:
  bool handleFrame(EffectState *state, LEDStrip *strip);
  void onColorUpdate(EffectState *state);

 private:
  bool transitioned_ = false;
  Progress progress_ = Progress();
};

extern StaticEffect StaticFx;
