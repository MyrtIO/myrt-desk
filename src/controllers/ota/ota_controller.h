#pragma once

#include <platforms.h>

class OTAController : public IOUnit {
  public:
	void setup();
	void loop();
	const char* name();

  private:
	IOWiFi* wifi_ = DI_INJECT(IOWiFi);
	bool firstConnect_ = true;
};

