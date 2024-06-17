#pragma once

#include <MyrtIO.h>
#include <platform.h>
#include "interfaces/features.h"

class HeightHandler : public IOHandler {
public:
  HeightHandler();
  void setup() override;

  uint8_t code();
  bool onAction(IOActionRequest* request);

private:
  IHeightPlatform* legs_ = IO_INJECT(IHeightPlatform);

  bool handleSetHeight_(IOActionRequest* request);
  bool handleGetHeight_(IOActionRequest* request);
};
