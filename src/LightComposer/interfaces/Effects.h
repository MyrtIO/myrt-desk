#pragma once

#include <stdint.h>
#include "../RGBColor.h"
#include "LightHAL.h"

class IEffectProvider {
public:
	virtual void requestEffectChange();
};

class IEffectChanger {
public:
	virtual void onEffectChange();
};

typedef uint8_t fract_t;

template <class Locator = void>
struct LightState {
	RGBColor previousColor = 0;
	RGBColor currentColor = 0;
	RGBColor targetColor = 0;
	Locator* locator;
	uint16_t transitionMs = 0;
};

class IPixels {
public:
	virtual void setup(ILightHAL* hal) = 0;
	virtual void set(RGBColor color) = 0;
	virtual void set(RGBColor color, uint8_t idx) = 0;
	virtual void set(RGBColor color, uint8_t from, uint8_t to) = 0;
	virtual void mirror() = 0;
	virtual RGBColor* raw();
	virtual uint16_t count();
	virtual uint16_t center();
};

template <class Locator>
class IPixelsEffect {
public:
	// Handle a frame of animation.
	// Should return true if rendering is needed.
	virtual bool handleFrame(LightState<Locator>& state, IPixels& pixels) = 0;

	// Lifecycle hooks, called when the state target color changes
	virtual void onColorUpdate(LightState<Locator>& state) {}

	// Lifecycle hooks, called when the effect is activated
	virtual void onActivate(LightState<Locator>& state, IPixels& pixels) {}
};
