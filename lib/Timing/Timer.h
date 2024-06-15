#pragma once

#include <Arduino.h>

class Timer {
 public:
  bool isDone() {
    return millis() >= _end;
  }

  void set(size_t ms) {
    _end = millis() + ms;
  }

 private:
  uint32_t _end = 0;
};
