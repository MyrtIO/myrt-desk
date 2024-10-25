#include "MyrtIO_WiFiPlatform.h"
#include <WiFi.h>

const char* kWiFiPlatformName = "WiFi";

io::Logger wifiLog(kWiFiPlatformName);

void io::WiFiPlatform::setup() {
	state_ = Disconnected;
	WiFi.setHostname(params_.hostname);
}

void io::WiFiPlatform::loop() {
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

const char* io::WiFiPlatform::name() {
	return kWiFiPlatformName;
}

bool io::WiFiPlatform::connected() {
	return state_ == Connected;
}

WiFiState io::WiFiPlatform::state() {
	return state_;
}

const char* io::WiFiPlatform::hostname() {
	return params_.hostname;
}

void io::WiFiPlatform::connect_() {
	wifiLog.builder()
	    ->append("connecting to ")
	    ->append(params_.ssid)
	    ->append("...");
	wifiLog.flush();
	state_ = Connecting;
	WiFi.begin(params_.ssid, params_.password);
	timeout_.start(params_.reconnectTimeout);
}
