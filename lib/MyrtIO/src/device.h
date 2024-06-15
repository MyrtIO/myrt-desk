#pragma once

#include <stdint.h>
#include "request.h"
#include "action_request.h"

#define IO_DEVICE_MAX_FEATURES 8
#define IO_DEVICE_MAX_PLATFORMS 8

class IOPlatform {
  public:
    virtual void setup() = 0;
    virtual void onLoop() = 0;
};

class IOHandler {
  public:
    virtual void setup() = 0;
    virtual uint8_t code() = 0;
    virtual bool onAction(IOActionRequest* request) = 0;
};

class IODevice {
  public:
    uint8_t handlersCount = 0;
    uint8_t platformsCount = 0;
    IOHandler* handlerList[IO_DEVICE_MAX_FEATURES];
    IOPlatform* platformList[IO_DEVICE_MAX_PLATFORMS];

    template<typename... Args>
    IODevice* handlers(IOHandler *last) {
      addHandler_(last);
      return this;
    }

    template<typename... Args>
    IODevice* handlers(IOHandler* first, Args... args) {
      addHandler_(first);
      return features(args...);
    }

    template<typename... Args>
    IODevice* platforms(IOPlatform *last) {
      addPlatform_(last);
      return this;
    }

    template<typename... Args>
    IODevice* platforms(IOPlatform* first, Args... args) {
      addPlatform_(first);
      return platforms(args...);
    }

  private:
    bool addHandler_(IOHandler* c) {
      c->setup();
      if (handlersCount >= IO_DEVICE_MAX_FEATURES) {
        // TODO: add handling
        return false;
      }
      handlerList[handlersCount] = c;
      handlersCount++;
      return true;
    }

    bool addPlatform_(IOPlatform* c) {
      Serial.println("IODevice::addPlatform_");
      c->setup();
      if (platformsCount >= IO_DEVICE_MAX_PLATFORMS) {
        // TODO: add handling
        return false;
      }
      platformList[platformsCount] = c;
      platformsCount++;
      return true;
    }
};
