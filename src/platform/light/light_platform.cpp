#include "light_platform.h"
#include "effects/effects.h"

const uint8_t kFramesPerSecond = 50;

const char* kLightPlatformName = "Light";

IOLogger lightLog(kLightPlatformName, &Serial);

const char* LightPlatform::name() {
    return kLightPlatformName;
}

// Implementation of the setup function to initialize the LED platform.
void LightPlatform::setup() {
    state_.colorTransitionMs  = 600;
    state_.effectTransitionMs = 1000;
    state_.currentColor       = RGB::Black;
    state_.selectedColor      = RGB::Black;
    state_.targetColor        = RGB(255, 74, 51);
    state_.currentBrightness  = 255;
    state_.targetBrightness   = 255;
    state_.enabled            = true;

    lightLog.print("setup ws2812");
    pixels_.setup(&ws2812_, CONFIG_LIGHT_LED_COUNT);
    lightLog.print("setup pixel handler");
    pixelHandler_.setup(&StaticFx, &state_, &pixels_);
    lightLog.print("setup brightness handler");
    brightnessHandler_.setup(&ws2812_, &state_, this);
    brightnessHandler_.handleBrightnessUpdate();
    coordinator_.addHandlers(&pixelHandler_, &brightnessHandler_);
    coordinator_.setFPS(15);
    coordinator_.start(&renderer_, kFramesPerSecond);
}

// Implementation of the function called at the start of each loop iteration.
void LightPlatform::loop() {
    coordinator_.handle();
}

RGB LightPlatform::getColor() {
    return state_.targetColor;
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

void LightPlatform::setColor(RGB color) {
    lightLog.print("update color");
    state_.targetColor = color;
    pixelHandler_.handleStateUpdate();
}

bool LightPlatform::setEffect(uint8_t effectCode) {
    switch (effectCode) {
    case LightEffect::Static: nextEffect_ = &StaticFx; break;
    case LightEffect::Rainbow: nextEffect_ = &RainbowFx; break;
    case LightEffect::Loading: nextEffect_ = &LoadingFx; break;
    default: return false; break;
    }
    nextEffect_->onActivate(&state_, &pixels_);
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
    ILightEffect* current = pixelHandler_.getEffect();
    if (current == &StaticFx) {
        return LightEffect::Static;
    } else if (current == &RainbowFx) {
        return LightEffect::Rainbow;
    } else if (current == &LoadingFx) {
        return LightEffect::Loading;
    }
    return 0;
}
