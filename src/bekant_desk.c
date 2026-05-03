#include "bekant_desk.h"

#include <math.h>
#include <string.h>

#include <app_log.h>
#include <attotime.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>

#include "persistent_data.h"

#define BEKANT_LOG_TAG "Desk"
#define BEKANT_LIN_SYNC 0x55u
#define BEKANT_LIN_POSITION_PID 0x92u
#define BEKANT_LIN_BAUD_RATE 19230u
#define BEKANT_POSITION_MIN 170u
#define BEKANT_POSITION_MAX 6400u
#define BEKANT_FRAME_SIZE 5u
#define BEKANT_LIN_TIMEOUT_MS 2000u
#define BEKANT_SETTLE_TIME_MS 500u
#define BEKANT_CALIBRATION_CENTER_MM 800u
#define BEKANT_CALIBRATION_SAMPLE_COUNT 8u
#define BEKANT_FOLLOWUP_BUFFER_MM 5u

typedef enum bekant_motion_state_t {
    BEKANT_MOTION_IDLE = 0,
    BEKANT_MOTION_UP,
    BEKANT_MOTION_DOWN,
} bekant_motion_state_t;

typedef enum bekant_calibration_state_t {
    BEKANT_CAL_STATE_IDLE = 0,
    BEKANT_CAL_STATE_MOVE_TO_CENTER,
    BEKANT_CAL_STATE_MOVE_SAMPLE,
    BEKANT_CAL_STATE_COMPLETE,
    BEKANT_CAL_STATE_ERROR,
} bekant_calibration_state_t;

typedef struct bekant_saved_calibration_t {
    bool valid;
    uint16_t up_correction_mm;
    uint16_t down_correction_mm;
} bekant_saved_calibration_t;

typedef struct bekant_followup_move_t {
    bool active;
    uint16_t final_target_mm;
} bekant_followup_move_t;

typedef struct bekant_calibration_runtime_t {
    bekant_calibration_state_t state;
    uint8_t current_sample_index;
    uint8_t completed_samples;
    uint16_t current_target_mm;
    bekant_motion_state_t current_direction;
    int16_t last_error_mm;
    int32_t up_error_sum_mm;
    int32_t down_error_sum_mm;
    uint8_t up_sample_count;
    uint8_t down_sample_count;
} bekant_calibration_runtime_t;

static const uint16_t s_calibration_targets[BEKANT_CALIBRATION_SAMPLE_COUNT] = {
    795u, 800u, 790u, 800u, 780u, 800u, 770u, 800u,
};

static bekant_desk_config_t s_cfg;
static bool s_initialized = false;
static bool s_has_height = false;
static uint16_t s_height_mm = 0;
static uint16_t s_target_height_mm = 0;
static uint32_t s_last_frame_at = 0;
static uint32_t s_last_height_change_at = 0;
static bekant_motion_state_t s_motion_state = BEKANT_MOTION_IDLE;
static bekant_saved_calibration_t s_saved_calibration;
static bekant_followup_move_t s_followup_move;
static bekant_calibration_runtime_t s_calibration;
static uint8_t s_frame[BEKANT_FRAME_SIZE];
static uint8_t s_frame_head = 0;
static uart_inst_t *s_uart = NULL;

static bool bekant_start_height_request(uint16_t height_mm,
                                        bool use_compensation,
                                        bool allow_staged);
static void bekant_fail_calibration(const char *message);

static bool bekant_uart_is_uart0_pair(uint8_t tx_pin, uint8_t rx_pin) {
    return (tx_pin == 0u || tx_pin == 12u || tx_pin == 16u || tx_pin == 28u) &&
           (rx_pin == 1u || rx_pin == 13u || rx_pin == 17u || rx_pin == 29u);
}

static bool bekant_uart_is_uart1_pair(uint8_t tx_pin, uint8_t rx_pin) {
    return (tx_pin == 4u || tx_pin == 8u || tx_pin == 20u || tx_pin == 24u) &&
           (rx_pin == 5u || rx_pin == 9u || rx_pin == 21u || rx_pin == 25u);
}

