#pragma once

#include "myrt_qtt.h"
#include <MyrtIO.h>
#include <WiFi.h>
#include <platform.h>

typedef void (*TopicRegisterer)(MyrtQTT* mqtt);

class MQTTController_ : public IOUnit {
  public:
    void setup();
    void loop();
    const char* name();
    void handleMessage(char* topic, byte* payload, unsigned int length);

  private:
    WiFiClient wifiClient_ = WiFiClient();
    PubSubClient client_ = PubSubClient(wifiClient_);
    MyrtQTT mqtt_ = MyrtQTT(&client_, CONFIG_DEVICE_NAME);
    IWiFiPlatform* wifi_ = IO_INJECT(IWiFiPlatform);
    bool connected_();
};

extern MQTTController_ MQTTController;
