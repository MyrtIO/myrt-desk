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
        return;
    }

    wifiLog.builder()
    ->append("connecting to ")
    ->append(CONFIG_WIFI_SSID)
    ->append("...")
    ->flush();
    state_ = Connecting;
    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
}

const char* WiFiPlatform::name() {
    return kWiFiPlatformName;
}

bool WiFiPlatform::connected() {
    return state_ == Connected;
}
