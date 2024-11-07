#pragma once

#include <Attotime.h>
#include "interfaces/Effects.h"
#include "interfaces/Renderers.h"

#define LIGHT_COMPOSER_FPS 60

template <class Locator>
class LightComposer: public IEffectChanger, IEffectProvider {
public:
	// Initialization composer
	void start(
		ILightHAL& hal,
		IBrightnessRenderer& brightness,
		IPixelsRenderer<Locator>& pixels,
		uint8_t fps = LIGHT_COMPOSER_FPS
	) {
		brightness_ = &brightness;
		pixels_ = &pixels;
		hal_ = &hal;
		brightness_->attachComposer(this, &hal);
		pixels_->attachComposer(this, &hal);
		setFPS(fps);
		requestFrame_();
	}

	// Main handling method for updating LEDs
	void loop() {
		if (!frameDeadline_.finished()) {
			return;
		}
		bool hasChanges = false;
		hasChanges |= brightness_->renderFrame();
		hasChanges |= pixels_->renderFrame();
		if (hasChanges) {
			hal_->apply();
		}
		requestFrame_();
	}

	void setFPS(uint8_t fps) {
		frameTime_ = 1000 / fps;
	}

	void onEffectChange() {
		pixels_->onEffectChange();
	}
	void requestEffectChange() {
		brightness_->requestEffectChange();
	}

private:
	ILightHAL* hal_;
	IBrightnessRenderer* brightness_;
	IPixelsRenderer<Locator>* pixels_;

	Timer frameDeadline_;
	uint16_t frameTime_;

	// Request the next frame
	void requestFrame_() {
		frameDeadline_.start(frameTime_);
	}
};
