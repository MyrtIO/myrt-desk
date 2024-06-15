// Copyright 2022, Mikhael Khrustik <misha@myrt.co>
//
// All components of Myrt Desk Firmware are licensed under the BSD 3-Clause
// License. See LICENSE.txt for details.

#pragma once

#include <MyrtIO.h>
#include <Timing.h>

class StaticEffect : public StripEffect {
 public:
  bool handleFrame(EffectState *state, Strip *strip);
  void finalize(EffectState *state);
  void onColorUpdate(EffectState *state);
  void prepare(EffectState *state, Strip *strip);
  bool setArguments(EffectState *state, uint8_t *args, uint8_t length);
  bool setData(EffectState *state, uint8_t *data, uint8_t length);

 private:
  bool _transitioned = false;
  Progress _progress = Progress();
};

extern StaticEffect Static;
