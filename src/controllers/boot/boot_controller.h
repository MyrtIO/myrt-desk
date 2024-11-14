#pragma once

#include <Attotime.h>
#include <MyrtIO/device/unit.h>
#include <platforms.h>

class BootController : public io::Unit {
  public:
	BootController(RGBColor connectionColor):
		connectionColor_(connectionColor) {};
	void setup() override;
	void loop() override;
	const char* getName() override;

  private:
	bool firstConnect_;
	RGBColor connectionColor_;
	ILightPlatform* light_ = IO_INJECT(ILightPlatform);
	IWiFiPlatform* wifi_ = IO_INJECT(IWiFiPlatform);
};
