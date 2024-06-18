#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>
#include <PicoTime.h>

class StaticEffect : public LEDEffect {
  public:
    bool handleFrame(EffectState *state, LEDStrip *strip);
    void onColorUpdate(EffectState *state);
    void onEffectUpdate(EffectState *state);

  private:
    Progress progress_ = Progress();
    bool forceUpdate_ = false;
};

extern StaticEffect StaticFx;
