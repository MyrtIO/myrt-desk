#include <WiFi.h>
#include <MyrtIO.h>

#include "config.h"
#include "platform.h"
#include "controllers.h"

IODevice desk;

void setup() {
    Serial.begin();
    #ifdef CONFIG_DEBUG
    while (!Serial) {}
    #endif
    IOLog.print("starting desk...");

    desk.setup()
      ->platform(
        IO_INJECT_INSTANCE(HeightPlatform),
        IO_INJECT_INSTANCE(LightPlatform)
      )
      ->controllers(
        &WiFiController,
        &MQTTController
      );

    IOLog.print("desk is initialized");
}

void loop() {
  desk.loop();
}
