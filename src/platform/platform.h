#pragma once
#include <MyrtIO.h>
#include <io_di.h>

#include "light/light_platform.h"
#include "height/height_platform.h"

IO_PROVIDE(ILightPlatform, LightPlatform);
IO_PROVIDE(IHeightPlatform, HeightPlatform);
// TODO: Implement storage platform
