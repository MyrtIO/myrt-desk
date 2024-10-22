#pragma once

#include <Attotime.h>
#include <MyrtIO.h>
#include <platform.h>

class ConnectionController_ : public IOUnit {
  public:
	void setup();
	void loop();
	const char* name();

  private:
	bool firstConnect_ = false;
	ILightPlatform* light_ = IO_INJECT(ILightPlatform);
	IWiFiPlatform* wifi_ = IO_INJECT(IWiFiPlatform);
	effect_t previousEffect_ = LightEffect::Static;
};

extern ConnectionController_ ConnectionController;
