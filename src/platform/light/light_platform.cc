#include "light_platform.h"
#include "effects/effects.h"

const uint8_t kFramesPerSecond = 100;

const char* kLightPlatformName = "Light";
const CRGB kMaxTemperatureColor = CRGB(237, 249, 255);
const CRGB kMinTemperatureColor = CRGB(255, 139, 20);

IOLogger lightLog(kLightPlatformName, &Serial);

const char* LightPlatform::name() {
    return kLightPlatformName;
}

// Implementation of the setup function to initialize the LED platform.
void LightPlatform::setup() {
    state_.colorTransitionMs = 600;
    state_.effectTransitionMs = 1000;
    state_.currentColor = CRGB::Black;
    state_.selectedColor = CRGB::Black;
    state_.targetColor = CRGB(255, 74, 51);
    state_.currentBrightness = 255;
    state_.targetBrightness = 255;
    state_.enabled = true;

    lightLog.print("setup ws2812");
    pixels_.setup(CONFIG_LIGHT_LED_COUNT);
    lightLog.print("setup pixel handler");
    pixelHandler_.setup(&StaticFx, &state_, &pixels_);
    lightLog.print("setup brightness handler");
    brightnessHandler_.setup(&state_, this);
    brightnessHandler_.handleBrightnessUpdate();
    coordinator_.addHandlers(&pixelHandler_, &brightnessHandler_);
    coordinator_.setFPS(15);
    coordinator_.start(&renderer_, kFramesPerSecond);
}

// Implementation of the function called at the start of each loop iteration.
void LightPlatform::loop() {
    coordinator_.handle();
}

CRGB LightPlatform::getColor() {
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

void LightPlatform::setColor(CRGB color) {
    lightLog.print("update color");
    state_.targetColor = color;
    state_.mode = LightMode::RGBMode;
    pixelHandler_.handleStateUpdate();
}

void LightPlatform::setColorTemperature(uint16_t mireds) {
    lightLog.print("update color temperature");
    state_.mode = LightMode::WhiteMode;
    state_.temperature = mireds;
    uint8_t ratio = 255 - map(
        mireds,
        CONFIG_LIGHT_MIREDS_MIN,
        CONFIG_LIGHT_MIREDS_MAX,
        0,
        255
    );
    state_.targetColor = blend(kMinTemperatureColor, kMaxTemperatureColor, ratio);
    pixelHandler_.handleStateUpdate();
}

uint16_t LightPlatform::getTemperature() {
    return state_.temperature;
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

effect_t LightPlatform::getEffect() {
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

uint8_t LightPlatform::getMode() {
    return state_.mode;
}
