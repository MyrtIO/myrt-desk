#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>
#include <Timing.h>

class StaticEffect : public LEDEffect {
 public:
  bool handleFrame(EffectState *state, LEDStrip *strip);
  void finalize(EffectState *state);
  void onColorUpdate(EffectState *state);
  void prepare(EffectState *state, LEDStrip *strip);
  bool setArguments(EffectState *state, uint8_t *args, uint8_t length);
  bool setData(EffectState *state, uint8_t *data, uint8_t length);

 private:
  bool _transitioned = false;
  Progress _progress = Progress();
};

extern StaticEffect StaticFx;
