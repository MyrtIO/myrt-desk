#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bekant_desk_config_t {
    uint8_t pin_lin_tx;
    uint8_t pin_lin_rx;
    uint8_t pin_button_up;
    uint8_t pin_button_down;
    uint16_t height_min_mm;
    uint16_t height_max_mm;
    float height_slope;
    float height_bias;
} bekant_desk_config_t;

typedef enum bekant_desk_calibration_phase_t {
    BEKANT_DESK_CALIBRATION_IDLE = 0,
    BEKANT_DESK_CALIBRATION_MOVE_TO_CENTER,
    BEKANT_DESK_CALIBRATION_MOVE_SAMPLE,
    BEKANT_DESK_CALIBRATION_SETTLING,
    BEKANT_DESK_CALIBRATION_COMPLETE,
    BEKANT_DESK_CALIBRATION_ERROR,
} bekant_desk_calibration_phase_t;

typedef struct bekant_desk_calibration_status_t {
    bool running;
    bool has_calibration;
    bekant_desk_calibration_phase_t phase;
    uint8_t sample_index;
    uint8_t sample_total;
    uint16_t up_correction_mm;
    uint16_t down_correction_mm;
    int16_t last_error_mm;
} bekant_desk_calibration_status_t;

bool bekant_desk_init(const bekant_desk_config_t *cfg);
void bekant_desk_loop(void);
bool bekant_desk_has_height(void);
uint16_t bekant_desk_get_height_mm(void);
bool bekant_desk_set_height_mm(uint16_t height_mm);
bool bekant_desk_start_calibration(void);
bool bekant_desk_is_calibrating(void);
bool bekant_desk_get_calibration_status(bekant_desk_calibration_status_t *status);
const char *bekant_desk_calibration_phase_name(
    bekant_desk_calibration_phase_t phase);

#ifdef __cplusplus
}
#endif
