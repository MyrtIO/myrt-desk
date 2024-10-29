#pragma once

#include "boot/boot_controller.h"
#include "mqtt/mqtt_controller.h"
#include "ota/ota_controller.h"

IO_PROVIDE_INSTANCE(BootController);
IO_PROVIDE_INSTANCE(MQTTController);
IO_PROVIDE_INSTANCE(OTAController);
