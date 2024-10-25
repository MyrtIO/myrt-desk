#pragma once

#include <Attotime.h>
#include <MyrtIO.h>
#include <platform.h>

class BootController : public IOUnit {
  public:
	BootController(RGBColor connectionColor):
		connectionColor_(connectionColor) {};
	void setup();
	void loop();
	const char* name();

  private:
	bool firstConnect_ = false;
	IOLight* light_ = DI_INJECT(IOLight);
	IOWiFi* wifi_ = DI_INJECT(IOWiFi);
	effect_t previousEffect_ = LightEffect::Static;
	RGBColor connectionColor_;
};
