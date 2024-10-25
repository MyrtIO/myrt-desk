#pragma once

#include <MyrtIO.h>
#include <FastLED.h>

#include "rgb_color.h"
#include "light_interface.h"
#include "brightness_handler.h"
#include "fastled/renderer.h"
#include "light_effect.h"
#include "pixel_handler.h"

struct LightPlatformParams {
	uint8_t ledCount;
	uint32_t colorCorrection;
	uint32_t colorInitial;
	uint32_t colorWarmWhite;
	uint16_t colorWarmWhiteMireds;
	uint32_t colorColdWhite;
	uint16_t colorColdWhiteMireds;
	uint16_t transitionMs;
};

class LightPlatform : public io::Unit, public IOLight, public EffectSwitcher {
  public:
	LightPlatform(const LightPlatformParams& params) : params_(params) {};
	void setup();
	void loop();

	const char* name();

	RGBColor getColor();
	void setColor(RGBColor color);
	void setColorTemperature(uint16_t mireds);

	void setBrightness(uint8_t brightness);
	uint8_t getBrightness();

	void setPower(bool enabled);
	bool getPower();

	void onEffectSwitch();
	bool setEffect(uint8_t effectCode, bool force = false);
	effect_t getEffect();
	uint8_t getMode();
	uint16_t getTemperature();

	void setFPS(uint8_t fps);

  private:
	LEDCoordinator coordinator_;
	ILightEffect* nextEffect_ = nullptr;
	LightState state_;
	SmoothBrightness brightnessHandler_;
	PixelHandler pixelHandler_;
	Pixels pixels_;
	FastLEDRenderer renderer_;
	LightPlatformParams params_;
};
