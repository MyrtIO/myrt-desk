#pragma once

#include <stdint.h>
#include "../RGBColor.h"
#include "LightHAL.h"
#include "Effects.h"

class IRenderer {
public:
	// Handle frame and update LED state.
	// Should return true if the state needs to be updated, false otherwise.
	virtual bool renderFrame();
};

class IBrightnessRenderer: public IRenderer, public IEffectProvider {
public:
	virtual void attachComposer(IEffectChanger* composer, ILightHAL* hal) = 0;

	virtual void setBrightness(uint8_t brightness) = 0;
	virtual void setPower(bool enabled) = 0;
	virtual uint8_t getBrightness() = 0;
	virtual bool getPower() = 0;
};

template <class Locator>
class IPixelsRenderer: public IRenderer, public IEffectChanger {
public:
	virtual void attachComposer(IEffectProvider* composer, ILightHAL* hal) = 0;

	virtual void setColor(const RGBColor color) = 0;
	virtual void setEffect(IPixelsEffect<Locator>* effect, bool force = false) = 0;
	virtual void setLocator(Locator* locator) = 0;
	virtual RGBColor getColor();
	virtual IPixelsEffect<Locator>* getEffect();
};
