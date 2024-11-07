#pragma once

#include <LightComposer/LightComposer.h>
#include <Attotime.h>

typedef LightState<void> FillEffectState;

class FillEffect : public IPixelsEffect<void> {
  public:
	bool handleFrame(FillEffectState& state, IPixels& pixels);
	void onColorUpdate(FillEffectState& state);
	void onActivate(FillEffectState& state, IPixels& pixels);

  private:
	Progress progress_ = Progress();
};

extern FillEffect FillFx;
