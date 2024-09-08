#include "height_platform.h"

const char* kHeightPlatformName = "Height";

IOLogger heightLog(kHeightPlatformName, &Serial);

const char *HeightPlatform::name() {
  return kHeightPlatformName;
}

void HeightPlatform::setup() {
  pinMode(CONFIG_PIN_BUTTON_UP, OUTPUT);
  pinMode(CONFIG_PIN_BUTTON_DOWN, OUTPUT);
  heightLog.debug("starting LIN reader...");
  stream_->setTX(CONFIG_PIN_LIN_TX);
  stream_->setRX(CONFIG_PIN_LIN_RX);
  stream_->begin(LIN_BAUD_RATE);
  reader_.begin(stream_);
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
  heightLog.print("got update request");
  if (state_ != DeskState::Chill) {
    return false;
  }
  if (height > CONFIG_BEKANT_HEIGHT_MAX
   || height < CONFIG_BEKANT_HEIGHT_MIN) {
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

  heightLog.print("moving up");
}

void HeightPlatform::moveDown_() {
  digitalWrite(CONFIG_PIN_BUTTON_UP, LOW);
  digitalWrite(CONFIG_PIN_BUTTON_DOWN, HIGH);
  state_ = DeskState::MoveDown;

  heightLog.print("moving down");
}

void HeightPlatform::stop_() {
  digitalWrite(CONFIG_PIN_BUTTON_UP, LOW);
  digitalWrite(CONFIG_PIN_BUTTON_DOWN, LOW);
  state_ = DeskState::Chill;

  heightLog.print("stopping");
}
