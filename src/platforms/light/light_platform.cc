#include <MyrtIO/logging/logger.h>
#include <LightComposer/color/blend.h>
#include "light_platform.h"
#include "effects/effects.h"

const uint8_t kFramesPerSecond = 120;

const char* kIOLightPlatformName = "Light";

io::Logger lightLog(kIOLightPlatformName);

const char* LightPlatform::getName() {
	return kIOLightPlatformName;
}

// Implementation of the setup function to initialize the LED platform.
void LightPlatform::setup() {
	lightLog.print("starting HAL...");
	hal_.setup(params_.colorCorrection);

	effects_
		.insert(&StaticFx)
		.insert(&RainbowFx)
		.insert(&LoadingFx)
		.insert(&FillFx);

	lightLog.print("setup LEDs");
	pixels_.setColor(params_.colorInitial);
	pixels_.setTransition(params_.transitionMs);
	pixels_.setEffect(&StaticFx);
	brightness_.setBrightness(255);
	brightness_.setPower(true);

	lightLog.print("starting composer...");
	composer_.start(hal_, brightness_, pixels_);
}

// Implementation of the function called at the start of each loop iteration.
void LightPlatform::loop() {
	composer_.loop();
}

RGBColor LightPlatform::getColor() {
	return pixels_.getColor();
}

uint8_t LightPlatform::getBrightness() {
	return brightness_.getBrightness();
}

// Implementation of the public method to set the overall brightness of the
// LEDs.
void LightPlatform::setBrightness(uint8_t brightness) {
	lightLog.print("update brightness");
	brightness_.setBrightness(brightness);
}

// Implementation of the public method to set power status of the LEDs.
void LightPlatform::setPower(bool enabled) {
	lightLog.print("update power");
	brightness_.setPower(enabled);
}

bool LightPlatform::getPower() {
	return brightness_.getPower();
}

void LightPlatform::setColor(RGBColor color) {
	lightLog.print("update color");
	pixels_.setColor(color);
}

void LightPlatform::setColorTemperature(mireds_t temperature) {
	// TODO: move to separate class
	lightLog.print("update color temperature");
	mode_ = LightMode::WhiteMode;
	temperature_ = temperature;
	uint8_t ratio = map(temperature,
		params_.colorColdWhiteMireds, params_.colorWarmWhiteMireds,
		0, 255
	);
	ratio = 255 - ratio;
	pixels_.setColor(
		blendColors(params_.colorWarmWhite, params_.colorColdWhite, ratio)
	);
}

mireds_t LightPlatform::getTemperature() {
	return temperature_;
}

bool LightPlatform::setEffect(const char* effectName, bool force) {
	if (effectName == nullptr) {
		return false;
	}

	auto effect = effects_.find(effectName);
	if (effect == nullptr || pixels_.getEffect() == effect) {
		return false;
	}
	pixels_.setEffect(effect, force);
	return true;
}

const char* LightPlatform::getEffect() {
	auto current = pixels_.getEffect();
	if (current == nullptr) {
		return nullptr;
	}

	return current->getName();
}

const char** LightPlatform::effects() {
	return effects_.names();
}

uint8_t LightPlatform::effectCount() {
	return effects_.size();
}

color_mode_t LightPlatform::getMode() {
	return mode_;
}

void LightPlatform::setFPS(uint8_t fps) {
	composer_.setFPS(fps);
}
