#pragma once

#include <MyrtIO.h>
#include <PubSubServer/PubSubServer.h>
#include <WiFiClient.h>

namespace io {
	struct MQTTControllerParams {
		char* clientID;
		char* host;
		uint16_t port;
	};

	class MQTTController : public Unit, public PubSubServerListener {
	  public:

		MQTTController(const MQTTControllerParams& params);

		// Unit interface
		const char* name();
		void setup();
		void loop();

		// PubSubServerListener interface
		void onConnect();
		void onDisconnect();
		void onMessage(char* topic, uint8_t* payload, uint16_t length);

		// Controller interface
		virtual void setupHandlers(PubSubServer* server) = 0;

	  protected:
		MQTTControllerParams params_;
		PubSubServer server_;
		WiFiClient client_;
	};
};
