#pragma once

#include <stdint.h>
#include "logger/io_logger.h"

#define IO_DEVICE_MAX_CONTROLLERS 8
#define IO_DEVICE_MAX_PLATFORMS 8

class IOUnit {
  public:
    virtual const char* name() = 0;
    virtual void setup() = 0;
    virtual void loop() = 0;
};

class IODevice {
  public:
    IODevice* setup();
    void loop();

    template <typename... Args> IODevice* controllers(IOUnit* last) {
        addController_(last);
        return this;
    }

    template <typename... Args>
    IODevice* controllers(IOUnit* first, Args... args) {
        addController_(first);
        return controllers(args...);
    }

    template <typename... Args> IODevice* platform(IOUnit* last) {
        addPlatform_(last);
        return this;
    }

    template <typename... Args>
    IODevice* platform(IOUnit* first, Args... args) {
        addPlatform_(first);
        return platform(args...);
    }

  private:
    uint8_t controllersCount_ = 0;
    uint8_t platformsCount_ = 0;
    IOUnit* controllers_[IO_DEVICE_MAX_CONTROLLERS];
    IOUnit* platforms_[IO_DEVICE_MAX_PLATFORMS];

    bool addController_(IOUnit* c);
    bool addPlatform_(IOUnit* p);
};
