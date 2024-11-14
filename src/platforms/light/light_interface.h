#pragma once

#include <stdint.h>
#include <LightComposer/color/rgb_color.h>

typedef uint8_t effect_t;
typedef uint8_t color_mode_t;
typedef uint16_t mireds_t;

enum LightEffect : effect_t {
	Static = 0,
	Rainbow = 1,
	Loading = 2,
	Fill = 3,
};

enum LightMode : color_mode_t {
	RGBMode = 0,
	WhiteMode = 1,
};

class ILightPlatform {
  public:
	virtual RGBColor getColor() = 0;
	virtual void setColor(RGBColor color) = 0;

	virtual mireds_t getTemperature() = 0;
	virtual void setColorTemperature(mireds_t mireds) = 0;

	virtual bool getPower() = 0;
	virtual void setPower(bool enabled) = 0;

	virtual uint8_t getBrightness() = 0;
	virtual void setBrightness(uint8_t brightness) = 0;

	virtual void setFPS(uint8_t fps) = 0;

	virtual effect_t getEffect() = 0;
	virtual bool setEffect(effect_t code, bool force = false) = 0;

	virtual color_mode_t getMode() = 0;
};
