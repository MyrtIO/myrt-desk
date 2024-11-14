#pragma once

#include <MyrtIO/device/unit.h>
#include <LightComposer.h>
#include <LightComposer/brightness/brightness_renderer.h>
#include <LightComposer/pixels/pixels_renderer.h>
#include <config.h>
#include "light_interface.h"
#include "ws2812_hal.h"

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

class LightPlatform : public io::Unit, public ILightPlatform {
  public:
	LightPlatform(const LightPlatformParams& params) : params_(params) {};

	const char* getName() override;
	void setup() override;
	void loop() override;

	void setColor(RGBColor color);
	RGBColor getColor();
	void setColorTemperature(mireds_t temperature);
	mireds_t getTemperature();

	void setBrightness(uint8_t brightness);
	uint8_t getBrightness();

	void setPower(bool enabled);
	bool getPower();

	bool setEffect(uint8_t effectCode, bool force = false);
	effect_t getEffect();
	color_mode_t getMode();

	void setFPS(uint8_t fps);

  private:
	WS2812HAL hal_;
	BrightnessRenderer brightness_;
	PixelsRenderer<void> pixels_;
	LightComposer<void> composer_;
	LightPlatformParams params_;
	LightMode mode_ = LightMode::RGBMode;
	mireds_t temperature_ = 0;
};
