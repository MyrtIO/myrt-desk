#pragma once

#include <MyrtIO.h>
#include <interfaces/platform.h>
#include <Attotime.h>

enum WiFiState { Disconnected, Connecting, Connected };

class WiFiPlatform : public IOUnit, public IWiFiPlatform {
  public:
    void setup();
    void loop();
    const char* name();

    WiFiState state();
    bool connected();

  private:
    WiFiState state_ = Disconnected;
    Timer timeout_;

    void connect_();
};
