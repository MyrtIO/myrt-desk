#include "rainbow_effect.h"
#include <FastLED.h>

const size_t kRainbowTransitionDuration = 12000;

bool RainbowEffect::handleFrame(EffectState *state, LEDStrip *strip) {
  hueProgress_ = progress_.get();
  firstColor_.hue = hueShift_(0);
  secondColor_.hue = hueShift_(60);
  thirdColor_.hue = hueShift_(120);

  fill_gradient(
    strip->leds,
    strip->length / 2,
    firstColor_,
    secondColor_,
    thirdColor_,
    FORWARD_HUES
  );
  strip->mirror();
  if (progress_.finished()) {
    restart_();
  }
  return true;
}

void RainbowEffect::onColorUpdate(EffectState *state) {
  restart_();
}

void RainbowEffect::onEffectUpdate(EffectState *state) {}

uint8_t RainbowEffect::hueShift_(uint8_t shift) {
  return (hueProgress_ + shift) % 255;
}

void RainbowEffect::restart_() {
  progress_.start(kRainbowTransitionDuration);
}

RainbowEffect RainbowFx;
