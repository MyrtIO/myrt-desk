#pragma once

#include <MyrtIO.h>
#include <Attotime.h>
#include <platform.h>

class WiFiController_: public IOUnit {
  public:
    void setup();
    void loop();
    const char *name();

  private:
    bool isConnecting_ = false;
    bool wereConnected_ = false;
    ILightPlatform* light_ = IO_INJECT(ILightPlatform);
    Timer timeout_;
    effect_t previousEffect_ = LightEffect::Static;

    bool connected_();
};

extern WiFiController_ WiFiController;
