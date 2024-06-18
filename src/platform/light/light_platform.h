#pragma once

#include <MyrtIO.h>
#include <FastLED.h>
#include "interfaces/platform.h"
#include "brightness.h"
#include "pixel_renderer.h"

enum LightPlatformState {
  Off,
  On
};

class LightPlatform :
  public IOPlatform,
  public ILightPlatform,
  public EffectSwitcher {
public:
  void setup();
  void onLoop();

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
  LEDBee leds_;
  EffectState state_;
  SmoothBrightness brightness_;
  PixelRenderer pixels_;
  LEDEffect *nextEffect_;
};
