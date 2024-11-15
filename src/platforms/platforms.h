#pragma once

#include <MyrtIO/device/di.h>

#include "height/height_platform.h"
#include "light/light_platform.h"
#include "wifi/wifi_platform.h"

IO_PROVIDE_IMPLEMENTATION(IWiFiPlatform);
IO_PROVIDE_IMPLEMENTATION(IHeightPlatform);
IO_PROVIDE_IMPLEMENTATION(ILightPlatform);
