#include "height_platform.h"

void HeightPlatform::setup() {
  pinMode(PIN_TX_UP, OUTPUT);
  pinMode(PIN_TX_DOWN, OUTPUT);
  if (reader_ == nullptr) {
    Serial.println("No bekant reader set");
    return;
  }
  reader_->handle();
}

void HeightPlatform::setReader(BekantReader *reader) {
  reader_ = reader;
}

void HeightPlatform::loop() {
  if (reader_ == nullptr) {
    Serial.println("No bekant reader set");
    return;
  }
  reader_->handle();
  switch (state_) {
  case DeskState::MoveUp:
    if (reader_->getHeight() >= targetHeight_) {
      stop_();
    }
    break;
  case DeskState::MoveDown:
    if (reader_->getHeight() <= targetHeight_) {
      stop_();
    }
    break;
  }
}

uint16_t HeightPlatform::getHeight() {
  return reader_->getHeight();
}

bool HeightPlatform::setHeight(uint16_t height) {
  if (reader_ == nullptr || state_ != DeskState::Chill) {
    return false;
  }
  if (height > HEIGHT_MAX || height < HEIGHT_MIN) {
    return false;
  }
  uint16_t currentHeight = reader_->getHeight();
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
  digitalWrite(PIN_TX_UP, HIGH);
  digitalWrite(PIN_TX_DOWN, LOW);
  state_ = DeskState::MoveUp;
}

void HeightPlatform::moveDown_() {
  digitalWrite(PIN_TX_UP, LOW);
  digitalWrite(PIN_TX_DOWN, HIGH);
  state_ = DeskState::MoveDown;
}

void HeightPlatform::stop_() {
  digitalWrite(PIN_TX_UP, LOW);
  digitalWrite(PIN_TX_DOWN, LOW);
  state_ = DeskState::Chill;
}
