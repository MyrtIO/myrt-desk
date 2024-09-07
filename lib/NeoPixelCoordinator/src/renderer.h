#pragma once

#include <LEDCoordinator.h>
#include "ws2812.h"

class WS2812Renderer : public IRenderer {
  public:
    WS2812Renderer(PioWS2812* ws2812) {
      ws2812_ = ws2812;
    }
    void apply();
  private:
    PioWS2812* ws2812_;
};
