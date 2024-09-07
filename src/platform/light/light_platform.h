#pragma once

#include <MyrtIO.h>
#include <NeoPixelCoordinator.h>
#include "interfaces/platform.h"
#include "brightness_handler.h"
#include "pixel_handler.h"

#define STRIP_LENGTH 92

class LightPlatform :
  public IOPlatform,
  public ILightPlatform,
  public EffectSwitcher {
public:
  void setup();
  void loop();
  void setCoordinator(LEDCoordinator *coordinator);

  RGB getColor();
  void setColor(RGB color);

  void setBrightness(uint8_t brightness);
  uint8_t getBrightness();

  void setPower(bool enabled);
  bool getPower();

  void onEffectSwitch();
  bool setEffect(uint8_t effectCode);
  uint8_t getEffect();

private:
  LEDCoordinator* coordinator_ = nullptr;
  ILightEffect* nextEffect_ = nullptr;
  LightState state_;
  SmoothBrightness brightnessHandler_;
  PixelHandler pixelHandler_;
  Pixels pixels_;
  PioWS2812 ws2812_ = PioWS2812(PIN_LED_STRIP, pio1, 1);
  WS2812Renderer renderer_ = WS2812Renderer(&ws2812_);
};
