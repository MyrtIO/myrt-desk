#pragma once

#include "light_effect.h"
#include <MyrtIO.h>
#include <NeoPixelCoordinator.h>

enum class BrightnessChangeReason {
    Brightness,
    Power,
    Effect,
};

class EffectSwitcher {
  public:
    virtual void onEffectSwitch() = 0;
};

class SmoothBrightness : public ILEDHandler {
  public:
    void setup(PioWS2812* ws2812, LightState* state, EffectSwitcher* switcher);

    // Public method to handle frame and update brightness gradually.
    // Returns true if the brightness needs to be updated, false otherwise.
    bool handleFrame();

    void handleBrightnessUpdate();
    void handlePowerUpdate();
    void handleEffectUpdate();

  private:
    bool enabled_ = true;
    bool transitioning_ = false;
    bool effectSwitched_ = false;
    uint8_t previous_ = 0;
    uint8_t current_ = 254;
    uint8_t target_ = 255;
    LightState* state_;
    Progress transition_;
    BrightnessChangeReason reason_;
    PioWS2812* ws2812_;

    EffectSwitcher* switcher_ = nullptr;

    bool handleEffectChangeFrame_();
};
