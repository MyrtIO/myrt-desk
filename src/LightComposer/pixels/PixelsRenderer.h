#pragma once

#include "../interfaces/Renderers.h"
#include "../interfaces/Effects.h"
#include "Pixels.h"

template <class Locator>
class PixelsRenderer : public IPixelsRenderer<Locator> {
public:
	void attachComposer(IEffectProvider* composer, ILightHAL* hal) {
		composer_ = composer;
		pixels_.setup(hal);
	}
	bool renderFrame() {
		if (effect_ == nullptr) {
			return false;
		}
		return effect_->handleFrame(state_, pixels_);
	}
	void onEffectChange() {
		if (nextEffect_ == nullptr) {
			return;
		}
		effect_ = nextEffect_;
		effect_->onActivate(state_, pixels_);
		nextEffect_ = nullptr;
	}

	void setColor(const RGBColor color) {
		state_.previousColor = state_.currentColor;
		state_.targetColor = color;
		if (effect_ != nullptr) {
			effect_->onColorUpdate(state_);
		}
	}
	void setEffect(IPixelsEffect<Locator>* effect, bool force = false) {
		if (effect_ == nullptr || force) {
			effect_ = effect;
			effect_->onActivate(state_, pixels_);
			return;
		}
		nextEffect_ = effect;
		if (composer_ != nullptr) {
			composer_->requestEffectChange();
		}
	}
	void setTransition(uint16_t duration) {
		state_.transitionMs = duration;
	}
	void setLocator(Locator* locator) {
		state_.locator = locator;
	}
	RGBColor getColor() {
		return state_.targetColor;
	}
	IPixelsEffect<Locator>* getEffect() {
		return effect_;
	}

private:
	IPixelsEffect<Locator>* effect_;
	IPixelsEffect<Locator>* nextEffect_;
	IEffectProvider* composer_;
	LightState<Locator> state_;
	Pixels pixels_;
};
