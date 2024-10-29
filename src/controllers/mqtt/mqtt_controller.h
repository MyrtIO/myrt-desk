#pragma once

#include <MyrtIO.h>
#include <WiFiClient.h>
#include <RingBuffer.h>
#include "handlers/topics.h"

struct MQTTControllerParams {
	char* clientID;
	char* host;
	uint16_t port;
};

class MQTTController : public io::Unit, public PubSubServerListener {
public:
	MQTTController(const MQTTControllerParams& params):
		params_(params),
		server_(PubSubServer(client_, params.clientID)),
		client_(WiFiClient()) {
		server_.setListener(this);
	};

	// Unit interface
	const char* name();
	void setup();
	void loop();

	// Controller interface
	Stream* logStream() {
		return &logStream_;
	}

	// PubSubServerListener interface
	void onConnect();
	void onDisconnect();
	void onMessage(char* topic, uint8_t* payload, uint16_t length);

private:
	MQTTControllerParams params_;
	PubSubServer server_;
	WiFiClient client_;
	RingBuffer<2048> logStream_;
};
