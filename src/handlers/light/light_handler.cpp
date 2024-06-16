#include "light_handler.h"

uint8_t LightHandler::code() {
  return FeatureCode::Light;
}

void LightHandler::setup() {
  // storage_->append(&stateDescriptor_);
  // leds_->setDigitsColor(state_.color[0], state_.color[1], state_.color[2]);
  // leds_->setBrightness(state_.brightness);
  // leds_->setEffect(state_.effect);
}

LightHandler::LightHandler() {}

// Handles action requests and performs actions based on the request.
bool LightHandler::onAction(IOActionRequest* request) {
  switch (request->action) {
    case LightAction::SetColor:
      return handleSetColor_(request);
    case LightAction::GetColor:
      return handleGetColor_(request);
    case LightAction::SetBrightness:
      return handleSetBrightness_(request);
    case LightAction::GetBrightness:
      return handleGetBrightness_(request);
    case LightAction::SetPower:
      return handleSetPower_(request);
    case LightAction::GetPower:
      return handleGetPower_(request);
    // case LightAction::SetEffect:
    //   return handleSetEffect_(request);
    // case LightAction::GetEffect:
    //   return handleGetEffect_(request);
  }
  return false; // Unknown action
}

// Private method to handle setting LED color based on the request.
bool LightHandler::handleSetColor_(IOActionRequest* request) {
  if (request->length != 3) {
    return false; // Invalid payload length
  }
  leds_->setColor(CRGB(request->payload[0], request->payload[1], request->payload[2]));
  return true;
}

// Private method to handle setting LED brightness based on the request.
bool LightHandler::handleSetBrightness_(IOActionRequest* request) {
  if (request->length != 1) {
    return false; // Invalid payload length
  }
  leds_->setBrightness(request->payload[0]);
  // state_.brightness = request->payload[0];
  // stateDescriptor_.update();
  return true; // Successfully set brightness
}

bool LightHandler::handleGetColor_(IOActionRequest* request) {
  if (request->length != 0) {
    return false;
  }
  CRGB color = leds_->getColor();
  request
    ->startReply(true)
    ->append(color.r)
    ->append(color.g)
    ->append(color.b)
    ->flush();
  return true;
}

bool LightHandler::handleGetBrightness_(IOActionRequest* request) {
  if (request->length != 0) {
    return false;
  }
  request
    ->startReply(true)
    ->append(leds_->getBrightness())
    ->flush();
  return true;
}

bool LightHandler::handleSetPower_(IOActionRequest* request) {
  if (request->length != 1 || request->payload[0] > 1) {
    return false;
  }
  leds_->setPower(request->payload[0]);
  return true;
}

bool LightHandler::handleGetPower_(IOActionRequest* request) {
  if (request->length != 0) {
    return false;
  }
  request
    ->startReply(true)
    ->append(leds_->getPower())
    ->flush();
  return true;
}

// bool LightHandler::handleGetEffect_(IOActionRequest* request) {
//   if (request->length != 0) {
//     return false;
//   }
//   request
//     ->startReply(true)
//     ->append(state_.effect)
//     ->flush();
//   return true;
// }

// bool LightHandler::handleSetEffect_(IOActionRequest* request) {
//   if (request->length != 1) {
//     return false;
//   }
//   state_.effect = request->payload[0];
//   leds_->setEffect(request->payload[0]);
//   return true;
// }
