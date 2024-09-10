#pragma once

#include "Arduino.h"

class IFlusher {
  public:
    virtual void flush(const char* message, uint8_t length) = 0;
};

class IOLogBuilder {
  public:
    IOLogBuilder(IFlusher* flusher) : flusher_(flusher) {
    }
    IOLogBuilder* append(const char* message);
    IOLogBuilder* append(const uint8_t message);
    IOLogBuilder* append(const uint16_t message);
    IOLogBuilder* append(const uint32_t message);
    void prepare(bool debug);
    void flush();

    char* buffer();
    uint8_t size();

  private:
    IFlusher* flusher_ = nullptr;
    bool debug_ = false;
    char buffer_[256];
    uint8_t size_ = 0;
};

class IOLogger : public IFlusher {
  public:
    IOLogger(Stream* stream) : stream_(stream){};
    IOLogger(const char* moduleName, Stream* stream)
    : moduleName_(moduleName), stream_(stream){};

    void print(const char* message);
    void debug(const char* message);
    void flush(const char* message, uint8_t length);
    IOLogBuilder* builder();
    IOLogBuilder* debugBuilder();

  private:
    Stream* stream_;
    const char* moduleName_ = nullptr;
    IOLogBuilder builder_ = IOLogBuilder(this);

    void printPrefix_();
};

extern IOLogger IOLog;
