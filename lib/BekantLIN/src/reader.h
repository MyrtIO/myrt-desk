#pragma once

#include <Arduino.h>

#define LIN_BAUD_RATE 19230
#define LIN_SYNC_MASK 0x55
#define LIN_POSITION_PID 0x92
#define POSITION_MIN 170
#define HEIGHT_MAX 1260
#define HEIGHT_MIN 650
#define POSITION_MAX 6400
#define POSITION_TO_HEIGHT_SLOPE 0.09792
#define POSITION_TO_HEIGHT_BIAS 633.3536

class BekantReader {
  public:
    void setup(Stream *stream) {
      stream_ = stream;
    }

    void handle() {
      while (stream_->available()) {
        uint8_t byte = Serial2.read();
        if (byte == LIN_SYNC_MASK) {
            clear_();
            buffer_[head_] = byte;
            head_++;
        } else if (head_ == 5) {
          clear_();
        } else if (head_ > 0) {
            buffer_[head_] = byte;
            head_++;
        }
        if (head_ == 5) {
          tryParsePosition_();
          clear_();
        }
      }
    }

    uint16_t getHeight() {
      float height = POSITION_TO_HEIGHT_SLOPE * static_cast<float>(position_) + POSITION_TO_HEIGHT_BIAS;
      return static_cast<uint16_t>(round(height));
    }

  private:
    Stream *stream_;
    uint16_t position_;
    uint8_t head_ = 0;
    uint8_t buffer_[5];

    void clear_() {
      head_ = 0;
    }

    void tryParsePosition_() {
       for (uint8_t index = 0; index < 5; index++) {
          if (buffer_[index + 1] == LIN_POSITION_PID) {
            uint16_t position{buffer_[index + 3]};
            position <<= 8;
            position |= buffer_[index + 2];
            if (isPositionValid_(position)) {
              position_ = position;
            }
          }
        }
    }


    bool isPositionValid_(uint16_t position) {
      return (position >= POSITION_MIN && position <= POSITION_MAX);
    }

    uint16_t position_to_height_() {
      float height = POSITION_TO_HEIGHT_SLOPE * static_cast<float>(position_) + POSITION_TO_HEIGHT_BIAS;
      return static_cast<uint16_t>(round(height));
    }
};
