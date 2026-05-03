#pragma once

#include "persistent_data.h"
#include "light.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*web_configuration_changed_cb_t)(void);

void web_init(persistent_data_t *pdata,
              light_config_t *light_cfg,
              web_configuration_changed_cb_t on_configuration_changed);
void web_start(void);
void web_loop(void);

#ifdef __cplusplus
}
#endif