static bool bekant_uart_init(uint8_t tx_pin, uint8_t rx_pin) {
    uart_inst_t *uart = NULL;

    if (bekant_uart_is_uart0_pair(tx_pin, rx_pin)) {
        uart = uart0;
    } else if (bekant_uart_is_uart1_pair(tx_pin, rx_pin)) {
        uart = uart1;
    } else {
        return false;
    }

    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);

    uart_init(uart, BEKANT_LIN_BAUD_RATE);
    uart_set_format(uart, 8u, 1u, UART_PARITY_NONE);
    uart_set_hw_flow(uart, false, false);
    uart_set_fifo_enabled(uart, true);

    s_uart = uart;
    return true;
}

static int bekant_uart_available(void) {
    return (s_uart != NULL && uart_is_readable(s_uart)) ? 1 : 0;
}

static int bekant_uart_read(void) {
    if (s_uart == NULL || !uart_is_readable(s_uart)) {
        return -1;
    }

    return (int)uart_getc(s_uart);
}

static bool bekant_calibration_is_running(void) {
    return s_calibration.state == BEKANT_CAL_STATE_MOVE_TO_CENTER ||
           s_calibration.state == BEKANT_CAL_STATE_MOVE_SAMPLE;
}

static void bekant_reset_followup_move(void) {
    s_followup_move.active = false;
    s_followup_move.final_target_mm = 0;
}

static void bekant_reset_calibration_runtime(void) {
    memset(&s_calibration, 0, sizeof(s_calibration));
}

static bool bekant_height_is_in_range(uint16_t height_mm) {
    return height_mm >= s_cfg.height_min_mm && height_mm <= s_cfg.height_max_mm;
}

static uint16_t bekant_clamp_height(int32_t height_mm) {
    if (height_mm < (int32_t)s_cfg.height_min_mm) {
        return s_cfg.height_min_mm;
    }
    if (height_mm > (int32_t)s_cfg.height_max_mm) {
        return s_cfg.height_max_mm;
    }
    return (uint16_t)height_mm;
}

static bool bekant_height_is_settled(uint32_t now) {
    if (!s_has_height) {
        return false;
    }

    return (uint32_t)(now - s_last_height_change_at) >= BEKANT_SETTLE_TIME_MS;
}

static void bekant_load_saved_calibration(void) {
    memset(&s_saved_calibration, 0, sizeof(s_saved_calibration));
    if (!desk_calibration_exists()) {
        return;
    }

    desk_calibration_data_t data;
    desk_calibration_load(&data);
    s_saved_calibration.valid = true;
    s_saved_calibration.up_correction_mm = data.up_correction_mm;
    s_saved_calibration.down_correction_mm = data.down_correction_mm;
}

static void bekant_store_saved_calibration(uint16_t up_correction_mm,
                                           uint16_t down_correction_mm) {
    desk_calibration_data_t data = {
        .up_correction_mm = up_correction_mm,
        .down_correction_mm = down_correction_mm,
    };

    desk_calibration_save(&data);
    s_saved_calibration.valid = true;
    s_saved_calibration.up_correction_mm = up_correction_mm;
    s_saved_calibration.down_correction_mm = down_correction_mm;
}

static bool bekant_calibration_targets_are_valid(void) {
    if (!bekant_height_is_in_range(BEKANT_CALIBRATION_CENTER_MM)) {
        return false;
    }

    for (uint8_t i = 0; i < BEKANT_CALIBRATION_SAMPLE_COUNT; i++) {
        if (!bekant_height_is_in_range(s_calibration_targets[i])) {
            return false;
        }
    }

    return true;
}

