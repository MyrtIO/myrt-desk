#include "config.h"
#include "controllers/controllers.h"
#include "platform/platform.h"
#include <MyrtIO.h>
#include <WiFi.h>

IODevice desk;

void setup() {
    Serial.begin();
#ifdef CONFIG_DEBUG
    // Wait for serial port to connect
    while (!Serial) {
    }
#endif
    IOLog.print("starting desk...");
    auto height = IO_INJECT_INSTANCE(HeightPlatform);
    auto light  = IO_INJECT_INSTANCE(LightPlatform);
    auto wifi   = IO_INJECT_INSTANCE(WiFiPlatform);
    desk.setup()->platform(height, light, wifi)->controllers(&ConnectionController, &MQTTController);
    IOLog.print("desk is initialized");
}

void loop() {
    desk.loop();
}
