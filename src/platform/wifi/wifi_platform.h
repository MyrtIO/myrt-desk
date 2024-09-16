#pragma once

#include <Attotime.h>
#include <MyrtIO.h>
#include <interfaces/platform.h>

enum WiFiState { Disconnected, Connecting, Connected };

class WiFiPlatform : public IOUnit, public IWiFiPlatform {
  public:
	void setup();
	void loop();
	const char* name();

	WiFiState state();
	bool connected();

  private:
	WiFiState state_ = Disconnected;
	Timer timeout_;
	bool firstConnect_ = true;
	Timer connectTimer_ = Timer();

	void connect_();
};
