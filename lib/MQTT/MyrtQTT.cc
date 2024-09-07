#include "MyrtQTT.h"

MyrtQTT::MyrtQTT(PubSubClient* client, const char* clientID) {
  clientID_ = clientID;
  client_ = client;
}

void MyrtQTT::setup(const char *host, uint16_t port) {
  randomSeed(micros());
  client_->setServer(host, port);
}

void MyrtQTT::loop() {
  if (!connected_()) {
    return;
  }
  client_->loop();
  handleReports_();
}

void MyrtQTT::handleMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  for (int i = 0; i < handlersCount_; i++) {
    if (strcmp(handlers_[i].topic, topic) == 0) {
      handlers_[i].callback(client_, payload, length);
    }
  }
}

MyrtQTT* MyrtQTT::on(const char *topic, DataCallback callback) {
  if (handlersCount_ < MQTT_MAX_TOPICS) {
    handlers_[handlersCount_].topic = topic;
    handlers_[handlersCount_].callback = callback;
    handlersCount_++;
  }
  return this;
}

MyrtQTT* MyrtQTT::report(ReportCallback callback, size_t interval) {
  if (reportersCount_ < MQTT_MAX_TOPICS) {
    reporters_[reportersCount_].callback = callback;
    reporters_[reportersCount_].interval = interval;
    reporters_[reportersCount_].lastExecution = 0;
    reportersCount_++;
  }
  return this;
}

MyrtQTT* MyrtQTT::reportConfig(ReportCallback callback) {
  return report(callback, 60000);
}

bool MyrtQTT::connected_() {
  if (client_->connected()) {
    return true;
  }
  if (client_->connect(clientID_)) {
    reportAll_();
    subscribe_();
    return true;
  }
  return false;
}

void MyrtQTT::subscribe_() {
  for (int i = 0; i < handlersCount_; i++) {
    client_->subscribe(handlers_[i].topic);
  }
}

void MyrtQTT::handleReports_() {
  size_t now = millis();
  bool needReport = false;
  for (int i = 0; i < reportersCount_; i++) {
    if (now - reporters_[i].lastExecution >= reporters_[i].interval) {
      needReport = true;
      reporters_[i].lastExecution = now;
      reporters_[i].callback(client_);
    }
  }
}

void MyrtQTT::reportAll_() {
  size_t now = millis();
  for (int i = 0; i < reportersCount_; i++) {
    reporters_[i].callback(client_);
    reporters_[i].lastExecution = now;
  }
}
