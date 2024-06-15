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
  for (uint8_t i = 0; i < device_->handlersCount; i++) {
    if (device_->handlerList[i]->code() == payload[0]) {
      Serial.println("IODispatcher::onRequest::found");
      runAction_(request, device_->handlerList[i]);
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

 void IODispatcher::runAction_(IORequest* request, IOHandler* target) {
  IOActionRequest* action = new IOActionRequest(request);
  bool success = target->onAction(action);
  if (!action->sent()) {
    action->replyStatus(success);
  }
  delete action;
}
