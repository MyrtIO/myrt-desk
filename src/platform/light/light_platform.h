#pragma once

#include <MyrtIO.h>
#include <FastLED.h>
#include "interfaces/platform.h"
#include "brightness.h"
#include "pixel_renderer.h"

class LightPlatform : public IOPlatform, public ILightPlatform {
  public:
    // Setup function to initialize the LED platform.
    void setup();

    // Function called at the start of each loop iteration.
    void onLoop();

    CRGB getColor();

    void setColor(CRGB color);

    // Set the overall brightness of the LEDs.
    void setBrightness(uint8_t brightness);

    uint8_t getBrightness();

    // Set power status of the LEDs.
    void setPower(bool enabled);

    void setEffect(uint8_t effectCode);

    bool getPower();

  private:
    LEDBee leds_;
    EffectState state_;
    SmoothBrightness brightness_;
    PixelRenderer pixels_;
};
