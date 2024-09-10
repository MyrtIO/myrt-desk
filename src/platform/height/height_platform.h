#pragma once

#include "bekant_reader.h"
#include <MyrtIO.h>
#include <config.h>
#include <interfaces/platform.h>

enum class DeskState { Chill, MoveUp, MoveDown };

class HeightPlatform : public IOUnit, public IHeightPlatform {
  public:
    void setup();
    void loop();

    const char* name();

    uint16_t getHeight();
    bool setHeight(uint16_t height);

  private:
    BekantReader reader_;
    uint16_t targetHeight_ = 0;
    DeskState state_ = DeskState::Chill;
    SerialUART* stream_ = &CONFIG_BEKANT_LIN_UART;

    void moveUp_();
    void moveDown_();
    void stop_();
};
