#include "light_platform.h"
#include "effects/effects.h"
#include "interfaces/effects.h"

// Implementation of the setup function to initialize the LED platform.
void LightPlatform::setup() {
  state_.transitionTime = 800;
  state_.currentBrightness = 255;
  state_.targetBrightness = 255;
  state_.enabled = true;
  pixels_.setup(&StaticFx, &state_);
  brightness_.setup(&state_);
  leds_.init()
    ->pixels(&pixels_)
    ->brightness(&brightness_);

  brightness_.handleBrightnessUpdate();
}

// Implementation of the function called at the start of each loop iteration.
void LightPlatform::onLoop() {
  leds_.handle();
}

CRGB LightPlatform::getColor() {
  return state_.currentColor;
}

uint8_t LightPlatform::getBrightness() {
  return state_.currentBrightness;
}

// Implementation of the public method to set the overall brightness of the LEDs.
void LightPlatform::setBrightness(uint8_t brightness) {
  state_.targetBrightness = brightness;
  brightness_.handleBrightnessUpdate();
}

// Implementation of the public method to set power status of the LEDs.
void LightPlatform::setPower(bool enabled) {
  state_.enabled = enabled;
  brightness_.handlePowerUpdate();
}

bool LightPlatform::getPower() {
  return state_.enabled;
}

void LightPlatform::setColor(CRGB color) {
  state_.targetColor = color;
  pixels_.handleStateUpdate();
}

bool LightPlatform::setEffect(uint8_t effectCode) {
  switch (effectCode) {
  case LightEffect::Static:
    pixels_.setEffect(&StaticFx);
    break;
  case LightEffect::Rainbow:
    pixels_.setEffect(&RainbowFx);
  default:
    return false;
    break;
  }
  return true;
}

uint8_t LightPlatform::getEffect() {
  LEDEffect *current = pixels_.getEffect();
  if (current == &StaticFx) {
    return LightEffect::Static;
  } else if (current == &RainbowFx) {
    return LightEffect::Rainbow;
  }
  return 0;
}
