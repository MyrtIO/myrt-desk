#include "light_platform.h"
#include "effects/effects.h"
#include "interfaces/effects.h"

const uint8_t kFramesPerSecond = 50;

const uint8_t asd = 6;

// Implementation of the setup function to initialize the LED platform.
void LightPlatform::setup() {
  state_.colorTransitionMs = 600;
  state_.effectTransitionMs = 1000;
  state_.currentColor = RGB::Black;
  state_.selectedColor = RGB::Black;
  state_.targetColor = RGB::Blue;
  state_.currentBrightness = 255;
  state_.targetBrightness = 255;
  state_.enabled = true;

  Serial.println("Setup pixels");

  pixels_.setup(&ws2812_, STRIP_LENGTH);

  Serial.println("Setup pixel handler");
  pixelHandler_.setup(&StaticFx, &state_, &pixels_);

  Serial.println("Setup brightness handler");
  brightnessHandler_.setup(&ws2812_, &state_, this);
  brightnessHandler_.handleBrightnessUpdate();

  coordinator_->addHandlers(&pixelHandler_, &brightnessHandler_);
  coordinator_->start(&renderer_, kFramesPerSecond);
}

void LightPlatform::setCoordinator(LEDCoordinator* coordinator) {
  coordinator_ = coordinator;
}

// Implementation of the function called at the start of each loop iteration.
void LightPlatform::loop() {
  // Serial.println("Light loop");
  coordinator_->handle();
}

RGB LightPlatform::getColor() {
  return state_.targetColor;
}

uint8_t LightPlatform::getBrightness() {
  return state_.targetBrightness;
}

// Implementation of the public method to set the overall brightness of the LEDs.
void LightPlatform::setBrightness(uint8_t brightness) {
  state_.targetBrightness = brightness;
  brightnessHandler_.handleBrightnessUpdate();
}

// Implementation of the public method to set power status of the LEDs.
void LightPlatform::setPower(bool enabled) {
  state_.enabled = enabled;
  brightnessHandler_.handlePowerUpdate();
}

bool LightPlatform::getPower() {
  return state_.enabled;
}

void LightPlatform::setColor(RGB color) {
  state_.targetColor = color;
  pixelHandler_.handleStateUpdate();
}

bool LightPlatform::setEffect(uint8_t effectCode) {
  switch (effectCode) {
  case LightEffect::Static:
    nextEffect_ = &StaticFx;
    break;
  case LightEffect::Rainbow:
    nextEffect_ = &RainbowFx;
    break;
  default:
    return false;
    break;
  }
  brightnessHandler_.handleEffectUpdate();
  return true;
}

void LightPlatform::onEffectSwitch() {
  if (nextEffect_ != nullptr) {
    pixelHandler_.setEffect(nextEffect_);
    nextEffect_->onEffectUpdate(&state_);
    nextEffect_ = nullptr;
  }
}

uint8_t LightPlatform::getEffect() {
  ILightEffect *current = pixelHandler_.getEffect();
  if (current == &StaticFx) {
    return LightEffect::Static;
  } else if (current == &RainbowFx) {
    return LightEffect::Rainbow;
  }
  return 0;
}
