#pragma once

#include <MyrtIO.h>
#include <LEDCoordinator.h>
#include <NeoPixelCoordinator.h>
#include "light_effect.h"

class PixelHandler : public ILEDHandler {
  public:
    // Setup function to initialize the LED matrix and set the rendering effect.
    void setup(ILightEffect *effect, LightState *state, Pixels* pixels);

    // Public method to handle a frame of animation and rendering.
    // Returns true if rendering is needed.
    bool handleFrame();

    // Set the rendering effect for the LED matrix.
    void setEffect(ILightEffect *effect);

    ILightEffect *getEffect() {
      return effect_;
    }

    // Set the color for the LED matrix.
    void handleStateUpdate();

  private:
    ILightEffect *effect_ = nullptr;
    LightState *state_ = nullptr;
    Pixels* pixels_;
};
