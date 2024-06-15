#pragma once

#include "Timing.h"
#include "interfaces.h"
#include "color_utils.h"
#include "effect.h"
#include "ws2812.h"

class LEDBee {
public:
  // Constructor and initialization method
  LEDBee* init();

  // Set the brightness renderer
  LEDBee* brightness(IRenderer* render);

  // Set the pixels renderer
  LEDBee* pixels(IRenderer* render);

  // Main handling method for updating LEDs
  void handle();

  // Get the frames per second (FPS)
  uint8_t fps();

  // // Get current LEDs state
  // EffectState *state();

private:
  Timer frame_;
  IRenderer* brightness_;
  IRenderer* pixels_;

  // Request the next frame
  void requestFrame_();

  // Check if it's time to render the frame
  bool shouldRenderFrame_();
};
