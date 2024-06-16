#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>

enum class BrightnessChangeReason {
  Brightness,
  Power
};

class SmoothBrightness : public IRenderer {
  public:
    void setup(EffectState *state);

    // Public method to handle frame and update brightness gradually.
    // Returns true if the brightness needs to be updated, false otherwise.
    bool handleFrame();

    void handleBrightnessUpdate();
    void handlePowerUpdate();

  private:
    bool enabled_ = true;
    uint8_t previous_ = 0;
    uint8_t current_ = 254;
    uint8_t target_ = 255;
    EffectState *state_;
    Progress transition_;
    BrightnessChangeReason reason_;
};
