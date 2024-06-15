#pragma once

#include <Arduino.h>
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "../request.h"

/**
 * @class UDPRequest
 * Represents a request received over UDP.
 */
class UDPRequest : public IORequest {
  public:
    UDPRequest(
      uint8_t* payload,
      uint8_t payloadLength,
      struct udp_pcb *pcb,
      struct pbuf *buf,
      const ip_addr_t *addr,
      uint16_t port
    ) : IORequest(payload, payloadLength) {
      _pcb = pcb;
      _buf = buf;
      _addr = addr;
      _port = port;
    }
    void flushMessage(uint8_t* message, uint8_t messageLength) override;

  private:
    struct udp_pcb *_pcb;
    struct pbuf *_buf;
    const ip_addr_t *_addr;
    uint16_t _port;
};
