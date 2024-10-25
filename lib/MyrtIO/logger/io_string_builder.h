#pragma once

#include <stdint.h>

#ifndef IO_LOGGER_BUFFER_SIZE
	#define IO_LOGGER_BUFFER_SIZE 256
#endif

class IOStringBuilder {
  public:
	IOStringBuilder* append(const char* message);
	IOStringBuilder* append(const char symbol);
	IOStringBuilder* append(const uint8_t message);
	IOStringBuilder* append(const uint16_t message);
	IOStringBuilder* append(const uint32_t message);
	IOStringBuilder* clear();

	uint16_t size() { return size_; }
	char* buffer() { return buffer_; }

  private:
	char buffer_[IO_LOGGER_BUFFER_SIZE];
	uint16_t size_ = 0;
	uint16_t offset_ = 0;
};
