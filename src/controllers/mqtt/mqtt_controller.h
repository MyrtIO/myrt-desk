#pragma once

#include <MyrtIO_WiFiPlatform.h>
#include "topics/topics.h"

class MQTTController : public io::MQTTController {
  public:
	MQTTController(const io::MQTTControllerParams& params):
		io::MQTTController(params) {};
	void setupHandlers(PubSubServer* server);
};
