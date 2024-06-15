#include "renderer.h"
#include "pixeltypes.h"

// Implementation of the setup function to initialize the LED matrix and set the rendering effect.
void LEDRenderer::setup(LEDEffect *effect) {
  // LEDS.addLeds<WS2812B, STRIP_PIN, GRB>(pixels_, STRIP_LENGTH);
  // LEDS.setCorrection(TypicalPixelString);
  state_.currentColor = CRGB::Black;
  state_.targetColor = CRGB::Blue;
  state_.enabled = true;
  effect->onColorUpdate(&state_);
  setEffect(effect);
}

// Implementation of the public method to handle a frame of animation and rendering.
bool LEDRenderer::handleFrame() {
  return effect_->handleFrame(&state_, &strip_);
  // return effect_(&, &state_);
}

// Implementation of the public method to set the rendering effect for the LED matrix.
void LEDRenderer::setEffect(LEDEffect *effect) {
  // TODO: add brightness up down animation
  effect_ = effect;
}

// Implementation of the public method to set the color for the LED matrix.
void LEDRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
  state_.targetColor.r = r;
  state_.targetColor.g = g;
  state_.targetColor.b = b;
  effect_->onColorUpdate(&state_);
}

// Implementation of the public method to set power status of the LEDs.
void LEDRenderer::setPower(bool enabled) {
  state_.enabled = enabled;
}

EffectState *LEDRenderer::state() {
  return &state_;
}
