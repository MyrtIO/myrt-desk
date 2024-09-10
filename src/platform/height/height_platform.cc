#include "height_platform.h"

const char* kHeightPlatformName = "Height";

IOLogger heightLog(kHeightPlatformName, &Serial);

const char* HeightPlatform::name() {
    return kHeightPlatformName;
}

void HeightPlatform::setup() {
    pinMode(CONFIG_PIN_BUTTON_UP, OUTPUT);
    pinMode(CONFIG_PIN_BUTTON_DOWN, OUTPUT);
    heightLog.print("starting UART...");
    stream_->setTX(CONFIG_PIN_LIN_TX);
    stream_->setRX(CONFIG_PIN_LIN_RX);
    stream_->begin(LIN_BAUD_RATE);
    heightLog.print("starting LIN reader...");
    reader_.begin(stream_);
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

uint16_t HeightPlatform::getHeight() {
    return reader_.getHeight();
}

bool HeightPlatform::setHeight(uint16_t height) {
    // clang-format off
    heightLog.debugBuilder()
        ->append("updating to to ")
        ->append(height)
        ->flush();
    // clang-format on
    if (state_ != DeskState::Chill) {
        return false;
    }
    if (height > CONFIG_BEKANT_HEIGHT_MAX || height < CONFIG_BEKANT_HEIGHT_MIN) {
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
    digitalWrite(CONFIG_PIN_BUTTON_UP, HIGH);
    digitalWrite(CONFIG_PIN_BUTTON_DOWN, LOW);
    state_ = DeskState::MoveUp;

    heightLog.debug("moving up");
}

void HeightPlatform::moveDown_() {
    digitalWrite(CONFIG_PIN_BUTTON_UP, LOW);
    digitalWrite(CONFIG_PIN_BUTTON_DOWN, HIGH);
    state_ = DeskState::MoveDown;

    heightLog.debug("moving down");
}

void HeightPlatform::stop_() {
    digitalWrite(CONFIG_PIN_BUTTON_UP, LOW);
    digitalWrite(CONFIG_PIN_BUTTON_DOWN, LOW);
    state_ = DeskState::Chill;

    heightLog.debug("stopping");
}
