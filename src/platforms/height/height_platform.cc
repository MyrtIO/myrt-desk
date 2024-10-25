#include "height_platform.h"

const char* kHeightPlatformName = "Height";

IOLogger heightLog(kHeightPlatformName);

const char* HeightPlatform::name() {
	return kHeightPlatformName;
}

void HeightPlatform::setup() {
	pinMode(params_.bekantPinUp, OUTPUT);
	pinMode(params_.bekantPinDown, OUTPUT);
	heightLog.print("starting UART...");
	params_.linUART->setTX(params_.linPinTX);
	params_.linUART->setRX(params_.linPinRX);
	params_.linUART->begin(LIN_BAUD_RATE);
	heightLog.print("starting LIN reader...");
	reader_.begin(
		params_.linUART,
		params_.bekantHeightSlope,
		params_.bekantHeightBias
	);
	heightLog.print("starting state machine...");
}

void HeightPlatform::loop() {
	reader_.handle();
	switch (state_) {
	case DeskState::MoveUp:
		if (reader_.getHeight() >= targetHeight_) {
			stop_();
		}
		break;
	case DeskState::MoveDown:
		if (reader_.getHeight() <= targetHeight_) {
			stop_();
		}
		break;
	}
}

uint16_t HeightPlatform::get() {
	return reader_.getHeight();
}

bool HeightPlatform::set(uint16_t height) {
	heightLog.builder()
	    ->append("updating to ")
	    ->append(height);
	heightLog.flush();
	if (state_ != DeskState::Chill) {
		return false;
	}

	if (height > params_.bekantHeightMax || height < params_.bekantHeightMin) {
		return false;
	}

	uint16_t currentHeight = reader_.getHeight();

	if (targetHeight_ == currentHeight) {
		return true;
	}

	targetHeight_ = height;

	if (targetHeight_ > currentHeight) {
		moveUp_();
	} else {
		moveDown_();
	}

	return true;
}

void HeightPlatform::moveUp_() {
	digitalWrite(params_.bekantPinUp, HIGH);
	digitalWrite(params_.bekantPinDown, LOW);
	state_ = DeskState::MoveUp;

#ifdef IO_DEBUG
	heightLog.print("moving up");
#endif
}

void HeightPlatform::moveDown_() {
	digitalWrite(params_.bekantPinUp, LOW);
	digitalWrite(params_.bekantPinDown, HIGH);
	state_ = DeskState::MoveDown;

#ifdef IO_DEBUG
	heightLog.print("moving down");
#endif
}

void HeightPlatform::stop_() {
	digitalWrite(params_.bekantPinUp, LOW);
	digitalWrite(params_.bekantPinDown, LOW);
	state_ = DeskState::Chill;

#ifdef IO_DEBUG
	heightLog.print("stopping");
#endif
}
