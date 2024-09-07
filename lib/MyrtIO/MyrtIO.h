#pragma once

#include <stdint.h>
#include <Arduino.h>

#define IO_DEVICE_MAX_FEATURES 8
#define IO_DEVICE_MAX_PLATFORMS 8

class IOPlatform {
  public:
    virtual void setup() = 0;
    virtual void loop() = 0;
};

class IOController {
  public:
    virtual void loop() = 0;
};

class IODevice {
  public:
    IODevice* setup() {
      return this;
    }

    void loop() {
      for (uint8_t i = 0; i < controllersCount_; i++) {
        controllers_[i]->loop();
      }
      for (uint8_t i = 0; i < platformsCount_; i++) {
        platforms_[i]->loop();
      }
    }

    template<typename... Args>
    IODevice* withControllers(IOController *last) {
      addController_(last);
      return this;
    }

    template<typename... Args>
    IODevice* withControllers(IOController* first, Args... args) {
      addController_(first);
      return withControllers(args...);
    }

    template<typename... Args>
    IODevice* withPlatforms(IOPlatform *last) {
      addPlatform_(last);
      return this;
    }

    template<typename... Args>
    IODevice* withPlatforms(IOPlatform* first, Args... args) {
      addPlatform_(first);
      return withPlatforms(args...);
    }

  private:
    uint8_t controllersCount_ = 0;
    uint8_t platformsCount_ = 0;
    IOController* controllers_[IO_DEVICE_MAX_FEATURES];
    IOPlatform* platforms_[IO_DEVICE_MAX_PLATFORMS];

    bool addController_(IOController* c) {
      if (controllersCount_ >= IO_DEVICE_MAX_FEATURES) {
        // TODO: add handling
        return false;
      }
      controllers_[controllersCount_] = c;
      controllersCount_++;
      return true;
    }

    bool addPlatform_(IOPlatform* p) {
      p->setup();
      if (platformsCount_ >= IO_DEVICE_MAX_PLATFORMS) {
        // TODO: add handling
        return false;
      }
      platforms_[platformsCount_] = p;
      platformsCount_++;
      return true;
    }
};
