#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>
// #include "strip.h"
// #include "effect.h"
// #include "effects/effects.h"
#include <pins.h>

#define STRIP_LENGTH 92
#define STRIP_COLOR_CORRECTION 0xFF7A60

class PixelRenderer : public IRenderer {
  public:
    // Setup function to initialize the LED matrix and set the rendering effect.
    void setup(LEDEffect *effect, EffectState *state);

    // Public method to handle a frame of animation and rendering.
    // Returns true if rendering is needed.
    bool handleFrame();

    // Set the rendering effect for the LED matrix.
    void setEffect(LEDEffect *effect);

    LEDEffect *getEffect() {
      return effect_;
    }

    // Set the color for the LED matrix.
    void handleStateUpdate();

  private:
    LEDEffect *effect_ = nullptr;
    WS2812Strip<PIN_LED_STRIP, STRIP_LENGTH> strip_ = WS2812Strip<PIN_LED_STRIP, STRIP_LENGTH>(
      STRIP_COLOR_CORRECTION
    );
    CRGB pixels_[STRIP_LENGTH];
    EffectState *state_;
};
