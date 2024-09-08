#pragma once
#include <MyrtIO.h>
#include <io_di.h>

#include "platform/light/light_platform.h"
#include "platform/height/height_platform.h"

IO_PROVIDE(ILightPlatform, LightPlatform);
IO_PROVIDE(IHeightPlatform, HeightPlatform);
