#pragma once

#include "boot/boot_controller.h"
#include "mqtt/mqtt_controller.h"
#include "ota/ota_controller.h"

DI_PROVIDE_INSTANCE(BootController);
DI_PROVIDE_INSTANCE(MQTTController);
DI_PROVIDE_INSTANCE(OTAController);
