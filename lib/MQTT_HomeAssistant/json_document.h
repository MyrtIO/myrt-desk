#pragma once

#include <ArduinoJson.h>

#ifndef HOME_ASSISTANT_BUFFER_SIZE
	#define HOME_ASSISTANT_BUFFER_SIZE 1024
#endif

namespace HomeAssistant {
	extern StaticJsonDocument<HOME_ASSISTANT_BUFFER_SIZE> json;
}

