#pragma once

#include <Attotime.h>
#include <MyrtIO.h>
#include <platforms.h>

class BootController : public io::Unit {
  public:
	BootController(RGBColor connectionColor):
		connectionColor_(connectionColor) {};
	void setup();
	void loop();
	const char* name();

  private:
	bool firstConnect_ = false;
	IOLight* light_ = DI_INJECT(IOLight);
	IWiFiPlatform* wifi_ = DI_INJECT(IWiFiPlatform);
	effect_t previousEffect_ = LightEffect::Static;
	RGBColor connectionColor_;
};
