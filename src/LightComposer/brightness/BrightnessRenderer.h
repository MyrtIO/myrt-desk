#pragma once

#include <stdint.h>
#include <Attotime.h>
#include "../interfaces/Renderers.h"

enum class BrightnessChangeReason {
	Brightness,
	Power,
	Effect,
};

class BrightnessRenderer : public IBrightnessRenderer {
  public:
	void attachComposer(IEffectChanger* composer, ILightHAL* hal);
	bool renderFrame();
	void requestEffectChange();

	void setBrightness(uint8_t brightness);
	uint8_t getBrightness();
	void setPower(bool enabled);
	bool getPower();

  private:
	bool enabled_ = true;
	bool transitioning_ = false;
	bool effectSwitched_ = false;
	uint8_t previousValue_ = 0;
	uint8_t currentValue_ = 0;
	uint8_t targetValue_ = 0;
	uint8_t selectedValue_ = 0;
	Progress transition_;
	BrightnessChangeReason reason_;
	IEffectChanger* composer_ = nullptr;
	ILightHAL* hal_ = nullptr;
};
