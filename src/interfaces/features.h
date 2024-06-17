#pragma once

#include <stdint.h>

enum FeatureCode : uint8_t {
  System = 0,
  Light = 1,
  Height = 2,
};

enum SystemAction : uint8_t {
  Ping = 0,
  GetName = 1,
};

enum LightAction : uint8_t {
  SetColor = 0,
  GetColor = 1,
  SetBrightness = 2,
  GetBrightness = 3,
  SetPower = 4,
  GetPower = 5,
  SetEffect = 6,
  GetEffect = 7,
};

enum HeightAction : uint8_t {
  SetHeight = 0,
  GetHeight = 1,
};

// #define COMMAND_BACKLIGHT_READ 0
// #define COMMAND_BACKLIGHT_SET_EFFECT 1
// #define COMMAND_BACKLIGHT_SET_EFFECT_DATA 2
// #define COMMAND_BACKLIGHT_SET_COLOR 3
// #define COMMAND_BACKLIGHT_SET_WHITE 4
// #define COMMAND_BACKLIGHT_SET_BRIGHTNESS 5
// #define COMMAND_BACKLIGHT_SET_POWER 6
// #define COMMAND_BACKLIGHT_FIRMWARE_RECEIVE 7
// #define COMMAND_BACKLIGHT_FIRMWARE_FRAME 8
// #define COMMAND_BACKLIGHT_FIRMWARE_APPLY 9
// #define COMMAND_BACKLIGHT_BROADCAST_STATE 10
