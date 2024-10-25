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
	IOWiFi* wifi_ = DI_INJECT(IOWiFi);
	bool firstConnect_ = true;
	OTAControllerParams params_;
};

