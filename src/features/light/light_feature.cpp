#include "light_feature.h"

uint8_t LightFeature::code() {
  return FeatureCode::Light;
}

void LightFeature::setup() {
  // storage_->append(&stateDescriptor_);
  // leds_->setDigitsColor(state_.color[0], state_.color[1], state_.color[2]);
  // leds_->setBrightness(state_.brightness);
  // leds_->setEffect(state_.effect);
}

LightFeature::LightFeature() {}

// Handles action requests and performs actions based on the request.
bool LightFeature::onAction(IOActionRequest* request) {
  switch (request->action) {
    case LightAction::SetColor:
      return handleSetColor_(request);
    case LightAction::SetBrightness:
      return handleSetBrightness_(request);
    // case LightAction::GetColor:
    //   return handleGetColor_(request);
    // case LightAction::GetBrightness:
    //   return handleGetBrightness_(request);
    // case LightAction::SetPower:
    //   return handleSetPower_(request);
    // case LightAction::GetPower:
    //   return handleGetPower_(request);
    // case LightAction::SetEffect:
    //   return handleSetEffect_(request);
    // case LightAction::GetEffect:
    //   return handleGetEffect_(request);
  }
  return false; // Unknown action
}

// Private method to handle setting LED color based on the request.
bool LightFeature::handleSetColor_(IOActionRequest* request) {
  Serial.println("handleSetColor_");
  if (request->length != 3) {
    Serial.println("handleSetColor_::invalid length");
    return false; // Invalid payload length
  }
  // request->payload[0], request->payload[1], request->payload[2]
  leds_->setColor(CRGB(request->payload[0], request->payload[1], request->payload[2]));
  // copyColor(request->payload, state_.color);
  // stateDescriptor_.update();
  return true;
}

// Private method to handle setting LED brightness based on the request.
bool LightFeature::handleSetBrightness_(IOActionRequest* request) {
  if (request->length != 1) {
    return false; // Invalid payload length
  }
  leds_->setBrightness(request->payload[0]);
  // state_.brightness = request->payload[0];
  // stateDescriptor_.update();
  return true; // Successfully set brightness
}

// bool LightFeature::handleGetColor_(IOActionRequest* request) {
//   if (request->length != 0) {
//     return false;
//   }
//   request
//     ->startReply(true)
//     ->append(state_.color[0])
//     ->append(state_.color[1])
//     ->append(state_.color[2])
//     ->flush();
//   return true;
// }

// bool LightFeature::handleGetBrightness_(IOActionRequest* request) {
//   if (request->length != 0) {
//     return false;
//   }
//   request
//     ->startReply(true)
//     ->append(state_.brightness)
//     ->flush();
//   return true;
// }

// bool LightFeature::handleSetPower_(IOActionRequest* request) {
//   if (request->length != 1 || request->payload[0] > 1) {
//     return false;
//   }
//   leds_->setPower(request->payload[0]);
//   return true;
// }

// bool LightFeature::handleGetPower_(IOActionRequest* request) {
//   if (request->length != 0) {
//     return false;
//   }
//   request
//     ->startReply(true)
//     ->append(leds_->getPower() ? 1 : 0)
//     ->flush();
//   return true;
// }

// bool LightFeature::handleGetEffect_(IOActionRequest* request) {
//   if (request->length != 0) {
//     return false;
//   }
//   request
//     ->startReply(true)
//     ->append(state_.effect)
//     ->flush();
//   return true;
// }

// bool LightFeature::handleSetEffect_(IOActionRequest* request) {
//   if (request->length != 1) {
//     return false;
//   }
//   state_.effect = request->payload[0];
//   leds_->setEffect(request->payload[0]);
//   return true;
// }
