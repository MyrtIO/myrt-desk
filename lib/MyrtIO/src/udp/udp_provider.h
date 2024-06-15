#pragma once

#include <Arduino.h>
#include "../request.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "udp_request.h"

/**
 * @class UDPRequestProvider
 * Provides functionalities to handle and process UDP requests.
 */
class UDPRequestProvider : public IORequestProvider {
public:
    UDPRequestProvider() {

    }

    void handleMessage(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

private:
    uint8_t message_[IO_PROTOCOL_MESSAGE_MAX_LENGTH];
};

