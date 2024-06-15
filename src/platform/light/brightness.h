#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>

class SmoothBrightness : public IRenderer {
  public:
    void setup(EffectState *state);

    // Public method to handle frame and update brightness gradually.
    // Returns true if the brightness needs to be updated, false otherwise.
    bool handleFrame();
  
    void handleStateUpdate();

  private:
    uint8_t current_ = 254;
    EffectState *state_;
    Progress transition_;
};
