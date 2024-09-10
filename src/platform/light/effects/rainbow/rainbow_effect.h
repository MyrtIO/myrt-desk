#pragma once

#include "platform/light/light_effect.h"
#include <MyrtIO.h>

class RainbowEffect : public ILightEffect {
  public:
    bool handleFrame(LightState* state, Pixels* pixels);

  private:
    Progress progress_   = Progress();
    uint8_t hueProgress_ = 0;
    HSV firstColor_      = HSV(0, 255, 255);
    HSV secondColor_     = HSV(0, 255, 255);
    HSV thirdColor_      = HSV(0, 255, 255);

    uint8_t hueShift_(uint8_t shift);
    void restart_();
};

extern RainbowEffect RainbowFx;
