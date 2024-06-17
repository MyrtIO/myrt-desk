#include <WiFi.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <FastLED.h>
#include <MyrtIO.h>
#include <MyrtIO/udp_provider.h>
#include <BekantLIN.h>

#include <secrets.h>
#include <handlers.h>
#include <platform.h>
#include <pins.h>

UDPRequestProvider udpProvider;
IODispatcher io;
BekantReader heightReader;

LightHandler light;
HeightHandler height;

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

void setup() {
    // Clear LED strip
    LEDS.show();

    // Setup logging
    Serial.begin();

    // Setup height related stuff
    Serial2.setTX(PIN_TX_LIN);
    Serial2.setRX(PIN_RX_LIN);
    Serial2.begin(LIN_BAUD_RATE);
    pinMode(PIN_TX_UP, OUTPUT);
    pinMode(PIN_TX_DOWN, OUTPUT);
    heightReader.setup(&Serial2);
    auto heightPlatform = IO_INJECT_INSTANCE(HeightPlatform);
    heightPlatform->setReader(&heightReader);

    // Setup UDP server
    udpProvider.setListener(&io);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setHostname("MyrtDesk");
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    io.setup()
      ->handlers(
        &light,
        &height
      )
      ->platforms(
        IO_INJECT_INSTANCE(LightPlatform),
        IO_INJECT_INSTANCE(HeightPlatform)
      );
    multicore_launch_core1(udp_server_task);

    Serial.println("Setup is done");
}

uint8_t counter = 0;
uint8_t buffer[16];

void clear() {
    counter = 0;
    for (int i = 0; i < 16; i++) {
        buffer[i] = 0;
    }
}

void loop() {
  io.handle();
}
