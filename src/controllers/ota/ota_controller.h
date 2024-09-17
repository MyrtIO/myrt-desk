#pragma once

#include <platform.h>

class OTAController_ : public IOUnit {
  public:
	void setup();
	void loop();
	const char* name();

  private:
	IWiFiPlatform* wifi_ = IO_INJECT(IWiFiPlatform);
	bool firstConnect_ = true;
};

extern OTAController_ OTAController;
