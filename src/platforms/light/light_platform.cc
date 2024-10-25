#include "light_platform.h"
#include "effects/effects.h"
#include "fastled/crgb_color.h"

const uint8_t kFramesPerSecond = 120;

const char* kIOLightPlatformName = "Light";

IOLogger lightLog(kIOLightPlatformName);

const char* LightPlatform::name() {
	return kIOLightPlatformName;
}

// Implementation of the setup function to initialize the LED platform.
void LightPlatform::setup() {
	state_.colorTransitionMs = params_.transitionMs;
	state_.effectTransitionMs = 1000;
	state_.previousColor = CRGB::Black;
	state_.currentColor = CRGB::Black;
	state_.selectedColor = CRGB::Black;
	state_.targetColor = params_.colorInitial;
	state_.currentBrightness = 255;
	state_.targetBrightness = 255;
	state_.enabled = true;

	lightLog.print("setup ws2812");
	pixels_.setup(params_.ledCount);
	lightLog.print("setup pixel handler");
	auto initialEffect = &StaticFx;
	pixelHandler_.setup(initialEffect, &state_, &pixels_);
	lightLog.print("setup brightness handler");
	brightnessHandler_.setup(&state_, this);
	brightnessHandler_.handleBrightnessUpdate();
	coordinator_.addHandlers(&pixelHandler_, &brightnessHandler_);
	coordinator_.start(&renderer_, kFramesPerSecond);
	initialEffect->onActivate(&state_, &pixels_);
	pixelHandler_.handleStateUpdate();
}

// Implementation of the function called at the start of each loop iteration.
void LightPlatform::loop() {
	coordinator_.handle();
}

RGBColor LightPlatform::getColor() {
	return rgbFromFastLED(state_.currentColor);
}

uint8_t LightPlatform::getBrightness() {
	return state_.targetBrightness;
}

// Implementation of the public method to set the overall brightness of the
// LEDs.
void LightPlatform::setBrightness(uint8_t brightness) {
	lightLog.print("update brightness");
	state_.targetBrightness = brightness;
	brightnessHandler_.handleBrightnessUpdate();
}

// Implementation of the public method to set power status of the LEDs.
void LightPlatform::setPower(bool enabled) {
	lightLog.print("update power");
	state_.enabled = enabled;
	brightnessHandler_.handlePowerUpdate();
}

bool LightPlatform::getPower() {
	return state_.enabled;
}

void LightPlatform::setColor(RGBColor color) {
	lightLog.print("update color");
	state_.previousColor = state_.currentColor;
	state_.targetColor = rgbToFastLED(color);
	state_.mode = LightMode::RGBMode;
	pixelHandler_.handleStateUpdate();
}

void LightPlatform::setColorTemperature(uint16_t mireds) {
	lightLog.print("update color temperature");
	state_.mode = LightMode::WhiteMode;
	state_.temperature = mireds;
	uint8_t ratio = map(mireds,
		params_.colorColdWhiteMireds, params_.colorWarmWhiteMireds,
		0, 255
	);
	ratio = 255 - ratio;
	state_.targetColor = blend(params_.colorWarmWhite, params_.colorColdWhite, ratio);
	pixelHandler_.handleStateUpdate();
}

uint16_t LightPlatform::getTemperature() {
	return state_.temperature;
}

bool LightPlatform::setEffect(uint8_t effectCode, bool force) {
	switch (effectCode) {
	case LightEffect::Static:
		nextEffect_ = &StaticFx;
		break;
	case LightEffect::Rainbow:
		nextEffect_ = &RainbowFx;
		break;
	case LightEffect::Loading:
		nextEffect_ = &LoadingFx;
		break;
	case LightEffect::Fill:
		nextEffect_ = &FillFx;
		break;
	default:
		return false;
		break;
	}
	nextEffect_->onActivate(&state_, &pixels_);
	if (force) {
		onEffectSwitch();
	} else {
		brightnessHandler_.handleEffectUpdate();
	}
	return true;
}

void LightPlatform::onEffectSwitch() {
	if (nextEffect_ != nullptr) {
		pixelHandler_.setEffect(nextEffect_);
		nextEffect_->onEffectUpdate(&state_);
		nextEffect_ = nullptr;
	}
}

effect_t LightPlatform::getEffect() {
	ILightEffect* current = pixelHandler_.getEffect();

	if (current == &StaticFx) {
		return LightEffect::Static;
	} else if (current == &RainbowFx) {
		return LightEffect::Rainbow;
	} else if (current == &LoadingFx) {
		return LightEffect::Loading;
	} else if (current == &FillFx) {
		return LightEffect::Fill;
	}

	return 0;
}

uint8_t LightPlatform::getMode() {
	return state_.mode;
}

void LightPlatform::setFPS(uint8_t fps) {
	coordinator_.setFPS(fps);
}
