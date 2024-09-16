#pragma once

#include <Arduino.h>

#define LIN_BAUD_RATE 19230
#define LIN_SYNC_MASK 0x55
#define LIN_POSITION_PID 0x92
#define POSITION_MIN 170
#define POSITION_MAX 6400

class BekantReader {
  public:
	void begin(Stream* stream);
	void handle();
	uint16_t getHeight();

  private:
	Stream* stream_;
	uint16_t position_;
	uint8_t head_ = 0;
	uint8_t buffer_[5];

	void clear_();
	void tryParsePosition_();
	bool isPositionValid_(uint16_t position);
	uint16_t positionToHeight_(uint16_t position);
};
