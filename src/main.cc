#include <MyrtIO.h>
#include <WiFi.h>
#include "config.h"
#include "platform/platform.h"
#include "controllers/controllers.h"

IODevice desk;

void setup() {
  Serial.begin();
  #ifdef CONFIG_DEBUG
  // Wait for serial port to connect
  while (!Serial) {}
  #endif
  IOLog.print("starting desk...");
  desk.setup()
    ->platform(
      IO_INJECT_INSTANCE(HeightPlatform),
      IO_INJECT_INSTANCE(LightPlatform),
      IO_INJECT_INSTANCE(WiFiPlatform)
    )
    ->controllers(
      &ConnectionController,
      &MQTTController
    );
  IOLog.print("desk is initialized");
}

void loop() {
  desk.loop();
}
