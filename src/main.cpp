#include <WiFi.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <MyrtIO.h>
#include <LEDCoordinator.h>
#include <PubSubClient.h>

#include "config.h"
#include "platform.h"
#include "controllers.h"

IODevice desk;

void setup() {
    Serial.begin();
    IOLog.print("starting desk...");

    IOLog.print("connecting to WiFi...");
    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    WiFi.setHostname(CONFIG_DEVICE_NAME);
    while (WiFi.status() != WL_CONNECTED) {
      IOLog.print(".");
      delay(100);
    }
    IOLog.print("WiFi connected");

    desk.setup()
      ->platform(
        IO_INJECT_INSTANCE(HeightPlatform),
        IO_INJECT_INSTANCE(LightPlatform)
      )
      ->controllers(&MQTTController);

    IOLog.print("desk is initialized");
}

void loop() {
  desk.loop();
}
