#pragma once

#include <stdint.h>

struct RGB {
  union {
    struct {
      union {
        uint8_t r;
        uint8_t red;
      };
      union {
        uint8_t g;
        uint8_t green;
      };
      union {
          uint8_t b;
          uint8_t blue;
      };
    };
  };

  inline RGB() __attribute__((always_inline)) = default;

  inline bool operator==(const RGB& other) const __attribute__((always_inline)) {
    return r == other.r && g == other.g && b == other.b;
  }

  inline bool operator!=(const RGB& other) const __attribute__((always_inline)) {
    return r != other.r || g != other.g || b != other.b;
  }

  inline RGB& operator=(const RGB& other) __attribute__((always_inline)) {
    r = other.r;
    g = other.g;
    b = other.b;
    return *this;
  }

  constexpr RGB(uint8_t ir, uint8_t ig, uint8_t ib)
    : r(ir), g(ig), b(ib) {}

  constexpr RGB(uint32_t colorcode)  __attribute__((always_inline))
    : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b((colorcode >> 0) & 0xFF) {}

  uint32_t u32() const {
    return  ((uint32_t)(r) << 8) |
            ((uint32_t)(g) << 16) |
            (uint32_t)(b);
  }

  typedef enum {
    Black = 0x000000,
    Red = 0xFF0000,
    Green = 0x00FF00,
    Blue = 0x0000FF,
    White = 0xFFFFFF
  };
};
