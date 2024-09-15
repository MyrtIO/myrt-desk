#pragma once

// Common parameters
#define CONFIG_DEVICE_NAME "MyrtDesk"
// Wi-Fi parameters
#define CONFIG_WIFI_SSID "Your SSID" // replace: "Your SSID"
#define CONFIG_WIFI_PASSWORD "Your password" // replace: "Your password"
#define CONFIG_WIFI_RECONNECT_TIMEOUT 10000
// MQTT parameters
#define CONFIG_MQTT_HOST "your.mqtt.server.local" // replace: "your.mqtt.server.local"
#define CONFIG_MQTT_PORT 1883
#define CONFIG_MQTT_BUFFER_SIZE 1024
// GPIO pins
#define CONFIG_PIN_LED_CTL 3
#define CONFIG_PIN_LIN_TX 4
#define CONFIG_PIN_LIN_RX 5
#define CONFIG_PIN_BUTTON_UP 8
#define CONFIG_PIN_BUTTON_DOWN 9
// Bekant legs parameters
#define CONFIG_BEKANT_HEIGHT_MAX 1260
#define CONFIG_BEKANT_HEIGHT_MIN 650
#define CONFIG_BEKANT_HEIGHT_SLOPE 0.09792
#define CONFIG_BEKANT_HEIGHT_BIAS 633.3536
#define CONFIG_BEKANT_LIN_UART Serial2
// Light parameters
#define CONFIG_LIGHT_WORKER_PIO pio1
#define CONFIG_LIGHT_WORKER_SM 1
#define CONFIG_LIGHT_LED_COUNT 92
