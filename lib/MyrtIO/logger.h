#pragma once

#include "Arduino.h"

class IFlusher {
    public:
        virtual void flush(const char* message, uint8_t length) = 0;
};

class IOLogBuilder {
  public:
    IOLogBuilder(IFlusher* flusher) : flusher_(flusher) {
        clear();
    }
    IOLogBuilder* append(const char* message);
    void flush();
    void clear();
    // void debug(const char* message);

    char* buffer();
    uint8_t size();

  private:
    IFlusher* flusher_ = nullptr;
    char buffer_[256];
    uint8_t size_ = 0;
};

class IOLogger: public IFlusher {
  public:
    IOLogger(Stream* stream) : stream_(stream){};
    IOLogger(const char* moduleName, Stream* stream)
    : moduleName_(moduleName), stream_(stream){};

    void print(const char* message);
    void debug(const char* message);
    void flush(const char* message, uint8_t length);
    IOLogBuilder* builder();

  private:
    Stream* stream_;
    const char* moduleName_ = nullptr;
    IOLogBuilder builder_ = IOLogBuilder(this);

    void printPrefix_();
};

extern IOLogger IOLog;
