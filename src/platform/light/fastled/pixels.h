#pragma once

#include <FastLED.h>

#define PIXELS_MAX_COUNT 128

class Pixels {
  public:
	void setup(uint8_t count);
	void mirror();
	void set(CRGB color);
	void set(CRGB color, uint8_t idx);
	void set(CRGB color, uint8_t from, uint8_t to);
	CRGB* colors();
	uint8_t length();

  protected:
	CRGB colors_[PIXELS_MAX_COUNT];
	uint8_t count_;
};
