#pragma once

#include "request.h"
#include "device.h"

class IODispatcher : public IORequestListener {
  public:
    // Initializes the IODevice
    IODevice* setup();

    // Handles incoming IORequest
    void onRequest(IORequest* request) override;

    // Primary handler for processing
    void handle();
  private:
      IODevice* device_ = nullptr;

      void runAction_(IORequest* request, IOHandler* target);
};
