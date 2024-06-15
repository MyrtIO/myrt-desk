#include "udp_request.h"
#include <stdio.h>

void UDPRequest::flushMessage(uint8_t* message, uint8_t messageLength) {
  struct pbuf *response_pbuf = pbuf_alloc(PBUF_TRANSPORT, messageLength, PBUF_RAM);
  if (response_pbuf != NULL) {
    memcpy(response_pbuf->payload, message, messageLength);
    udp_sendto(_pcb, response_pbuf, _addr, _port);
    pbuf_free(response_pbuf);
  }
}
