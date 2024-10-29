#pragma once

#include <PubSubClient.h>
#include "TopicHandler.h"
#include "PubSubServerListener.h"

#define PUBSUBSERVER_MAX_HANDLERS 10
#define PUBSUBSERVER_MAX_REPORTERS 10
#define PUBSUBSERVER_CONNECTION_TIMEOUT 5000


class PubSubServer {
  public:
	PubSubServer(
		Client& client,
		const char* clientID
	);

	void start(char* host, uint16_t port, size_t bufferSize = 1024);
	void loop();
	void setListener(PubSubServerListener* listener) {
		listener_ = listener;
	}

	PubSubServer* on(const char* topic, DataHandler_t callback);
	PubSubServer* report(ReportHandler_t callback, size_t interval);

	// Public for internal use, not intended for external use
	void handleMessage_(char* topic, byte* payload, unsigned int length);

  private:
	PubSubClient client_;
	const char* clientID_;
	TopicHandler handlers_[PUBSUBSERVER_MAX_HANDLERS];
	uint8_t handlersCount_ = 0;
	TopicReporter reporters_[PUBSUBSERVER_MAX_REPORTERS];
	uint8_t reportersCount_ = 0;

	PubSubServerListener* listener_ = nullptr;
	bool connected_ = false;

	void handleReports_();
	void subscribe_();
	void reportAll_();
	bool keepConnected_();
};
