#pragma once

#include "../interfaces/LightHAL.h"
#include "../interfaces/Effects.h"
#include "../RGBColor.h"

class Pixels : public IPixels {
public:
	void setup(ILightHAL* hal);
	void set(RGBColor color);
	void set(RGBColor color, uint8_t idx);
	void set(RGBColor color, uint8_t from, uint8_t to);
	void mirror();
	RGBColor* raw();
	uint16_t count();
	uint16_t center();

private:
	ILightHAL* hal_;
	uint16_t count_ = 0;
	uint16_t center_ = 0;
};
