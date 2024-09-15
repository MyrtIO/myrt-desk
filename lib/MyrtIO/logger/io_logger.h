#pragma once

#include <Arduino.h>
#include <Stream.h>
#include <stdint.h>
#include "string_builder.h"

class IOLogger : public IStringFlusher {
  public:
    IOLogger(Stream* stream = &Serial) : stream_(stream){};
    IOLogger(const char* moduleName, Stream* stream = &Serial)
    : moduleName_(moduleName), stream_(stream){};

    void print(const char* message);
    void debug(const char* message);
    void flush(const char* message, uint8_t length);
    IStringBuilder* builder();
    IStringBuilder* debugBuilder();

  private:
    Stream* stream_;
    const char* moduleName_ = nullptr;
    IOStringBuilder builder_ = IOStringBuilder(this);
    #ifndef IO_DEBUG
    IODummyStringBuilder dummyBuilder_;
    #endif

    void printPrefix_();
};
