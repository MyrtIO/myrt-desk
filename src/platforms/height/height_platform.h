#pragma once

#include <MyrtIO/device/unit.h>
#include "height_interface.h"
#include "BekantReader/bekant_reader.h"

enum class DeskState { Chill, MoveUp, MoveDown };

struct HeightPlatformParams {
	SerialUART* linUART = nullptr;
	uint8_t linPinTX = 0;
	uint8_t linPinRX = 0;
	uint8_t bekantPinUp = 0;
	uint8_t bekantPinDown = 0;
	uint16_t bekantHeightMax = 0;
	uint16_t bekantHeightMin = 0;
	float bekantHeightSlope = 0;
	float bekantHeightBias = 0;
};

class HeightPlatform : public io::Unit, public IHeightPlatform {
  public:
	HeightPlatform(const HeightPlatformParams& params): params_(params) {}
	const char* getName() override;
	void setup() override;
	void loop() override;


	uint16_t get();
	bool set(uint16_t height);

  private:
	BekantReader reader_;
	uint16_t targetHeight_ = 0;
	DeskState state_ = DeskState::Chill;
	HeightPlatformParams params_;

	void moveUp_();
	void moveDown_();
	void stop_();
};
