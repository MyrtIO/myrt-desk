#pragma once

#include <MyrtIO.h>
#include <FastLED.h>
// #include "strip.h"
// #include "effect.h"
#include "ws2812.h"
// #include "effects/effects.h"
// #include "pins.h"

#define STRIP_LENGTH 92
#define STRIP_PIN 6

class LEDRenderer : public IRenderer {
  public:
    // Setup function to initialize the LED matrix and set the rendering effect.
    void setup(StripEffect *effect);

    // Public method to handle a frame of animation and rendering.
    // Returns true if rendering is needed.
    bool handleFrame();

    // Set the rendering effect for the LED matrix.
    void setEffect(StripEffect *effect);

    // Set the color for the LED matrix.
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    void setPower(bool state);

    EffectState *state();

  private:
    StripEffect *effect_ = nullptr;
    WS2812Strip<STRIP_PIN, STRIP_LENGTH> strip_ = WS2812Strip<STRIP_PIN, STRIP_LENGTH>(0xFF7A60);
    CRGB pixels_[STRIP_LENGTH];
    EffectState state_;
};
