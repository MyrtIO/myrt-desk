#pragma once

#include <MyrtIO_WiFiPlatform.h>

struct WiFiPlatformParams {
	const char* ssid;
	const char* password;
	const char* hostname;
	size_t startDelayMs;
};

class WiFiPlatform : public io::WiFiPlatform {
public:
	WiFiPlatform(const WiFiPlatformParams& params):
	  io::WiFiPlatform(params.ssid, params.password, params.hostname),
	  params_(params) {}

	const char* name();

	void onStateChange(WiFiState state);
	void onSetup();

protected:
	bool shouldConnect();

private:
	const WiFiPlatformParams params_;
	Timer startDelay_;
	bool firstConnect_ = true;
};

