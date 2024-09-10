#pragma once

#include "brightness_handler.h"
#include "light_effect.h"
#include "pixel_handler.h"
#include <MyrtIO.h>
#include <FastLED.h>
#include <config.h>
#include <interfaces/platform.h>
#include "fastled/renderer.h"

class LightPlatform : public IOUnit, public ILightPlatform, public EffectSwitcher {
  public:
    void setup();
    void loop();

    const char* name();

    CRGB getColor();
    void setColor(CRGB color);

    void setBrightness(uint8_t brightness);
    uint8_t getBrightness();

    void setPower(bool enabled);
    bool getPower();

    void onEffectSwitch();
    bool setEffect(uint8_t effectCode);
    uint8_t getEffect();

  private:
    LEDCoordinator coordinator_;
    ILightEffect* nextEffect_ = nullptr;
    LightState state_;
    SmoothBrightness brightnessHandler_;
    PixelHandler pixelHandler_;
    Pixels pixels_;
    FastLEDRenderer renderer_;
};
