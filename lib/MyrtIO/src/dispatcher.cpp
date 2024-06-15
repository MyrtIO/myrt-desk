#include "dispatcher.h"

IODevice* IODispatcher::setup() {
  Serial.println("IODispatcher::setup");
  device_ = new IODevice();
  return device_;
}

void IODispatcher::onRequest(IORequest* request) {
  uint8_t* payload = request->payload();
  uint8_t length = request->length();
  if (length < 2) {
    return;
  }
  Serial.println("IODispatcher::onRequest");
  for (uint8_t i = 0; i < device_->featuresCount; i++) {
    if (device_->featureList[i]->code() == payload[0]) {
      Serial.println("IODispatcher::onRequest::found");
      runAction_(request, device_->featureList[i]);
      return;
    }
  }
  Serial.println("IODispatcher::onRequest::unknown");
}

void IODispatcher::handle() {
  for (uint8_t i = 0; i < device_->platformsCount; i++) {
    device_->platformList[i]->onLoop();
  }
}

 void IODispatcher::runAction_(IORequest* request, IOFeature* target) {
  IOActionRequest* action = new IOActionRequest(request);
  Serial.println("IODispatcher::runAction_");
  Serial.printf("Action: %d %d\n", action->capability, action->action);
  Serial.printf("Payload: %d\n", action->length);
  bool success = target->onAction(action);
  if (!action->sent()) {
    action->replyStatus(success);
  }
  delete action;
}
