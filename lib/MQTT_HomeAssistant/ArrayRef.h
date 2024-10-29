#pragma once

#include <stdint.h>

class ArrayRef {
public:
	template <uint16_t N>
	ArrayRef(
		const char* names[N]
	): data(data) {}

	char* data;
	// uint16_t size;
};
