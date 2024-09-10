#pragma once

#include <NeoPixelCoordinator.h>
#include <interfaces/platform.h>

enum LightEffect : effect_t {
    Static = 0,
    Rainbow = 1,
    Loading = 2,
};

struct LightState {
    bool enabled = true;
    uint8_t currentBrightness = 255;
    uint8_t targetBrightness = 255;
    RGB selectedColor = RGB::Black;
    RGB currentColor = RGB::Black;
    RGB targetColor = RGB::Black;

    size_t colorTransitionMs = 0;
    size_t effectTransitionMs = 0;
};

class ILightEffect : public ILEDEffect<LightState> {
  public:
    virtual bool handleFrame(LightState* state, Pixels* pixels) = 0;

    // Lifecycle hooks, called when the state target color changes
    virtual void onColorUpdate(LightState* state){};

    // Lifecycle hooks, called when the effect changes.
    // Useful for static effects that need to render even when the color is not changing
    virtual void onEffectUpdate(LightState* state){};

    // Lifecycle hooks, called when the effect is activated
    virtual void onActivate(LightState* state, Pixels* pixels) {
    }
};
