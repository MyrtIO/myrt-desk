#pragma once

#include <stdint.h>

class PubSubServerListener {
public:
	virtual void onConnect() = 0;
	virtual void onDisconnect() = 0;
	virtual void onMessage(char* topic, byte* payload, uint16_t length) = 0;
};
