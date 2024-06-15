#include <WiFi.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <FastLED.h>
#include <MyrtIO.h>
#include <MyrtIO/udp_provider.h>
#include "platform/platform.h"
#include "features/features.h"
#include <secrets.h>

UDPRequestProvider udpProvider;
IODispatcher io;

LightFeature lightFeature;

#define UDP_PORT 11011

void udp_receive(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
  if (p != NULL) {
    udpProvider.handleMessage(pcb, p, addr, port);
    pbuf_free(p);
  }
}

void udp_server_task() {
    struct udp_pcb *pcb;
    pcb = udp_new();

    if (pcb == NULL) {
        Serial.printf("Failed to create UDP PCB\n");
        return;
    }

    if (udp_bind(pcb, IP_ADDR_ANY, UDP_PORT) != ERR_OK) {
        Serial.printf("Failed to bind UDP PCB\n");
        return;
    }

    udp_recv(pcb, udp_receive, NULL);

    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
}

CRGB leds[92];

void setup() {
    LEDS.show();
    Serial.begin(115200);
    udpProvider.setListener(&io);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setHostname("MyrtDesk");
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Still not...");
    }
    Serial.println("Connected!");
    io.setup()
      ->features(
        &lightFeature
      )
      ->platforms(
        IO_INJECT_INSTANCE(LEDPlatform)
      );
    multicore_launch_core1(udp_server_task);
}

void loop() {
  io.handle();
}
