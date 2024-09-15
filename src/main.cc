#include "config.h"
#include "controllers/controllers.h"
#include "platform/platform.h"
#include <MyrtIO.h>
#include <WiFi.h>

IODevice desk;

IOLogger rootLog("root", &Serial);

void setup() {
    Serial.begin();
#ifdef IO_DEBUG
    while (!Serial) {
        // Wait for serial port to connect
    }
#endif
    rootLog.print("starting desk...");
    // clang-format off
    desk.setup()
        ->platform(
            IO_INJECT_INSTANCE(HeightPlatform),
            IO_INJECT_INSTANCE(LightPlatform),
            IO_INJECT_INSTANCE(WiFiPlatform)
        )
        ->controllers(&ConnectionController, &MQTTController);
    // clang-format on
    rootLog.print("desk is initialized");
}

void loop() {
    desk.loop();
}
