#pragma once

#include <lc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

void light_hal_set_pixel(uint16_t i, rgb_t c);

rgb_t light_hal_get_pixel(uint16_t i);

void light_hal_show(void);

void light_hal_init(void);

#ifdef __cplusplus
}
#endif
