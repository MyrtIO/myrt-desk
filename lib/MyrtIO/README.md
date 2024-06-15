# IO Protocol

The IO Protocol is designed for communication in embedded systems. It provides a structured way to send and receive messages with a defined header, payload, and tail.

## Message Structure

The message in the IO Protocol has the following structure:

- `Header` - Two-byte header consisting of HEADER_FIRST and HEADER_SECOND.
- `Payload Length` - A single byte indicating the length of the payload.
- `Payload` - Variable length, containing the actual message data.
- `Tail` - A single byte indicating the end of the message.

Protocol codes can be found in [`protocol.h`](./protocol.h) file.

## Implementations

- [Serial](./serial) - protocol implementation using Serial connection. Suitable for connecting a device to a computer or devices to each other.
