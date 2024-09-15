#include "wifi_platform.h"
#include <WiFi.h>
#include <config.h>

const char* kWiFiPlatformName = "WiFi";

IOLogger wifiLog(kWiFiPlatformName, &Serial);

void WiFiPlatform::setup() {
    state_ = Disconnected;
    WiFi.setHostname(CONFIG_DEVICE_NAME);
}

void WiFiPlatform::loop() {
    if (WiFi.status() == WL_CONNECTED) {
        if (state_ != Connected) {
            wifiLog.print("connected!");
            state_ = Connected;
        }
        return;
    }

    if (state_ == Connecting) {
        if (timeout_.finished()) {
            wifiLog.print("timeout, reconnecting...");
            state_ = Disconnected;
            connect_();
        }
        return;
    }

    wifiLog.builder()
    ->append("connecting to ")
    ->append(CONFIG_WIFI_SSID)
    ->append("...")
    ->flush();
    connect_();
}

const char* WiFiPlatform::name() {
    return kWiFiPlatformName;
}

bool WiFiPlatform::connected() {
    return state_ == Connected;
}

WiFiState WiFiPlatform::state() {
    return state_;
}

void WiFiPlatform::connect_() {
    state_ = Connecting;
    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    timeout_.start(CONFIG_WIFI_RECONNECT_TIMEOUT);
}