static void bekant_drive_buttons(bekant_motion_state_t state) {
    switch (state) {
        case BEKANT_MOTION_UP:
            gpio_put(s_cfg.pin_button_up, 1);
            gpio_put(s_cfg.pin_button_down, 0);
            break;
        case BEKANT_MOTION_DOWN:
            gpio_put(s_cfg.pin_button_up, 0);
            gpio_put(s_cfg.pin_button_down, 1);
            break;
        case BEKANT_MOTION_IDLE:
        default:
            gpio_put(s_cfg.pin_button_up, 0);
            gpio_put(s_cfg.pin_button_down, 0);
            break;
    }
}

static void bekant_stop_motion(void) {
    s_motion_state = BEKANT_MOTION_IDLE;
    if (s_has_height) {
        s_target_height_mm = s_height_mm;
    }
    bekant_drive_buttons(BEKANT_MOTION_IDLE);
}

static bool bekant_position_is_valid(uint16_t position) {
    return position >= BEKANT_POSITION_MIN && position <= BEKANT_POSITION_MAX;
}

static uint16_t bekant_position_to_height(uint16_t position) {
    float height = (s_cfg.height_slope * (float)position) + s_cfg.height_bias;
    return (uint16_t)lroundf(height);
}

static void bekant_clear_frame(void) {
    s_frame_head = 0;
}

static bool bekant_start_raw_motion(uint16_t target_mm) {
    if (!s_initialized || !bekant_height_is_in_range(target_mm)) {
        return false;
    }
    if (s_motion_state != BEKANT_MOTION_IDLE) {
        return false;
    }

    uint16_t current_height = s_has_height ? s_height_mm : 0;
    s_target_height_mm = target_mm;
    s_last_frame_at = atto_now();

    if (s_has_height && target_mm == current_height) {
        return true;
    }

    if (target_mm > current_height) {
        s_motion_state = BEKANT_MOTION_UP;
        bekant_drive_buttons(BEKANT_MOTION_UP);
    } else {
        s_motion_state = BEKANT_MOTION_DOWN;
        bekant_drive_buttons(BEKANT_MOTION_DOWN);
    }

    return true;
}

static uint16_t bekant_compensated_target(uint16_t requested_height_mm,
                                          bekant_motion_state_t direction) {
    int32_t target_mm = (int32_t)requested_height_mm;

    if (direction == BEKANT_MOTION_UP) {
        target_mm -= (int32_t)s_saved_calibration.up_correction_mm;
    } else if (direction == BEKANT_MOTION_DOWN) {
        target_mm += (int32_t)s_saved_calibration.down_correction_mm;
    }

    return bekant_clamp_height(target_mm);
}

static bool bekant_should_stage_move(uint16_t requested_height_mm,
                                     uint16_t current_height_mm,
                                     bekant_motion_state_t direction) {
    if (!s_saved_calibration.valid) {
        return false;
    }

    if (direction == BEKANT_MOTION_UP) {
        return requested_height_mm <=
               current_height_mm + s_saved_calibration.up_correction_mm;
    }

    if (direction == BEKANT_MOTION_DOWN) {
        return current_height_mm <=
               requested_height_mm + s_saved_calibration.down_correction_mm;
    }

    return false;
}

static bool bekant_try_stage_move(uint16_t requested_height_mm,
                                  bekant_motion_state_t direction) {
    int32_t intermediate_height_mm = (int32_t)requested_height_mm;

    if (direction == BEKANT_MOTION_UP) {
        intermediate_height_mm += (int32_t)s_saved_calibration.down_correction_mm +
                                  BEKANT_FOLLOWUP_BUFFER_MM;
    } else if (direction == BEKANT_MOTION_DOWN) {
        intermediate_height_mm -= (int32_t)s_saved_calibration.up_correction_mm +
                                  BEKANT_FOLLOWUP_BUFFER_MM;
    } else {
        return false;
    }

    if (intermediate_height_mm < (int32_t)s_cfg.height_min_mm ||
        intermediate_height_mm > (int32_t)s_cfg.height_max_mm) {
        return false;
    }

    if (!bekant_start_height_request((uint16_t)intermediate_height_mm, true,
                                     false)) {
        return false;
    }

    s_followup_move.active = true;
    s_followup_move.final_target_mm = requested_height_mm;
    return true;
}

