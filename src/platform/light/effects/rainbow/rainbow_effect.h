#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>
#include <Timing.h>

class RainbowEffect : public LEDEffect {
  public:
    bool handleFrame(EffectState *state, LEDStrip *strip);
    void onColorUpdate(EffectState *state);
    void onEffectUpdate(EffectState *state);

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
