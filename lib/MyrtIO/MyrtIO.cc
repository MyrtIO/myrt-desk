#include "MyrtIO.h"

IOLogger deviceLog("IODevice", &Serial);

IODevice* IODevice::setup() {
    deviceLog.print("initializing...");
    return this;
}

void IODevice::loop() {
    for (int i = 0; i < platformsCount_; i++) {
        platforms_[i]->loop();
    }
    for (int i = 0; i < controllersCount_; i++) {
        controllers_[i]->loop();
    }
}

bool IODevice::addController_(IOUnit* c) {
    deviceLog.append("adding controller: ");
    deviceLog.append(c->name());
    deviceLog.flush();
    c->setup();
    if (controllersCount_ >= IO_DEVICE_MAX_CONTROLLERS) {
        deviceLog.print("too many controllers");
        // TODO: add handling
        return false;
    }
    controllers_[controllersCount_] = c;
    controllersCount_++;
    deviceLog.print("controller added");
    return true;
}

bool IODevice::addPlatform_(IOUnit* p) {
    deviceLog.append("adding platform: ");
    deviceLog.append(p->name());
    deviceLog.flush();
    p->setup();
    if (platformsCount_ >= IO_DEVICE_MAX_PLATFORMS) {
        // TODO: add handling
        deviceLog.print("too many platforms");
        return false;
    }
    platforms_[platformsCount_] = p;
    platformsCount_++;
    deviceLog.print("platform added");
    return true;
}
