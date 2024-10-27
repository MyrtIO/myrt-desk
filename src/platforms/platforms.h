#pragma once

#include <MyrtIO.h>
#include <MyrtIO_WiFiPlatform.h>

#include "height/height_platform.h"
#include "light/light_platform.h"
#include "wifi/wifi_platform.h"

DI_PROVIDE_IMPLEMENTATION(IWiFiPlatform);
DI_PROVIDE_IMPLEMENTATION(IOHeight);
DI_PROVIDE_IMPLEMENTATION(IOLight);
