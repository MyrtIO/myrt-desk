#pragma once

#include <pins.h>
#include <MyrtIO.h>
#include <interfaces/platform.h>
#include <BekantLIN.h>

enum class DeskState {
  Chill,
  MoveUp,
  MoveDown
};

class HeightPlatform : public IOPlatform, public IHeightPlatform {
  public:
    void setup();
    void setReader(BekantReader *reader);
    void onLoop();

    uint16_t getHeight();
    bool setHeight(uint16_t height);

  private:
    BekantReader *reader_;
    uint16_t targetHeight_ = 0;
    DeskState state_ = DeskState::Chill;

    void moveUp_();
    void moveDown_();
    void stop_();
};
