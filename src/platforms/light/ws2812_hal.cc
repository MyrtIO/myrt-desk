#include "ws2812_hal.h"
#include <FastLED.h>


void WS2812HAL::setup(RGBColor correction = RGBColor::White) {
	FastLED.addLeds<WS2812B, CONFIG_LIGHT_PIN_CTL, GRB>(
		reinterpret_cast<CRGB*>(&leds_[0]),
		CONFIG_LIGHT_LED_COUNT
	);
	FastLED.setCorrection(CRGB(
		correction.r,
		correction.g,
		correction.b
	));
	FastLED.show();
}

uint16_t WS2812HAL::count() {
	return CONFIG_LIGHT_LED_COUNT;
}

void WS2812HAL::setPixel(const uint16_t index, const RGBColor& color) {
	leds_[index] = color;
}

RGBColor WS2812HAL::getPixel(const uint16_t index) {
	return leds_[index];
}

void WS2812HAL::setBrightness(const uint8_t brightness){
	FastLED.setBrightness(brightness);
}

void WS2812HAL::apply() {
	FastLED.show();
}
