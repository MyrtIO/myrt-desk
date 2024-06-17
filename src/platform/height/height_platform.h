#pragma once

#include <pins.h>
#include <MyrtIO.h>
#include <interfaces/platform.h>
#include <BekantLIN.h>

enum class DeskState {
  Chill,
  MoveUp,
  MoveDown
};

class HeightPlatform : public IOPlatform, public IHeightPlatform {
  public:
    void setup() {
      pinMode(PIN_TX_UP, OUTPUT);
      pinMode(PIN_TX_DOWN, OUTPUT);
    }

    void setReader(BekantReader *reader) {
      reader_ = reader;
    }

    void onLoop() {
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

    uint16_t getHeight() {
      return reader_->getHeight();
    }

    bool setHeight(uint16_t height) {
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

  private:
    BekantReader *reader_;
    uint16_t targetHeight_ = 0;
    DeskState state_ = DeskState::Chill;

    void moveUp_() {
      digitalWrite(PIN_TX_UP, HIGH);
      digitalWrite(PIN_TX_DOWN, LOW);
      state_ = DeskState::MoveUp;
    }

    void moveDown_() {
      digitalWrite(PIN_TX_UP, LOW);
      digitalWrite(PIN_TX_DOWN, HIGH);
      state_ = DeskState::MoveDown;
    }

    void stop_() {
      digitalWrite(PIN_TX_UP, LOW);
      digitalWrite(PIN_TX_DOWN, LOW);
      state_ = DeskState::Chill;
    }
};
