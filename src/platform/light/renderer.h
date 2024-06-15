#pragma once

#include <MyrtIO.h>
#include <MyrtIO/ledbee.h>
// #include "strip.h"
// #include "effect.h"
// #include "effects/effects.h"
// #include "pins.h"

#define STRIP_LENGTH 92
#define STRIP_PIN 6
#define STRIP_COLOR_CORRECTION 0xFF7A60

class LEDRenderer : public IRenderer {
  public:
    // Setup function to initialize the LED matrix and set the rendering effect.
    void setup(LEDEffect *effect);

    // Public method to handle a frame of animation and rendering.
    // Returns true if rendering is needed.
    bool handleFrame();

    // Set the rendering effect for the LED matrix.
    void setEffect(LEDEffect *effect);

    // Set the color for the LED matrix.
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    void setPower(bool state);

    EffectState *state();

  private:
    LEDEffect *effect_ = nullptr;
    WS2812Strip<STRIP_PIN, STRIP_LENGTH> strip_ = WS2812Strip<STRIP_PIN, STRIP_LENGTH>(
      STRIP_COLOR_CORRECTION
    );
    CRGB pixels_[STRIP_LENGTH];
    EffectState state_;
};
