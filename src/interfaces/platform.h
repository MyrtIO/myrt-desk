#pragma once

#include <stdint.h>
#include "NeoPixelCoordinator.h"

typedef uint8_t effect_t;

class ILightPlatform {
  public:
    virtual RGB getColor() = 0;
    virtual void setColor(RGB color) = 0;
    virtual void setPower(bool enabled) = 0;
    virtual bool getPower() = 0;
    virtual void setBrightness(uint8_t brightness) = 0;
    virtual uint8_t getBrightness() = 0;
    virtual bool setEffect(effect_t code) = 0;
    virtual effect_t getEffect() = 0;
};

class IHeightPlatform {
  public:
    virtual uint16_t getHeight() = 0;
    virtual bool setHeight(uint16_t height) = 0;
};
