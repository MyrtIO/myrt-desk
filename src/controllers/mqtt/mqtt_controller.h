#pragma once

#include <MyrtIO/device/unit.h>
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
	const char* getName() override;
	void setup() override;
	void loop() override;

	// Controller interface
	Stream* logStream() {
		return &logStream_;
	}

	// PubSubServerListener interface
	void onConnect() override;
	void onDisconnect() override;
	void onMessage(char* topic, uint8_t* payload, uint16_t length) override;

private:
	MQTTControllerParams params_;
	PubSubServer server_;
	WiFiClient client_;
	RingBuffer<2048> logStream_;
};
