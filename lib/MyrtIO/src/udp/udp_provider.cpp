#include "udp_provider.h"

void UDPRequestProvider::handleMessage(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
  if (p == NULL || p->len < 5) {
    return;
  }
  auto payload = (uint8_t *)p->payload;
  if (payload[0] != IOProtocolCode::HEADER_FIRST || payload[1] != IOProtocolCode::HEADER_SECOND) {
    Serial.println("Invalid header");
    return;
  }
  uint8_t len = payload[2];
  if (len > IO_PROTOCOL_PAYLOAD_MAX_LENGTH) {
    Serial.println("Overflow payload length");
    return;
  }
  if (p->len != len + 4) {
    Serial.println("Invalid payload length");
    return;
  }
  if (payload[len+3] != IOProtocolCode::TAIL) {
    return;
  }
  UDPRequest* request = new UDPRequest(&payload[3], len, pcb, p, addr, port);
  listener_->onRequest(request);
  delete request;
}