static bool bekant_start_height_request(uint16_t height_mm,
                                        bool use_compensation,
                                        bool allow_staged) {
    if (!s_initialized || !bekant_height_is_in_range(height_mm)) {
        return false;
    }
    if (s_motion_state != BEKANT_MOTION_IDLE) {
        return false;
    }

    uint16_t current_height = s_has_height ? s_height_mm : 0;
    if (s_has_height && height_mm == current_height) {
        return true;
    }

    if (!use_compensation || !s_saved_calibration.valid || !s_has_height) {
        return bekant_start_raw_motion(height_mm);
    }

    bekant_motion_state_t direction =
        height_mm > current_height ? BEKANT_MOTION_UP : BEKANT_MOTION_DOWN;

    if (allow_staged &&
        bekant_should_stage_move(height_mm, current_height, direction)) {
        if (bekant_try_stage_move(height_mm, direction)) {
            return true;
        }

        app_log_printf(BEKANT_LOG_TAG,
                       "small move fallback raw target=%u current=%u direction=%u",
                       (unsigned)height_mm, (unsigned)current_height,
                       (unsigned)direction);
        return bekant_start_raw_motion(height_mm);
    }

    uint16_t internal_target = bekant_compensated_target(height_mm, direction);
    if ((direction == BEKANT_MOTION_UP && internal_target <= current_height) ||
        (direction == BEKANT_MOTION_DOWN && internal_target >= current_height)) {
        app_log_printf(BEKANT_LOG_TAG,
                       "compensation fallback raw target=%u current=%u internal=%u "
                       "direction=%u",
                       (unsigned)height_mm, (unsigned)current_height,
                       (unsigned)internal_target, (unsigned)direction);
        return bekant_start_raw_motion(height_mm);
    }

    return bekant_start_raw_motion(internal_target);
}

static bekant_motion_state_t bekant_calibration_direction_for_sample(
    uint8_t sample_index) {
    return (sample_index & 1u) == 0u ? BEKANT_MOTION_DOWN : BEKANT_MOTION_UP;
}

static bool bekant_start_calibration_target(uint16_t target_mm,
                                            bekant_motion_state_t direction) {
    s_calibration.current_target_mm = target_mm;
    s_calibration.current_direction = direction;
    return bekant_start_raw_motion(target_mm);
}

static bool bekant_start_next_calibration_sample(uint8_t sample_index) {
    if (sample_index >= BEKANT_CALIBRATION_SAMPLE_COUNT) {
        return false;
    }

    s_calibration.current_sample_index = sample_index;
    return bekant_start_calibration_target(
        s_calibration_targets[sample_index],
        bekant_calibration_direction_for_sample(sample_index));
}

static int32_t bekant_div_round_nearest(int32_t sum, uint8_t count) {
    if (count == 0u) {
        return 0;
    }

    if (sum >= 0) {
        return (sum + (int32_t)(count / 2u)) / (int32_t)count;
    }

    return (sum - (int32_t)(count / 2u)) / (int32_t)count;
}

static void bekant_finish_calibration(void) {
    if (s_calibration.up_sample_count == 0u ||
        s_calibration.down_sample_count == 0u) {
        bekant_fail_calibration("calibration failed: incomplete sample set");
        return;
    }

    int32_t up_average_mm = bekant_div_round_nearest(s_calibration.up_error_sum_mm,
                                                     s_calibration.up_sample_count);
    int32_t down_average_mm = bekant_div_round_nearest(
        s_calibration.down_error_sum_mm, s_calibration.down_sample_count);

    if (up_average_mm < 0) {
        up_average_mm = 0;
    }
    if (down_average_mm > 0) {
        down_average_mm = 0;
    }

    bekant_store_saved_calibration((uint16_t)up_average_mm,
                                   (uint16_t)(-down_average_mm));

    s_calibration.state = BEKANT_CAL_STATE_COMPLETE;
    s_calibration.completed_samples = BEKANT_CALIBRATION_SAMPLE_COUNT;
    app_log_printf(BEKANT_LOG_TAG, "calibration saved up=%u down=%u",
                   (unsigned)s_saved_calibration.up_correction_mm,
                   (unsigned)s_saved_calibration.down_correction_mm);
}

