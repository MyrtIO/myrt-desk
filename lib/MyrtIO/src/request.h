#pragma once

#include <stdint.h>
#include <string.h>
#include <Arduino.h>

#define IO_PROTOCOL_MESSAGE_MAX_LENGTH 64
#define IO_PROTOCOL_HEADER_LENGTH 3
#define IO_PROTOCOL_PAYLOAD_PADDING IO_PROTOCOL_HEADER_LENGTH + 1
#define IO_PROTOCOL_MESSAGE_META_LENGTH IO_PROTOCOL_HEADER_LENGTH + 1
#define IO_PROTOCOL_PAYLOAD_MAX_LENGTH IO_PROTOCOL_MESSAGE_MAX_LENGTH - IO_PROTOCOL_MESSAGE_META_LENGTH

/**
 * Message structure: [HEADER_FIRST] [HEADER_SECOND] [payload_length] [...payload] [TAIL]
 * `payload_length` - total_length subtract 4 meta bytes.
 * `payload` represents message payload, that will be passed to handler.
 */

enum IOProtocolCode : uint8_t {
  HEADER_FIRST = 0xFE,
  HEADER_SECOND = 0xEF,
  TAIL = 0xAF
};

class IORequest {
  public:
    IORequest(uint8_t* message, uint8_t payloadLength);
    uint8_t length();
    uint8_t* payload();
    IORequest* response();
    IORequest* append(uint8_t value);
    IORequest* append(const char* value);
    bool sent();
    void flush();
    virtual void flushMessage(uint8_t* message, uint8_t messageLength) = 0;

  protected:
    uint8_t message_[IO_PROTOCOL_MESSAGE_MAX_LENGTH];
    uint8_t *payload_;
    uint8_t payloadLength_;

  private:
    bool sent_ = false;
};

class IORequestListener {
  public:
    virtual void onRequest(IORequest* request) = 0;
};

class IORequestProvider {
  public:
    void setListener(IORequestListener* newListener);

  protected:
    IORequestListener* listener_ = nullptr;
};
