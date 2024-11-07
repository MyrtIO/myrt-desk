#pragma once

#include <LightComposer/LightComposer.h>
#include <Attotime.h>

typedef LightState<void> LoadingEffectState;

class LoadingEffect : public IPixelsEffect<void> {
  public:
	bool handleFrame(LoadingEffectState& state, IPixels& pixels);
	void onColorUpdate(LoadingEffectState& state);
	void onActivate(LoadingEffectState& state, IPixels& pixels);

  private:
	Progress progress_ = Progress();
	bool isReverse_ = false;
	uint8_t fillSize_ = 0;
	size_t maxOffset_ = 0;
};

extern LoadingEffect LoadingFx;
