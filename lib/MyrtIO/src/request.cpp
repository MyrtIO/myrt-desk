#include "request.h"

IORequest::IORequest(uint8_t* message, uint8_t payloadLength) {
  uint8_t* payload = &message_[IO_PROTOCOL_PAYLOAD_PADDING];
  memcpy(payload, message, sizeof(uint8_t) * payloadLength);
  payloadLength_ = payloadLength;
  payload_ = payload;
}

uint8_t IORequest::length() {
  return payloadLength_;
}

bool IORequest::sent() {
  return sent_;
}

uint8_t* IORequest::payload() {
  return payload_;
}

IORequest* IORequest::response() {
  payloadLength_ = 0;
  return this;
}

IORequest* IORequest::append(uint8_t value) {
  message_[IO_PROTOCOL_HEADER_LENGTH + payloadLength_] = value;
  payloadLength_ += 1;
  return this;
}

IORequest* IORequest::append(const char* value) {
  uint8_t length = strlen(value);
  for (uint8_t i = 0; i < length; i++) {
    append(value[i]);
  }
  return this;
}

void IORequest::flush() {
  uint8_t messageLength = IO_PROTOCOL_MESSAGE_META_LENGTH + payloadLength_;
  message_[0] = IOProtocolCode::HEADER_FIRST;
  message_[1] = IOProtocolCode::HEADER_SECOND;
  message_[2] = payloadLength_;
  message_[messageLength-1] = IOProtocolCode::TAIL;
  flushMessage(&message_[0], messageLength);
  sent_ = true;
}

void IORequestProvider::setListener(IORequestListener* newListener) {
  listener_ = newListener;
}
