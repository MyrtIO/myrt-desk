#pragma once

#include <Attotime.h>
#include <MyrtIO.h>
#include <PubSubClient.h>

#define MYRTQTT_MAX_TOPICS 20
#define MYRTQTT_CONNECTION_TIMEOUT 5000
#define MYRTQTT_BUFFER_SIZE 1024

typedef void (*DataCallback)(PubSubClient* client, byte* payload, unsigned int length);
typedef void (*ReportCallback)(PubSubClient* client);

struct TopicHandler {
    const char* topic = nullptr;
    DataCallback callback = nullptr;
};

struct TopicReporter {
    ReportCallback callback = nullptr;
    unsigned long interval = 0;
    unsigned long lastExecution = 0;
};

class MyrtQTT {
  public:
    MyrtQTT(PubSubClient* client, const char* clientID);
    ~MyrtQTT();

    void setBufferSize(size_t size);
    void loop();
    void handleMessage(char* topic, byte* payload, unsigned int length);
    void setServer(char* host, uint16_t port);
    void onConnect();

    MyrtQTT* on(const char* topic, DataCallback callback);
    MyrtQTT* report(ReportCallback callback, size_t interval);
    MyrtQTT* reportConfig(ReportCallback callback);

  private:
    PubSubClient* client_ = nullptr;
    const char* clientID_ = nullptr;
    TopicHandler handlers_[MYRTQTT_MAX_TOPICS];
    uint8_t handlersCount_ = 0;
    TopicReporter reporters_[MYRTQTT_MAX_TOPICS];
    uint8_t reportersCount_ = 0;

    void subscribe_();
    void handleReports_();
    void reportAll_();
};
