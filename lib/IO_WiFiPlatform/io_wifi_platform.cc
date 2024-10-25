#include "io_wifi_platform.h"
#include <WiFi.h>

const char* kIOWiFiPlatformName = "WiFi";

IOLogger wifiLog(kIOWiFiPlatformName, &Serial);

void WiFiPlatform::setup() {
	state_ = Disconnected;
	WiFi.setHostname(params_.hostname);
}

void WiFiPlatform::loop() {
	if (WiFi.status() == WL_CONNECTED) {
		if (state_ != Connected) {
			state_ = Connected;
		}
		return;
	}

	if (firstConnect_) {
		firstConnect_ = false;
		connectTimer_.start(params_.connectDelay);
	}
	if (!connectTimer_.finished()) {
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
	connect_();
}

const char* WiFiPlatform::name() {
	return kIOWiFiPlatformName;
}

bool WiFiPlatform::connected() {
	return state_ == Connected;
}

WiFiState WiFiPlatform::state() {
	return state_;
}

const char* WiFiPlatform::hostname() {
	return params_.hostname;
}

void WiFiPlatform::connect_() {
	wifiLog.builder()
	    ->append("connecting to ")
	    ->append(params_.ssid)
	    ->append("...")
	    ->flush();
	state_ = Connecting;
	WiFi.begin(params_.ssid, params_.password);
	timeout_.start(params_.reconnectTimeout);
}
