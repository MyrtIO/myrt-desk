#pragma once

#include <stdint.h>
#include "rgb_color.h"

typedef uint8_t effect_t;

class IOLight {
  public:
	virtual RGBColor getColor() = 0;
	virtual void setColor(RGBColor color) = 0;
	virtual void setColorTemperature(uint16_t mireds) = 0;
	virtual void setPower(bool enabled) = 0;
	virtual bool getPower() = 0;
	virtual void setBrightness(uint8_t brightness) = 0;
	virtual uint8_t getBrightness() = 0;
	virtual void setFPS(uint8_t fps) = 0;
	virtual bool setEffect(effect_t code, bool force = false) = 0;
	virtual effect_t getEffect() = 0;
	virtual uint8_t getMode() = 0;
	virtual uint16_t getTemperature() = 0;
};
