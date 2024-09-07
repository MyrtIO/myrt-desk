#pragma once

#include <NeoPixelCoordinator.h>

struct LightState {
  bool enabled;
  uint8_t currentBrightness;
  uint8_t targetBrightness;
  RGB selectedColor;
  RGB currentColor;
  RGB targetColor;

  size_t colorTransitionMs  = 0;
  size_t effectTransitionMs = 0;
};

class ILightEffect : public ILEDEffect<LightState> {
  public:
    virtual bool handleFrame(LightState* state, Pixels* pixels) = 0;
    // Lifecycle hooks, called when the state target color changes
    virtual void onColorUpdate(LightState* state) {};
    // Lifecycle hooks, called when the effect changes.
    // Useful for static effects that need to render even when the color is not changing
    virtual void onEffectUpdate(LightState* state) {};
};
