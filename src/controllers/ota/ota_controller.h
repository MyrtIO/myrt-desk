#pragma once

#include <platforms.h>

struct OTAControllerParams {
	char* hostname;
	uint16_t port;
};

class OTAController : public io::Unit {
  public:
	OTAController(const OTAControllerParams& params);
	void setup();
	void loop();
	const char* name();

  private:
	IWiFiPlatform* wifi_ = IO_INJECT(IWiFiPlatform);
	bool firstConnect_ = true;
	OTAControllerParams params_;
};

