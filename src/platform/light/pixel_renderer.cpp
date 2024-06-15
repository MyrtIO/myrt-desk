#include "pixel_renderer.h"
#include "pixeltypes.h"

// Implementation of the setup function to initialize the LED matrix and set the rendering effect.
void PixelRenderer::setup(LEDEffect *effect, EffectState *state) {
  state_ = state;
  effect->onColorUpdate(state_);
  setEffect(effect);
}

// Implementation of the public method to handle a frame of animation and rendering.
bool PixelRenderer::handleFrame() {
  return effect_->handleFrame(state_, &strip_);
}

// Implementation of the public method to set the color for the LED matrix.
void PixelRenderer::handleStateUpdate() {
  effect_->onColorUpdate(state_);
}

// Implementation of the public method to set the rendering effect for the LED matrix.
void PixelRenderer::setEffect(LEDEffect *effect) {
  // TODO: add brightness up down animation
  effect_ = effect;
}
