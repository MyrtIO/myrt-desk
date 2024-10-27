#include "MyrtIO_WiFiPlatform.h"
#include <WiFi.h>

void io::WiFiPlatform::setup() {
	state_ = WiFiState::Disconnected;
	WiFi.setHostname(hostname_);
	onSetup();
}

void io::WiFiPlatform::loop() {
	if (WiFi.status() == WL_CONNECTED) {
		setState_(WiFiState::Connected);
		return;
	}

	switch (state_) {
	case WiFiState::Connecting:
		if (timeout_.finished()) {
			connect_();
		}
	default:
		connect_();
		break;
	}
}

bool io::WiFiPlatform::connected() {
	return state_ == WiFiState::Connected;
}

WiFiState io::WiFiPlatform::state() {
	return state_;
}

const char* io::WiFiPlatform::hostname() {
	return hostname_;
}

void io::WiFiPlatform::setTimeout(size_t timeoutMs) {
	timeoutMs_ = timeoutMs;
}

void io::WiFiPlatform::connect_() {
	if (!shouldConnect()) {
		return;
	}
	setState_(WiFiState::Connecting);
	WiFi.begin(ssid_, password_);
	timeout_.start(timeoutMs_);
}

void io::WiFiPlatform::setState_(WiFiState state) {
	if (state_ == state) {
		return;
	}
	state_ = state;
	onStateChange(state);
}
