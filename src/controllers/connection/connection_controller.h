#pragma once

#include <MyrtIO.h>
#include <Attotime.h>
#include <platform.h>

class ConnectionController_: public IOUnit {
  public:
    void setup();
    void loop();
    const char *name();

  private:
    bool wereConnected_ = false;
    ILightPlatform* light_ = IO_INJECT(ILightPlatform);
    IWiFiPlatform *wifi_   = IO_INJECT(IWiFiPlatform);
    effect_t previousEffect_ = LightEffect::Static;
};

extern ConnectionController_ ConnectionController;
