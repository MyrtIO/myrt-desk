#include "config.h"
#include "controllers/controllers.h"
#include "platform/platform.h"
#include <MyrtIO.h>
#include <WiFi.h>

IODevice desk;

#ifdef CONFIG_DEBUG
#define IO_LOG_DEBUG
#endif

void setup() {
    Serial.begin();
#ifdef CONFIG_DEBUG
    while (!Serial) {
        // Wait for serial port to connect
    }
#endif
    IOLog.print("starting desk...");
    // clang-format off
    desk.setup()
        ->platform(
            IO_INJECT_INSTANCE(HeightPlatform),
            IO_INJECT_INSTANCE(LightPlatform),
            IO_INJECT_INSTANCE(WiFiPlatform)
        )
        ->controllers(&ConnectionController, &MQTTController);
    // clang-format on
    IOLog.print("desk is initialized");
}

void loop() {
    desk.loop();
}
