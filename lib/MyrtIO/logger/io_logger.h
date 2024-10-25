#pragma once

#include <Arduino.h>
#include <Stream.h>
#include <stdint.h>
#include "io_string_builder.h"

class IOLogger {
  public:
	IOLogger(const char* moduleName) : moduleName_(moduleName) {}

	void print(const char* message);
	void flush();

	static void setOutput(Print* stream);

	IOStringBuilder* builder();

  private:
	const char* moduleName_ = nullptr;

	void appendPrefix_(IOStringBuilder* message);
};
