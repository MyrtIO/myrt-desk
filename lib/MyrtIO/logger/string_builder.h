#pragma once

#include <stdint.h>

#ifndef IO_LOG_MAX_LENGTH
#define IO_LOG_MAX_LENGTH 256
#endif

class IStringFlusher {
  public:
    virtual void flush(const char* message, uint8_t length) = 0;
};

class IStringBuilder {
  public:
    virtual IStringBuilder* append(const char* message) = 0;
    virtual IStringBuilder* append(const uint8_t message) = 0;
    virtual IStringBuilder* append(const uint16_t message) = 0;
    virtual IStringBuilder* append(const uint32_t message) = 0;
    virtual void prepare() = 0;
    virtual void flush() = 0;
};

class IOStringBuilder: public IStringBuilder {
  public:
    IOStringBuilder(IStringFlusher* flusher) : flusher_(flusher) {}
    IOStringBuilder* append(const char* message);
    IOStringBuilder* append(const uint8_t message);
    IOStringBuilder* append(const uint16_t message);
    IOStringBuilder* append(const uint32_t message);
    void prepare();
    void flush();

  private:
    char buffer_[IO_LOG_MAX_LENGTH];
    uint8_t size_ = 0;
    IStringFlusher* flusher_ = nullptr;
};

#ifndef IO_DEBUG
class IODummyStringBuilder: public IStringBuilder {
  public:
    IODummyStringBuilder* append(const char* message) { return this; }
    IODummyStringBuilder* append(const uint8_t message) { return this; }
    IODummyStringBuilder* append(const uint16_t message) { return this; }
    IODummyStringBuilder* append(const uint32_t message) { return this; }
    void prepare() {}
    void flush() {}
};
#endif