static void bekant_record_calibration_sample(void) {
    int16_t signed_error_mm =
        (int16_t)((int32_t)s_height_mm - (int32_t)s_calibration.current_target_mm);

    s_calibration.last_error_mm = signed_error_mm;

    if (s_calibration.current_direction == BEKANT_MOTION_UP) {
        s_calibration.up_error_sum_mm += signed_error_mm;
        s_calibration.up_sample_count++;
    } else if (s_calibration.current_direction == BEKANT_MOTION_DOWN) {
        s_calibration.down_error_sum_mm += signed_error_mm;
        s_calibration.down_sample_count++;
    }

    s_calibration.completed_samples++;
    app_log_printf(BEKANT_LOG_TAG,
                   "calibration sample=%u target=%u actual=%u error=%d",
                   (unsigned)(s_calibration.current_sample_index + 1u),
                   (unsigned)s_calibration.current_target_mm, (unsigned)s_height_mm,
                   (int)signed_error_mm);
}

static void bekant_fail_calibration(const char *message) {
    if (bekant_calibration_is_running()) {
        bekant_stop_motion();
    }

    s_calibration.state = BEKANT_CAL_STATE_ERROR;
    bekant_reset_followup_move();
    app_log_print(BEKANT_LOG_TAG, message);
}

static void bekant_process_calibration(uint32_t now) {
    if (!bekant_calibration_is_running()) {
        return;
    }
    if (!s_has_height) {
        bekant_fail_calibration("calibration failed: no height");
        return;
    }
    if (s_motion_state != BEKANT_MOTION_IDLE) {
        return;
    }
    if (!bekant_height_is_settled(now)) {
        return;
    }

    if (s_calibration.state == BEKANT_CAL_STATE_MOVE_TO_CENTER) {
        s_calibration.state = BEKANT_CAL_STATE_MOVE_SAMPLE;
        if (!bekant_start_next_calibration_sample(0u)) {
            bekant_fail_calibration("calibration failed: first sample start");
        }
        return;
    }

    if (s_calibration.state != BEKANT_CAL_STATE_MOVE_SAMPLE) {
        return;
    }

    bekant_record_calibration_sample();
    if (s_calibration.completed_samples >= BEKANT_CALIBRATION_SAMPLE_COUNT) {
        bekant_finish_calibration();
        return;
    }

    if (!bekant_start_next_calibration_sample(s_calibration.completed_samples)) {
        bekant_fail_calibration("calibration failed: next sample start");
    }
}

static void bekant_process_followup_move(uint32_t now) {
    if (!s_followup_move.active || s_motion_state != BEKANT_MOTION_IDLE ||
        !s_has_height) {
        return;
    }
    if (!bekant_height_is_settled(now)) {
        return;
    }

    uint16_t final_target_mm = s_followup_move.final_target_mm;
    bekant_reset_followup_move();
    if (!bekant_start_height_request(final_target_mm, true, false)) {
        app_log_printf(BEKANT_LOG_TAG, "followup move failed target=%u current=%u",
                       (unsigned)final_target_mm, (unsigned)s_height_mm);
    }
}

static void bekant_handle_position_frame(void) {
    if (s_frame[1] != BEKANT_LIN_POSITION_PID) {
        return;
    }

    uint16_t position = (uint16_t)s_frame[2];
    position |= (uint16_t)s_frame[3] << 8;
    if (!bekant_position_is_valid(position)) {
        return;
    }

    uint16_t next_height_mm = bekant_position_to_height(position);
    uint32_t now = atto_now();

    if (!s_has_height || next_height_mm != s_height_mm) {
        s_last_height_change_at = now;
    }

    s_height_mm = next_height_mm;
    s_has_height = true;
    s_last_frame_at = now;

    if (s_motion_state == BEKANT_MOTION_UP && s_height_mm >= s_target_height_mm) {
        bekant_stop_motion();
    } else if (s_motion_state == BEKANT_MOTION_DOWN &&
               s_height_mm <= s_target_height_mm) {
        bekant_stop_motion();
    }
}

