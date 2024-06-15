#pragma once

#include <MyrtIO.h>
#include <FastLED.h>
#include "interfaces/platform.h"
#include "brightness.h"
#include "renderer.h"

class LEDPlatform : public IOPlatform, public ILEDPlatform {
  public:
    // Setup function to initialize the LED platform.
    void setup();

    // Function called at the start of each loop iteration.
    void onLoop();

    CRGB getColor();

    void setColor(CRGB color);

    uint8_t brightness();

    // Set the overall brightness of the LEDs.
    void setBrightness(uint8_t brightness);

    // Set power status of the LEDs.
    void setPower(bool enabled);

    void setEffect(uint8_t effectCode);

    bool getPower();

  private:
    LEDBee leds_;
    SmoothBrightness brightness_;
    LEDRenderer pixels_;
};
