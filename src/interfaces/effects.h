#pragma once

#include <stdint.h>

enum LEDEffect : uint8_t {
  EffectStatic = 0,
  EffectSmooth = 1,
  EffectZoom = 2,
};
