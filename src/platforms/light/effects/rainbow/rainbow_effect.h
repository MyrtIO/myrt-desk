#pragma once

#include <LightComposer/pixels/i_pixels_effect.h>
#include <LightComposer/color/hsv_color.h>
#include <Attotime.h>

typedef LightState<void> RainbowEffectState;

class RainbowEffect : public IPixelsEffect<void> {
public:
	const char* getName();
	bool handleFrame(RainbowEffectState& state, IPixels& pixels);
	void onActivate(RainbowEffectState& state, IPixels& pixels);

private:
	Progress progress_ = Progress();
	uint8_t hueProgress_ = 0;
	HSVColor firstColor_ = HSVColor(0, 255, 255);
	HSVColor secondColor_ = HSVColor(0, 255, 255);
	HSVColor thirdColor_ = HSVColor(0, 255, 255);

	uint8_t hueShift_(uint8_t shift);
};

extern RainbowEffect RainbowFx;
