#include "pixel_handler.h"

// Implementation of the setup function to initialize the LED matrix and set the
// rendering effect.
void PixelHandler::setup(ILightEffect* effect, LightState* state, Pixels* pixels) {
	state_ = state;
	pixels_ = pixels;
	effect->onColorUpdate(state_);
	setEffect(effect);
}

// Implementation of the public method to handle a frame of animation and
// rendering.
bool PixelHandler::handleFrame() {
	return effect_->handleFrame(state_, pixels_);
}

// Implementation of the public method to set the color for the LED matrix.
void PixelHandler::handleStateUpdate() {
	effect_->onColorUpdate(state_);
}

// Implementation of the public method to set the rendering effect for the LED
// matrix.
void PixelHandler::setEffect(ILightEffect* effect) {
	effect_ = effect;
}
