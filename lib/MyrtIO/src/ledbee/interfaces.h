#pragma once

class IRenderer {
  public:
    virtual bool handleFrame() = 0;
};
