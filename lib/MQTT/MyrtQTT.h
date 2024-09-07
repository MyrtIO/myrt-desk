#pragma once

#include <PubSubClient.h>
#include <MyrtIO.h>

#define MQTT_MAX_TOPICS 20
#define MQTT_CONNECTION_TIMEOUT 5000

typedef void (*DataCallback)(PubSubClient* client, byte* payload, unsigned int length);
typedef void (*ReportCallback)(PubSubClient* client);

struct TopicHandler {
  const char* topic;
  DataCallback callback;
};

struct TopicReporter {
  ReportCallback callback;
  unsigned long interval;
  unsigned long lastExecution = 0;
};

class MyrtQTT: public IOController {
  public:
    MyrtQTT(PubSubClient* client, const char* clientID);

    void setup(const char *host, uint16_t port);
    void loop();
    void handleMessage(char* topic, byte* payload, unsigned int length);

    MyrtQTT* on(const char *topic, DataCallback callback);
    MyrtQTT* report(ReportCallback callback, size_t interval);
    MyrtQTT* reportConfig(ReportCallback callback);

  private:
    const char* clientID_;
    PubSubClient* client_;
    TopicHandler handlers_[MQTT_MAX_TOPICS];
    uint8_t handlersCount_ = 0;
    TopicReporter reporters_[MQTT_MAX_TOPICS];
    uint8_t reportersCount_ = 0;

    bool connected_();
    void subscribe_();
    void handleReports_();
    void reportAll_();
};
