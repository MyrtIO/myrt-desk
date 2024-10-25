#pragma once

#include <MyrtIO.h>
#include <io_wifi_platform.h>

#include "height/height_platform.h"
#include "light/light_platform.h"

DI_PROVIDE_IMPLEMENTATION(IOWiFi);
DI_PROVIDE_IMPLEMENTATION(IOHeight);
DI_PROVIDE_IMPLEMENTATION(IOLight);
