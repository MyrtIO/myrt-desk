#pragma once

#include <MyrtIO_WiFiPlatform.h>
#include <PubSubServer/PubSubServer.h>

namespace io {
	class TopicHandler {
	public:
		virtual void registerHandler(PubSubServer* server) = 0;
	};

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
		PubSubServer server_;
		MQTTControllerParams params_;
		WiFiClient client_;
		IWiFiPlatform* wifi_ = DI_INJECT(IWiFiPlatform);
	};
};
