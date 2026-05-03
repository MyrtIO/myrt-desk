#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct persistent_data_t {
    char wifi_ssid[33];
    char wifi_password[65];
    char mqtt_host[65];
    uint16_t mqtt_port;
    char mqtt_username[33];
    char mqtt_password[65];
    uint32_t color_correction;
} persistent_data_t;

typedef struct light_saved_state_t {
    bool power;
    uint8_t brightness;
    uint8_t r, g, b;
    uint16_t color_temp;
    uint8_t color_mode;
    char effect[16];
} light_saved_state_t;

typedef struct desk_calibration_data_t {
    uint16_t up_correction_mm;
    uint16_t down_correction_mm;
} desk_calibration_data_t;

void persistent_data_load(persistent_data_t *data);
void persistent_data_save(const persistent_data_t *data);
bool persistent_data_is_configured(const persistent_data_t *data);
uint8_t power_cycle_count_increment(void);
void power_cycle_count_reset(void);

bool light_state_exists(void);
void light_state_load(light_saved_state_t *state);
void light_state_save(const light_saved_state_t *state);

bool desk_calibration_exists(void);
void desk_calibration_load(desk_calibration_data_t *data);
void desk_calibration_save(const desk_calibration_data_t *data);

#ifdef __cplusplus
}
#endif
