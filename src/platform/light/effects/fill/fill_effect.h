#pragma once

#include "platform/light/light_effect.h"
#include <Attotime.h>
#include <MyrtIO.h>

class FillEffect : public ILightEffect {
  public:
	bool handleFrame(LightState* state, Pixels* pixels);
	void onColorUpdate(LightState* state);
	void onActivate(LightState* state, Pixels* pixels);

  private:
	Progress progress_ = Progress();
	uint8_t center_ = 0;
};

extern FillEffect FillFx;
