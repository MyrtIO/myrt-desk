#include "light_platform.h"
#include "effects/static/static.h"
// #include "interfaces/effects.h"

// Implementation of the setup function to initialize the LED platform.
void LightPlatform::setup() {
  state_.transitionTime = 500;
  pixels_.setup(&StaticFx, &state_);
  brightness_.setup(&state_);
  leds_.init()
    ->pixels(&pixels_)
    ->brightness(&brightness_);
}

// Implementation of the function called at the start of each loop iteration.
void LightPlatform::onLoop() {
  leds_.handle();
}

CRGB LightPlatform::getColor() {
  return state_.currentColor;
}

uint8_t LightPlatform::brightness() {
  return state_.currentBrightness;
}

// Implementation of the public method to set the overall brightness of the LEDs.
void LightPlatform::setBrightness(uint8_t brightness) {
  state_.targetBrightness = brightness;
  brightness_.handleStateUpdate();
}

// Implementation of the public method to set power status of the LEDs.
void LightPlatform::setPower(bool enabled) {
  state_.enabled = enabled;
  pixels_.handleStateUpdate();
}

bool LightPlatform::getPower() {
  return state_.enabled;
}

void LightPlatform::setColor(CRGB color) {
  state_.targetColor = color;
  pixels_.handleStateUpdate();
}

void LightPlatform::setEffect(uint8_t effectCode) {
  switch (effectCode) {
  case 0:
    pixels_.setEffect(&StaticFx);
    break;
  // case LEDEffect::EffectSmooth:
  //   pixels_.setEffect(renderSmoothEffect);
  //   break;
  // case LEDEffect::EffectZoom:
  //   pixels_.setEffect(renderZoomEffect);
  //   break;
  default:
    break;
  }
}

