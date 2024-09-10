#pragma once

#include <stdint.h>

#define median(arr, size) \
    calcMedianWirth(arr, size, (((size)&1) ? ((size) / 2) : (((size) / 2) - 1)))

uint32_t calcMedianWirth(uint32_t arr[], uint8_t size, uint8_t k);
