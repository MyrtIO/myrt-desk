#pragma once

#include <Attotime.h>
#include <MyrtIO.h>

enum WiFiState : uint8_t {
	Disconnected,
	Connecting,
	Connected
};

class IWiFiPlatform {
  public:
	virtual bool connected() = 0;
	virtual const char* hostname() = 0;
	virtual WiFiState state() = 0;
};

namespace io {
	struct WiFiPlatformParams {
		const char* ssid;
		const char* password;
		const char* hostname;
		size_t connectDelay;
		size_t reconnectTimeout;
	};

	class WiFiPlatform : public Unit, public IWiFiPlatform {
	  public:
		WiFiPlatform(
			const WiFiPlatformParams& params
		): params_(params) {}

		void setup();
		void loop();
		const char* name();
		const char* hostname();

		WiFiState state();
		bool connected();

	  private:
		WiFiState state_ = Disconnected;
		Timer timeout_;
		Timer connectTimer_;
		bool firstConnect_ = true;
		WiFiPlatformParams params_;

		void connect_();
	};
}
