#include "wifi_controller.h"
#include <WiFi.h>
#include <config.h>

const char *kWiFiControllerName = "WiFi";

IOLogger wifiLog(kWiFiControllerName, &Serial);

WiFiController_ WiFiController;

void WiFiController_::setup() {
  isConnecting_ = false;
  WiFi.setHostname(CONFIG_DEVICE_NAME);
}

void WiFiController_::loop() {
  if (connected_()) {
    if (isConnecting_) {
      wifiLog.print("connected");
      wereConnected_ = true;
      isConnecting_ = false;
    }
    return;
  }

  if (wereConnected_) {
    wifiLog.print("disconnected");
    wereConnected_ = false;
  }

  if (isConnecting_) {
    if (timeout_.finished()) {
      wifiLog.print("connection timeout");
      isConnecting_ = false;
    }
    return;
  }

  wifiLog.print("connecting to WiFi...");
  timeout_.start(CONFIG_WIFI_RECONNECT_TIMEOUT);
  WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
  isConnecting_ = true;
}

const char *WiFiController_::name() {
  return kWiFiControllerName;
}

bool WiFiController_::connected_() {
  return WiFi.status() == WL_CONNECTED;
}

