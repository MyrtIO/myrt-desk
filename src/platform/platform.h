#pragma once
#include <MyrtIO.h>
#include <io_di.h>

#include "height/height_platform.h"
#include "light/light_platform.h"
#include "wifi/wifi_platform.h"

IO_PROVIDE(ILightPlatform, LightPlatform);
IO_PROVIDE(IHeightPlatform, HeightPlatform);
IO_PROVIDE(IWiFiPlatform, WiFiPlatform);
