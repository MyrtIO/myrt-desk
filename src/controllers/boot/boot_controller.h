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
	bool firstConnect_;
	RGBColor connectionColor_;
	ILightPlatform* light_ = IO_INJECT(ILightPlatform);
	IWiFiPlatform* wifi_ = IO_INJECT(IWiFiPlatform);
};
