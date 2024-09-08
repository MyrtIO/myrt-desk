#pragma once

#include <MyrtIO.h>
#include <Attotime.h>

class WiFiController_: public IOUnit {
  public:
    void setup();
    void loop();
    const char *name();

  private:
    bool isConnecting_ = false;
    bool wereConnected_ = false;
    Timer timeout_;

    bool connected_();
};

extern WiFiController_ WiFiController;
