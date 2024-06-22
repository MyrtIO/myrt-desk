#include "rainbow_effect.h"
#include <FastLED.h>

const size_t kRainbowTransitionDuration = 12000;

bool RainbowEffect::handleFrame(LightState *state, Pixels *pixels) {
  hueProgress_ = progress_.get();
  firstColor_.hue = hueShift_(0);
  secondColor_.hue = hueShift_(60);
  thirdColor_.hue = hueShift_(120);

  fill_gradient(
    pixels->colors(),
    pixels->length() / 2,
    firstColor_,
    secondColor_,
    thirdColor_,
    FORWARD_HUES
  );
  pixels->mirror();
  if (progress_.finished()) {
    restart_();
  }
  return true;
}

uint8_t RainbowEffect::hueShift_(uint8_t shift) {
  return (hueProgress_ + shift) % 255;
}

void RainbowEffect::restart_() {
  progress_.start(kRainbowTransitionDuration);
}

RainbowEffect RainbowFx;
