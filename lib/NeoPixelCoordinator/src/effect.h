#pragma once

#include "pixels.h"

template <class State>
class ILEDEffect {
public:
  // Handle a frame of animation. Should return true if the frame should be rendered.
  virtual bool handleFrame(State* state, Pixels* pixels) = 0;
  // Lifecycle hooks, called when the state target color changes
  virtual void onColorUpdate(State* state);
  // Lifecycle hooks, called when the effect changes.
  // Useful for static effects that need to render even when the color is not changing
  virtual void onEffectUpdate(State* state);
};
