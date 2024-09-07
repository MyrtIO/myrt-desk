#include <WiFi.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <MyrtIO.h>
#include <BekantLIN.h>
#include <LEDCoordinator.h>
#include <PubSubClient.h>

#include <MyrtQTT.h>

#include <secrets.h>
#include <platform.h>
#include <pins.h>

#include "topics/topics.h"

IODevice desk;

BekantReader heightReader;
LEDCoordinator coordinator;

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);
MyrtQTT mqtt(&pubSubClient, "MyrtDesk");

void topicCallback(char* topic, byte* payload, unsigned int length) {
  mqtt.handleMessage(topic, payload, length);
}

void setup() {
    // Setup logging
    Serial.begin();

    // Setup LED related stuff
    auto lightPlatform = IO_INJECT_INSTANCE(LightPlatform);
    lightPlatform->setCoordinator(&coordinator);

    // Setup height related stuff
    Serial2.setTX(PIN_TX_LIN);
    Serial2.setRX(PIN_RX_LIN);
    Serial2.begin(LIN_BAUD_RATE);
    heightReader.setup(&Serial2);
    auto heightPlatform = IO_INJECT_INSTANCE(HeightPlatform);
    heightPlatform->setReader(&heightReader);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setHostname("MyrtDesk");
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);
    }

    setupLightTopics(&mqtt);
    setupHeightTopics(&mqtt);
    pubSubClient.setCallback(topicCallback);
    pubSubClient.setBufferSize(1024);
    mqtt.setup("192.168.1.230", 1883);

    desk.setup()
      ->withPlatforms(heightPlatform, lightPlatform)
      ->withControllers(&mqtt);

    Serial.println("Setup is done");
}

void loop() {
  desk.loop();
}
