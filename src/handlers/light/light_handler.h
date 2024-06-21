#pragma once

#include <MyrtIO.h>
#include <io_di.h>
#include <platform.h>
#include "interfaces/features.h"
// #include "state.h"
// #include "stored.h"

class LightHandler : public IOHandler {
public:
  LightHandler();
  void setup() override;

  uint8_t code();
  bool onAction(IOActionRequest* request);

private:
  // Stored stateDescriptor_ = Stored(state_, FeatureCode::Indicators);
  ILightPlatform* leds_ = IO_INJECT(ILightPlatform);
  // IStoragePlatform* storage_ = IO_INJECT(IStoragePlatform);

  bool handleSetColor_(IOActionRequest* request);
  bool handleSetBrightness_(IOActionRequest* request);
  bool handleGetColor_(IOActionRequest* request);
  bool handleGetBrightness_(IOActionRequest* request);
  bool handleSetPower_(IOActionRequest* request);
  bool handleGetPower_(IOActionRequest* request);
  bool handleSetEffect_(IOActionRequest* request);
  bool handleGetEffect_(IOActionRequest* request);
};
