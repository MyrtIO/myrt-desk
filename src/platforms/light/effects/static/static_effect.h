#pragma once

#include <LightComposer/pixels/i_pixels_effect.h>
#include <Attotime.h>

typedef LightState<void> StaticEffectState;

class StaticEffect : public IPixelsEffect<void> {
public:
	const char* getName();
	bool handleFrame(StaticEffectState& state, IPixels& pixels);
	void onColorUpdate(StaticEffectState& state);
	void onActivate(StaticEffectState& state);

private:
	Progress progress_ = Progress();
	bool forceUpdate_ = false;
};

extern StaticEffect StaticFx;
