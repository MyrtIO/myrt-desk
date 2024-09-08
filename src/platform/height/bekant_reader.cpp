#include "bekant_reader.h"
#include <config.h>

void BekantReader::begin(Stream *stream) {
  stream_ = stream;
  handle();
}

void BekantReader::handle() {
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

uint16_t BekantReader::getHeight() {
  if (position_ == 0) {
    return 0;
  }
  return positionToHeight_(position_);
}

void BekantReader::clear_() {
  head_ = 0;
}

void BekantReader::tryParsePosition_() {
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

bool BekantReader::isPositionValid_(uint16_t position) {
  return (position >= POSITION_MIN && position <= POSITION_MAX);
}

uint16_t BekantReader::positionToHeight_(uint16_t position) {
  float height = CONFIG_BEKANT_HEIGHT_SLOPE * static_cast<float>(position);
  height += CONFIG_BEKANT_HEIGHT_BIAS;
  return static_cast<uint16_t>(round(height));
}
