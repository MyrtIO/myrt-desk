#include "light.h"

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/atomic.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <light_composer.h>
#include <attotime.h>

#include "light_hal.h"
#include "persistent_data.h"
#include "config.h"

static const lc_hal_t hal = {
    .set_pixel = light_hal_set_pixel,
    .get_pixel = light_hal_get_pixel,
    .show = light_hal_show,
};

/* --- Effect name table --- */

typedef struct {
    const char *name;
    lc_effect_t *effect;
} light_effect_entry_t;

static light_effect_entry_t effect_table[] = {
    {"static", &lc_fx_static},      {"rainbow", &lc_fx_rainbow},
    {"neon", &lc_fx_flow_neon},     {"rest", &lc_fx_flow_lava_lamp},
    {"sunset", &lc_fx_flow_sunset}, {"breathing", &lc_fx_breathing},
    {"aurora", &lc_fx_aurora},      {"candle", &lc_fx_candlelight},
    {"tide", &lc_fx_tide},          {"ocean", &lc_fx_ocean},
    {"ember", &lc_fx_ember},
};

#define EFFECT_COUNT (sizeof(effect_table) / sizeof(effect_table[0]))

static const char *effect_names[EFFECT_COUNT];

static lc_effect_t *find_effect(const char *name) {
    for (size_t i = 0; i < EFFECT_COUNT; i++) {
        if (strcmp(effect_table[i].name, name) == 0) {
            return effect_table[i].effect;
        }
    }
    return NULL;
}

static const char *effect_name_of(const lc_effect_t *fx) {
    for (size_t i = 0; i < EFFECT_COUNT; i++) {
        if (effect_table[i].effect == fx) {
            return effect_table[i].name;
        }
    }
    return NULL;
}

/* --- State --- */

static rgb_t pixel_buf[CONFIG_LIGHT_LED_COUNT];

static QueueHandle_t cmd_queue;
static TaskHandle_t render_task_handle;

static light_state_t current_state;
static light_color_mode_t color_mode;
static const light_config_t *light_cfg;
static uint16_t white_kelvin;
static uint8_t brightness_max;
static uint8_t requested_brightness;

#define LIGHT_TASK_PRIORITY (configMAX_PRIORITIES - 3)

/* --- Command handling --- */

static void apply_cmd(const light_cmd_t *cmd) {
    switch (cmd->type) {
        case LIGHT_CMD_POWER:
            lc_set_power(cmd->power);
            break;
        case LIGHT_CMD_BRIGHTNESS: {
            requested_brightness = cmd->brightness;
            lc_set_brightness(lc_scale8_video(cmd->brightness, brightness_max));
            break;
        }
        case LIGHT_CMD_COLOR:
            color_mode = LIGHT_MODE_RGB;
            lc_set_color((rgb_t){cmd->color.r, cmd->color.g, cmd->color.b});
            break;
        case LIGHT_CMD_COLOR_TEMP: {
            uint16_t temp = cmd->color_temp;
            if (temp < light_cfg->kelvin_warm || temp > light_cfg->kelvin_cold) {
                break;
            }
            color_mode = LIGHT_MODE_WHITE;
            white_kelvin = temp;
            rgb_t white;
            lc_kelvin_to_rgb(temp, &white);
            lc_set_color(white);
            break;
        }
        case LIGHT_CMD_EFFECT: {
            if (cmd->effect_name == NULL)
                break;
            lc_effect_t *fx = find_effect(cmd->effect_name);
            if (fx != NULL && lc_get_effect() != fx) {
                lc_set_effect(fx);
            }
            break;
        }
    }
}

static void update_state(void) {
    rgb_t color = lc_get_color();

    portENTER_CRITICAL();
    current_state.power = lc_get_power();
    current_state.brightness = requested_brightness;
    current_state.color = color;
    current_state.color_temp = white_kelvin;
    current_state.effect = effect_name_of(lc_get_target_effect());
    current_state.color_mode = color_mode;
    portEXIT_CRITICAL();
}

static void render_task(void *) {
    for (;;) {
        light_cmd_t cmd;
        while (xQueueReceive(cmd_queue, &cmd, 0) == pdTRUE) {
            apply_cmd(&cmd);
        }
        lc_tick();
        update_state();
        vTaskDelay(1);
    }
}

/* --- Public API --- */

void light_init(light_config_t *cfg) {
    light_cfg = cfg;
    brightness_max = cfg->brightness_max;

    light_hal_init();

    for (size_t i = 0; i < EFFECT_COUNT; i++) {
        effect_names[i] = effect_table[i].name;
    }

    lc_config_t lc_cfg = {
        .hal = &hal,
        .pixel_buf = pixel_buf,
        .max_leds_count = CONFIG_LIGHT_LED_COUNT,
        .leds_count = CONFIG_LIGHT_LED_COUNT,
        .led_skip = 0,
        .color_correction = LC_RGB_FROM_CODE(cfg->color_correction),
        .color_order = (lc_color_order_t)cfg->color_order,
        .fps = 60,
    };
    lc_init(&lc_cfg);

    rgb_t initial_white;
    white_kelvin = cfg->kelvin_initial;
    lc_kelvin_to_rgb(cfg->kelvin_initial, &initial_white);

    lc_set_color(initial_white);
    lc_set_transition(cfg->transition_ms);
    lc_set_effect_force(&lc_fx_static);
    requested_brightness = cfg->brightness;
    color_mode = LIGHT_MODE_WHITE;

    cmd_queue = xQueueCreate(8, sizeof(light_cmd_t));
}

void light_start(void) {
    xTaskCreate(render_task, "light", 4096, NULL, LIGHT_TASK_PRIORITY,
                &render_task_handle);
    vTaskCoreAffinitySet(render_task_handle, 1 << 0);
}

void light_send_cmd(light_cmd_t *cmd) {
    xQueueSend(cmd_queue, cmd, 0);
}

void light_restore_state(light_saved_state_t *state) {
    requested_brightness = state->brightness;
    lc_set_brightness(lc_scale8_video(state->brightness, brightness_max));

    if (state->color_mode == LIGHT_MODE_WHITE && state->color_temp > 0) {
        color_mode = LIGHT_MODE_WHITE;
        white_kelvin = state->color_temp;
        rgb_t white;
        lc_kelvin_to_rgb(state->color_temp, &white);
        lc_set_color(white);
    } else {
        color_mode = LIGHT_MODE_RGB;
        lc_set_color((rgb_t){state->r, state->g, state->b});
    }

    if (state->effect[0] != '\0') {
        lc_effect_t *fx = find_effect(state->effect);
        if (fx != NULL) {
            lc_set_effect_force(fx);
        }
    }

    lc_set_power(state->power);
}

light_state_t light_get_state(void) {
    light_state_t copy;
    portENTER_CRITICAL();
    copy = current_state;
    portEXIT_CRITICAL();
    return copy;
}

const char **light_effect_names(void) {
    return effect_names;
}

uint8_t light_effect_count(void) {
    return EFFECT_COUNT;
}

void light_update_config(light_config_t *cfg) {
    lc_set_leds_count(CONFIG_LIGHT_LED_COUNT);
    lc_set_color_correction(LC_RGB_FROM_CODE(cfg->color_correction));
    lc_set_color_order((lc_color_order_t)cfg->color_order);

    brightness_max = cfg->brightness_max;
    lc_set_brightness(lc_scale8_video(requested_brightness, brightness_max));
}
