#include "renderer.h"
#include <FastLED.h>

void _FastLEDRenderer::apply() {
  FastLED.show();
}

_FastLEDRenderer FastLEDRenderer;