static void bekant_feed_byte(uint8_t byte) {
    if (byte == BEKANT_LIN_SYNC) {
        s_frame[0] = byte;
        s_frame_head = 1;
        return;
    }

    if (s_frame_head == 0 || s_frame_head >= BEKANT_FRAME_SIZE) {
        return;
    }

    s_frame[s_frame_head++] = byte;
    if (s_frame_head == BEKANT_FRAME_SIZE) {
        bekant_handle_position_frame();
        bekant_clear_frame();
    }
}

bool bekant_desk_init(const bekant_desk_config_t *cfg) {
    if (cfg == NULL || cfg->height_min_mm >= cfg->height_max_mm) {
        return false;
    }
    if (cfg->pin_button_up == cfg->pin_button_down) {
        return false;
    }

    if (!bekant_uart_init(cfg->pin_lin_tx, cfg->pin_lin_rx)) {
        return false;
    }

    memcpy(&s_cfg, cfg, sizeof(s_cfg));
    s_initialized = true;
    s_has_height = false;
    s_height_mm = 0;
    s_target_height_mm = 0;
    s_last_frame_at = 0;
    s_last_height_change_at = 0;
    s_motion_state = BEKANT_MOTION_IDLE;
    bekant_clear_frame();
    bekant_reset_followup_move();
    bekant_reset_calibration_runtime();
    bekant_load_saved_calibration();

    gpio_init(s_cfg.pin_button_up);
    gpio_set_dir(s_cfg.pin_button_up, GPIO_OUT);
    gpio_init(s_cfg.pin_button_down);
    gpio_set_dir(s_cfg.pin_button_down, GPIO_OUT);
    bekant_drive_buttons(BEKANT_MOTION_IDLE);

    return true;
}

void bekant_desk_loop(void) {
    if (!s_initialized) {
        return;
    }

    while (bekant_uart_available() > 0) {
        int value = bekant_uart_read();
        if (value < 0) {
            break;
        }
        bekant_feed_byte((uint8_t)value);
    }

    uint32_t now = atto_now();
    if (s_motion_state != BEKANT_MOTION_IDLE && s_has_height) {
        if ((uint32_t)(now - s_last_frame_at) >= BEKANT_LIN_TIMEOUT_MS) {
            bekant_stop_motion();

            if (bekant_calibration_is_running()) {
                bekant_fail_calibration("calibration failed: LIN timeout");
            } else if (s_followup_move.active) {
                bekant_reset_followup_move();
                app_log_print(BEKANT_LOG_TAG,
                              "followup move cancelled: LIN timeout");
            }
        }
    }

    if (bekant_calibration_is_running()) {
        bekant_process_calibration(now);
    } else {
        bekant_process_followup_move(now);
    }
}

bool bekant_desk_has_height(void) {
    return s_has_height;
}

uint16_t bekant_desk_get_height_mm(void) {
    return s_has_height ? s_height_mm : 0;
}

bool bekant_desk_set_height_mm(uint16_t height_mm) {
    if (!s_initialized) {
        return false;
    }
    if (!bekant_height_is_in_range(height_mm)) {
        return false;
    }
    if (s_motion_state != BEKANT_MOTION_IDLE || s_followup_move.active) {
        return false;
    }
    if (bekant_calibration_is_running()) {
        return false;
    }

    return bekant_start_height_request(height_mm, true, true);
}

