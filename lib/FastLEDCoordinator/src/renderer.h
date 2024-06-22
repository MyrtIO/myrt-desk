#pragma once

#include <LEDCoordinator.h>

class _FastLEDRenderer : public IRenderer {
public:
  void apply();
};

extern _FastLEDRenderer FastLEDRenderer;
