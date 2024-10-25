#pragma once

#include "fastled/effect.h"
#include "light_interface.h"

enum LightEffect : effect_t {
	Static = 0,
	Rainbow = 1,
	Loading = 2,
	Fill = 3,
};

enum LightMode : uint8_t {
	RGBMode = 0,
	WhiteMode = 1,
};

struct LightState {
	bool enabled = true;
	uint8_t currentBrightness = 255;
	uint8_t targetBrightness = 255;
	CRGB previousColor = CRGB::Black;
	CRGB selectedColor = CRGB::Black;
	CRGB currentColor = CRGB::Black;
	CRGB targetColor = CRGB::Black;
	LightMode mode = LightMode::RGBMode;
	uint16_t temperature = 0;

	size_t colorTransitionMs = 0;
	size_t effectTransitionMs = 0;
};

class ILightEffect : public ILEDEffect<LightState> {
  public:
	virtual bool handleFrame(LightState* state, Pixels* pixels) = 0;

	// Lifecycle hooks, called when the state target color changes
	virtual void onColorUpdate(LightState* state) {};

	// Lifecycle hooks, called when the effect changes.
	// Useful for static effects that need to render even when the color is not changing
	virtual void onEffectUpdate(LightState* state) {};

	// Lifecycle hooks, called when the effect is activated
	virtual void onActivate(LightState* state, Pixels* pixels) {
	}
};
