#include "MyrtIO.h"

#if IO_BENCHMARK
#include "io_benchmark.h"
#warning "Benchmarking enabled"
IOBenchmark ioBenchmark;
#endif

IOLogger deviceLog("IODevice", &Serial);

IODevice* IODevice::setup() {
    deviceLog.print("initializing...");
#if IO_BENCHMARK
    addController_(&ioBenchmark);
#endif
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
    deviceLog.builder()->append("adding controller: ")->append(c->name())->flush();
    c->setup();
    if (controllersCount_ >= IO_DEVICE_MAX_CONTROLLERS) {
        deviceLog.print("too many controllers");
        return false;
    }
    controllers_[controllersCount_] = c;
    controllersCount_++;
    deviceLog.print("controller added");
    return true;
}

bool IODevice::addPlatform_(IOUnit* p) {
    deviceLog.builder()->append("adding platform: ")->append(p->name())->flush();
    p->setup();
    if (platformsCount_ >= IO_DEVICE_MAX_PLATFORMS) {
        deviceLog.print("too many platforms");
        return false;
    }
    platforms_[platformsCount_] = p;
    platformsCount_++;
    deviceLog.print("platform added");
    return true;
}
