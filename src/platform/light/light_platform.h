#pragma once

#include <MyrtIO.h>
#include <NeoPixelCoordinator.h>
#include <config.h>
#include "interfaces/platform.h"
#include "brightness_handler.h"
#include "pixel_handler.h"

#define STRIP_LENGTH 92

class LightPlatform :
  public IOUnit,
  public ILightPlatform,
  public EffectSwitcher {
public:
  void setup();
  void loop();

  const char* name();

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
  LEDCoordinator coordinator_;
  ILightEffect* nextEffect_ = nullptr;
  LightState state_;
  SmoothBrightness brightnessHandler_;
  PixelHandler pixelHandler_;
  Pixels pixels_;
  PioWS2812 ws2812_ = PioWS2812(
    CONFIG_PIN_LED_CTL,
    CONFIG_LIGHT_WORKER_PIO,
    CONFIG_LIGHT_WORKER_SM
  );
  WS2812Renderer renderer_ = WS2812Renderer(&ws2812_);
};
