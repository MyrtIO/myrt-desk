#include "PubSubServer.h"
#include <WiFi.h>

PubSubServer* activePubSubServer = nullptr;

void handleTopicMessage(char* topic, byte* payload, unsigned int length) {
	if (activePubSubServer != nullptr) {
		activePubSubServer->handleMessage_(topic, payload, length);
	}
}

PubSubServer::PubSubServer(
	Client& client,
	const char* clientID
) : client_(PubSubClient(client)),
	clientID_(clientID) {}

void PubSubServer::start(char* host, uint16_t port, size_t bufferSize) {
	activePubSubServer = this;
	client_.setServer(host, port);
	client_.setBufferSize(bufferSize);
	client_.setCallback(handleTopicMessage);
}

void PubSubServer::loop() {
	client_.loop();
	if (!keepConnected_()) {
		return;
	}
	handleReports_();
}

PubSubServer* PubSubServer::on(const char* topic, DataHandler_t callback) {
	if (handlersCount_ < PUBSUBSERVER_MAX_HANDLERS) {
		handlers_[handlersCount_].topic = topic;
		handlers_[handlersCount_].callback = callback;
		handlersCount_++;
	}
	return this;
}

PubSubServer* PubSubServer::report(ReportHandler_t callback, size_t interval) {
	if (reportersCount_ < PUBSUBSERVER_MAX_REPORTERS) {
		reporters_[reportersCount_].callback = callback;
		reporters_[reportersCount_].interval = interval;
		reporters_[reportersCount_].lastExecution = 0;
		reportersCount_++;
	}
	return this;
}

void PubSubServer::subscribe_() {
	for (int i = 0; i < handlersCount_; i++) {
		client_.subscribe(handlers_[i].topic);
	}
}

void PubSubServer::handleMessage_(char* topic, byte* payload, uint length) {
	for (int i = 0; i < handlersCount_; i++) {
		if (strcmp(handlers_[i].topic, topic) == 0) {
			if (listener_ != nullptr) {
				listener_->onMessage(topic, payload, length);
			}
			handlers_[i].callback(&client_, payload, length);
			return;
		}
	}
}

void PubSubServer::handleReports_() {
	size_t now = millis();
	for (int i = 0; i < reportersCount_; i++) {
		if (now - reporters_[i].lastExecution >= reporters_[i].interval) {
			reporters_[i].lastExecution = now;
			reporters_[i].callback(&client_);
		}
	}
}

void PubSubServer::reportAll_() {
	size_t now = millis();
	for (int i = 0; i < reportersCount_; i++) {
		reporters_[i].callback(&client_);
		reporters_[i].lastExecution = now;
	}
}

bool PubSubServer::keepConnected_() {
	if (client_.connected()) {
		return true;
	} else if (connected_) {
		connected_ = false;
		listener_->onDisconnect();
	}

	randomSeed(micros());
	if (client_.connect(clientID_)) {
		connected_ = true;
		listener_->onConnect();
		reportAll_();
		subscribe_();
		return true;
	}
	return false;
}
