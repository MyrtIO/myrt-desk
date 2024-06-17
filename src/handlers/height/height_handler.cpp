#include "height_handler.h"

uint8_t HeightHandler::code() {
  return FeatureCode::Height;
}

void HeightHandler::setup() {}
HeightHandler::HeightHandler() {}

// Handles action requests and performs actions based on the request.
bool HeightHandler::onAction(IOActionRequest* request) {
  switch (request->action) {
    case HeightAction::SetHeight:
      return handleSetHeight_(request);
    case HeightAction::GetHeight:
      return handleGetHeight_(request);
  }
  return false;
}

// Private method to handle setting height based on the request.
bool HeightHandler::handleSetHeight_(IOActionRequest* request) {
  if (request->length != 2) {
    return false;
  }
  uint16_t height = (request->payload[0] << 8) + request->payload[1];
  return legs_->setHeight(height);
}

// Private method to handle getting height based on the request.
bool HeightHandler::handleGetHeight_(IOActionRequest* request) {
  if (request->length != 0) {
    return false; // Invalid payload length
  }
  uint16_t height = legs_->getHeight();
  request
    ->startReply(true)
    ->append(highByte(height))
    ->append(lowByte(height))
    ->flush();
  return true;
}
