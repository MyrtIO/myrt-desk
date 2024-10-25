#pragma once

#include <MyrtIO.h>
#include <PubSubServer.h>
#include <platform.h>

struct MQTTControllerParams {
	char* clientID;
	char* host;
	uint16_t port;
};

class MQTTController : public IOUnit, public PubSubServerListener {
  public:
	MQTTController(const MQTTControllerParams& params);

	// IOUnit interface
	const char* name();
	void setup();
	void loop();
	// PubSubServerListener interface
	void onConnect();
	void onDisconnect();
	void onMessage(char* topic, uint8_t* payload, uint16_t length);

  private:
	PubSubServer server_;
	MQTTControllerParams params_;
	WiFiClient client_ = WiFiClient();
	IOWiFi* wifi_ = DI_INJECT(IOWiFi);
};
