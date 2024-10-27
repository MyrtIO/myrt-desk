#include "wifi_platform.h"

char* kWiFiPlatformName = "WiFi";

io::Logger wifiLog(kWiFiPlatformName);

void WiFiPlatform::onSetup() {
	startDelay_.start(params_.startDelayMs);
}

void WiFiPlatform::onStateChange(WiFiState state) {
	switch (state) {
		case WiFiState::Connecting:
			wifiLog.builder()
				->append("connecting to ")
				->append(params_.ssid)
				->append("...");
			wifiLog.flush();
			break;
		case WiFiState::Connected:
			if (firstConnect_) {
				firstConnect_ = false;
			}
			wifiLog.print("connected");
			break;
		case WiFiState::Disconnected:
			wifiLog.print("disconnected");
			break;
		default:
			break;
	}
}

const char* WiFiPlatform::name() {
	return kWiFiPlatformName;
}

bool WiFiPlatform::shouldConnect() {
	return firstConnect_
		? startDelay_.finished()
		: true;
}
