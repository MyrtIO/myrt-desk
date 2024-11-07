#include "Pixels.h"

void Pixels::mirror() {
	for (uint8_t i = 0; i < center_; i++) {
		hal_->setPixel(count_ - 1 - i, hal_->getPixel(i));
	}
}

void Pixels::setup(ILightHAL* hal) {
	hal_ = hal;
	count_ = hal_->count();
	center_ = (count_ / 2);
	if (count_ % 2 != 0) {
        center_ += 1;
    }
	this->set(RGBColor::Black);
	hal_->apply();
}

void Pixels::set(RGBColor color, uint8_t idx) {
	hal_->setPixel(idx, color);
}

void Pixels::set(RGBColor color, uint8_t from, uint8_t to) {
	for (uint8_t i = from; i < to; i++) {
		this->set(color, i);
	}
}

void Pixels::set(RGBColor color) {
	this->set(color, 0, count_);
}

uint16_t Pixels::count() {
	return count_;
}

uint16_t Pixels::center() {
    return center_;
}

RGBColor* Pixels::raw() {
	return hal_->pixels();
}
