#include "rainbow_effect.h"
#include <LightComposer/color/rainbow.h>

const size_t kRainbowTransitionDuration = 12000;

bool RainbowEffect::handleFrame(RainbowEffectState& state, IPixels& pixels) {
	hueProgress_ = progress_.get();

	firstColor_.hue = hueShift_(0);
	secondColor_.hue = hueShift_(60);
	thirdColor_.hue = hueShift_(120);

	fillGradient(
		pixels.raw(),
		pixels.center(),
		firstColor_, secondColor_, thirdColor_,
	    FORWARD_HUES
	);
	pixels.mirror();

	if (progress_.finished()) {
		progress_.start(kRainbowTransitionDuration);
	}

	return true;
}

uint8_t RainbowEffect::hueShift_(uint8_t shift) {
	return (hueProgress_ + shift) % 255;
}

void RainbowEffect::onActivate(RainbowEffectState& state, IPixels& pixels) {
	progress_.start(kRainbowTransitionDuration);
}

RainbowEffect RainbowFx;
