
#include "Arduino.h"

class IOLogger {
  public:
    IOLogger(Stream *stream) : stream_(stream) {};
    IOLogger(const char* moduleName, Stream *stream):
      stream_(stream),
      moduleName_(moduleName) {};
    void print(const char* message);
    void debug(const char* message);
    void append(const char* message);
    void flush();

  private:
    Stream* stream_;
    const char* moduleName_ = nullptr;
    char buffer_[256];
    char size_ = 0;

    void printPrefix_();
};

extern IOLogger IOLog;
