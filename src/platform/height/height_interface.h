#include <stdint.h>

class IOHeight {
  public:
	virtual uint16_t get() = 0;
	virtual bool set(uint16_t height) = 0;
};
