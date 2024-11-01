#pragma once

#include "platforms/light/light_effect.h"
#include <Attotime.h>
#include <MyrtIO.h>

class LoadingEffect : public ILightEffect {
  public:
	bool handleFrame(LightState* state, Pixels* pixels);
	void onColorUpdate(LightState* state);
	void onActivate(LightState* state, Pixels* pixels);

  private:
	bool isReverse_ = true;
	uint8_t fillSize_ = 0;
	uint8_t maxOffset = 0;
	Progress progress_ = Progress();

	void fill_(Pixels* pixels, LightState* state, uint8_t start, uint8_t length);
};

extern LoadingEffect LoadingFx;
