#pragma once

#include "platform/light/light_effect.h"
#include <MyrtIO.h>
#include <Attotime.h>

class StaticEffect : public ILightEffect {
  public:
    bool handleFrame(LightState* state, Pixels* pixels);
    void onColorUpdate(LightState* state);
    void onEffectUpdate(LightState* state);

  private:
    Progress progress_ = Progress();
    bool forceUpdate_ = false;
};

extern StaticEffect StaticFx;
