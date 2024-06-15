#pragma once

#include <Arduino.h>

class Stopwatch {
 public:
  void start() {
    _start = millis();
  }

  size_t split() {
    return millis() - _start;
  }

 private:
  uint32_t _start;
};
