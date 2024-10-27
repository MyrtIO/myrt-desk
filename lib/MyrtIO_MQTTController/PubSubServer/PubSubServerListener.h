#pragma once

#include <stdint.h>

class PubSubServerListener {
	public:
		virtual void onConnect() {};
		virtual void onDisconnect() {};
		virtual void onMessage(char* topic, uint8_t* payload, uint16_t length) {};
};
