#include "light_platform.h"
#include "effects/static/static.h"
// #include "interfaces/effects.h"


// Implementation of the setup function to initialize the LED platform.
void LEDPlatform::setup() {
  pixels_.setup(&Static);
  leds_.init()
    ->pixels(&pixels_)
    ->brightness(&brightness_);
}

// Implementation of the function called at the start of each loop iteration.
void LEDPlatform::onLoop() {
  leds_.handle();
}

CRGB LEDPlatform::getColor() {
  return pixels_.state()->currentColor;
}

uint8_t LEDPlatform::brightness() {
  return brightness_.current();
}

// Implementation of the public method to set the overall brightness of the LEDs.
void LEDPlatform::setBrightness(uint8_t brightness) {
  brightness_.setBrightness(brightness);
}

// Implementation of the public method to set power status of the LEDs.
void LEDPlatform::setPower(bool enabled) {
  pixels_.setPower(enabled);
}

bool LEDPlatform::getPower() {
  return pixels_.state()->enabled;
}

void LEDPlatform::setColor(CRGB color) {
  Serial.println("LEDPlatform::setColor");
  pixels_.setColor(color.r, color.g, color.b);
}

void LEDPlatform::setEffect(uint8_t effectCode) {
  switch (effectCode) {
  case 0:
    pixels_.setEffect(&Static);
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

