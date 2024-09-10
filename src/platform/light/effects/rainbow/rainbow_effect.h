#pragma once

#include "platform/light/light_effect.h"
#include <Attotime.h>
#include <MyrtIO.h>

class RainbowEffect : public ILightEffect {
  public:
    bool handleFrame(LightState* state, Pixels* pixels);

  private:
    Progress progress_ = Progress();
    uint8_t hueProgress_ = 0;
    CHSV firstColor_ = CHSV(0, 255, 255);
    CHSV secondColor_ = CHSV(0, 255, 255);
    CHSV thirdColor_ = CHSV(0, 255, 255);

    uint8_t hueShift_(uint8_t shift);
    void restart_();
};

extern RainbowEffect RainbowFx;