bool bekant_desk_start_calibration(void) {
    if (!s_initialized || !s_has_height) {
        return false;
    }
    if (s_motion_state != BEKANT_MOTION_IDLE || s_followup_move.active) {
        return false;
    }
    if (bekant_calibration_is_running()) {
        return false;
    }
    if (!bekant_calibration_targets_are_valid()) {
        return false;
    }
    if (!bekant_height_is_settled(atto_now())) {
        return false;
    }

    bekant_reset_calibration_runtime();
    s_calibration.state = BEKANT_CAL_STATE_MOVE_TO_CENTER;

    bekant_motion_state_t center_direction = BEKANT_MOTION_IDLE;
    if (s_height_mm < BEKANT_CALIBRATION_CENTER_MM) {
        center_direction = BEKANT_MOTION_UP;
    } else if (s_height_mm > BEKANT_CALIBRATION_CENTER_MM) {
        center_direction = BEKANT_MOTION_DOWN;
    }

    if (!bekant_start_calibration_target(BEKANT_CALIBRATION_CENTER_MM,
                                         center_direction)) {
        bekant_reset_calibration_runtime();
        return false;
    }

    app_log_print(BEKANT_LOG_TAG, "calibration started");
    return true;
}

bool bekant_desk_is_calibrating(void) {
    return bekant_calibration_is_running();
}

bool bekant_desk_get_calibration_status(bekant_desk_calibration_status_t *status) {
    if (status == NULL) {
        return false;
    }

    memset(status, 0, sizeof(*status));
    status->running = bekant_calibration_is_running();
    status->has_calibration = s_saved_calibration.valid;
    status->sample_total = BEKANT_CALIBRATION_SAMPLE_COUNT;
    status->up_correction_mm = s_saved_calibration.up_correction_mm;
    status->down_correction_mm = s_saved_calibration.down_correction_mm;
    status->last_error_mm = s_calibration.last_error_mm;

    if (s_calibration.state == BEKANT_CAL_STATE_COMPLETE) {
        status->phase = BEKANT_DESK_CALIBRATION_COMPLETE;
        status->sample_index = BEKANT_CALIBRATION_SAMPLE_COUNT;
        return true;
    }

    if (s_calibration.state == BEKANT_CAL_STATE_ERROR) {
        status->phase = BEKANT_DESK_CALIBRATION_ERROR;
        status->sample_index = s_calibration.completed_samples;
        return true;
    }

    if (!status->running) {
        status->phase = BEKANT_DESK_CALIBRATION_IDLE;
        status->sample_index = 0;
        return true;
    }

    if (s_calibration.state == BEKANT_CAL_STATE_MOVE_TO_CENTER) {
        status->sample_index = 0;
    } else if (s_calibration.state == BEKANT_CAL_STATE_MOVE_SAMPLE) {
        status->sample_index = s_calibration.current_sample_index + 1u;
    }

    if (s_motion_state == BEKANT_MOTION_IDLE &&
        !bekant_height_is_settled(atto_now())) {
        status->phase = BEKANT_DESK_CALIBRATION_SETTLING;
        return true;
    }

    status->phase = s_calibration.state == BEKANT_CAL_STATE_MOVE_TO_CENTER
                        ? BEKANT_DESK_CALIBRATION_MOVE_TO_CENTER
                        : BEKANT_DESK_CALIBRATION_MOVE_SAMPLE;
    return true;
}

const char *bekant_desk_calibration_phase_name(
    bekant_desk_calibration_phase_t phase) {
    switch (phase) {
        case BEKANT_DESK_CALIBRATION_MOVE_TO_CENTER:
            return "move_to_center";
        case BEKANT_DESK_CALIBRATION_MOVE_SAMPLE:
            return "move_sample";
        case BEKANT_DESK_CALIBRATION_SETTLING:
            return "settling";
        case BEKANT_DESK_CALIBRATION_COMPLETE:
            return "complete";
        case BEKANT_DESK_CALIBRATION_ERROR:
            return "error";
        case BEKANT_DESK_CALIBRATION_IDLE:
        default:
            return "idle";
    }
}
