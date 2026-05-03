#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <lc_types.h>

#include "persistent_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum light_cmd_type_t {
    LIGHT_CMD_POWER,
    LIGHT_CMD_BRIGHTNESS,
    LIGHT_CMD_COLOR,
    LIGHT_CMD_COLOR_TEMP,
    LIGHT_CMD_EFFECT,
} light_cmd_type_t;

typedef enum light_color_mode_t {
    LIGHT_MODE_RGB = 0,
    LIGHT_MODE_WHITE = 1,
} light_color_mode_t;

typedef struct light_cmd_t {
    light_cmd_type_t type;
    union {
        bool power;
        uint8_t brightness;
        rgb_t color;
        uint16_t color_temp;
        const char *effect_name;
    };
} light_cmd_t;

typedef struct light_state_t {
    bool power;
    uint8_t brightness;
    rgb_t color;
    uint16_t color_temp;
    const char *effect;
    light_color_mode_t color_mode;
} light_state_t;

typedef struct light_config_t {
    uint8_t pin;
    uint16_t led_count;
    uint16_t led_skip;
    uint32_t color_correction;
    uint8_t color_order;
    uint16_t kelvin_warm;
    uint16_t kelvin_cold;
    uint16_t kelvin_initial;
    uint16_t transition_ms;
    uint8_t brightness;
    uint8_t brightness_max;
} light_config_t;

void light_init(light_config_t *cfg);
void light_start(void);
void light_update_config(light_config_t *cfg);
void light_restore_state(light_saved_state_t *state);
void light_send_cmd(light_cmd_t *cmd);
light_state_t light_get_state(void);

const char **light_effect_names(void);
uint8_t light_effect_count(void);

#ifdef __cplusplus
}
#endif
