#include "persistent_data.h"
#include <cpreferences.h>
#include <string.h>

#define NVS_NAMESPACE "config"
#define NVS_LIGHT_STATE "light_state"
#define NVS_DESK_CALIBRATION "desk_cal"
#define NVS_POWER_CYCLE "power_cycle"
#define NVS_POWER_CYCLE_COUNT_KEY "count"

static void load_string(cpreferences_t *prefs,
                        const char *key,
                        char *dst,
                        size_t max_len) {
    cpreferences_get_string(prefs, key, dst, max_len, "");
}

void persistent_data_load(persistent_data_t *data) {
    memset(data, 0, sizeof(persistent_data_t));

    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return;
    }

    if (!cpreferences_begin(prefs, NVS_NAMESPACE, true)) {
        cpreferences_destroy(prefs);
        return;
    }

    load_string(prefs, "wifi_ssid", data->wifi_ssid, sizeof(data->wifi_ssid));
    load_string(prefs, "wifi_pass", data->wifi_password,
                sizeof(data->wifi_password));
    load_string(prefs, "mqtt_host", data->mqtt_host, sizeof(data->mqtt_host));
    load_string(prefs, "mqtt_user", data->mqtt_username,
                sizeof(data->mqtt_username));
    load_string(prefs, "mqtt_pass", data->mqtt_password,
                sizeof(data->mqtt_password));

    data->mqtt_port = cpreferences_get_ushort(prefs, "mqtt_port", 1883);
    data->color_correction = cpreferences_get_ulong(prefs, "color_corr", 0xFFFFFF);

    cpreferences_destroy(prefs);
}

void persistent_data_save(const persistent_data_t *data) {
    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return;
    }

    if (!cpreferences_begin(prefs, NVS_NAMESPACE, false)) {
        cpreferences_destroy(prefs);
        return;
    }

    cpreferences_put_string(prefs, "wifi_ssid", data->wifi_ssid);
    cpreferences_put_string(prefs, "wifi_pass", data->wifi_password);
    cpreferences_put_string(prefs, "mqtt_host", data->mqtt_host);
    cpreferences_put_string(prefs, "mqtt_user", data->mqtt_username);
    cpreferences_put_string(prefs, "mqtt_pass", data->mqtt_password);
    cpreferences_put_ushort(prefs, "mqtt_port", data->mqtt_port);
    cpreferences_put_ulong(prefs, "color_corr", data->color_correction);

    cpreferences_destroy(prefs);
}

bool persistent_data_is_configured(const persistent_data_t *data) {
    return data->wifi_ssid[0] != '\0' && data->mqtt_host[0] != '\0';
}

uint8_t power_cycle_count_increment(void) {
    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return 0;
    }

    if (!cpreferences_begin(prefs, NVS_POWER_CYCLE, false)) {
        cpreferences_destroy(prefs);
        return 0;
    }

    uint8_t count = cpreferences_get_uchar(prefs, NVS_POWER_CYCLE_COUNT_KEY, 0);
    if (count < UINT8_MAX) {
        count++;
    }
    cpreferences_put_uchar(prefs, NVS_POWER_CYCLE_COUNT_KEY, count);

    cpreferences_destroy(prefs);
    return count;
}

void power_cycle_count_reset(void) {
    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return;
    }

    if (!cpreferences_begin(prefs, NVS_POWER_CYCLE, false)) {
        cpreferences_destroy(prefs);
        return;
    }

    cpreferences_put_uchar(prefs, NVS_POWER_CYCLE_COUNT_KEY, 0);
    cpreferences_destroy(prefs);
}

bool light_state_exists(void) {
    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return false;
    }

    if (!cpreferences_begin(prefs, NVS_LIGHT_STATE, true)) {
        cpreferences_destroy(prefs);
        return false;
    }

    bool exists = cpreferences_is_key(prefs, "brightness");
    cpreferences_destroy(prefs);
    return exists;
}

void light_state_load(light_saved_state_t *state) {
    memset(state, 0, sizeof(light_saved_state_t));

    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return;
    }

    if (!cpreferences_begin(prefs, NVS_LIGHT_STATE, true)) {
        cpreferences_destroy(prefs);
        return;
    }

    state->power = cpreferences_get_bool(prefs, "power", true);
    state->brightness = cpreferences_get_uchar(prefs, "brightness", 200);
    state->r = cpreferences_get_uchar(prefs, "r", 255);
    state->g = cpreferences_get_uchar(prefs, "g", 255);
    state->b = cpreferences_get_uchar(prefs, "b", 255);
    state->color_temp = cpreferences_get_ushort(prefs, "color_temp", 0);
    state->color_mode = cpreferences_get_uchar(prefs, "color_mode", 0);
    cpreferences_get_string(prefs, "effect", state->effect, sizeof(state->effect),
                            "static");

    cpreferences_destroy(prefs);
}

void light_state_save(const light_saved_state_t *state) {
    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return;
    }

    if (!cpreferences_begin(prefs, NVS_LIGHT_STATE, false)) {
        cpreferences_destroy(prefs);
        return;
    }

    cpreferences_put_bool(prefs, "power", state->power);
    cpreferences_put_uchar(prefs, "brightness", state->brightness);
    cpreferences_put_uchar(prefs, "r", state->r);
    cpreferences_put_uchar(prefs, "g", state->g);
    cpreferences_put_uchar(prefs, "b", state->b);
    cpreferences_put_ushort(prefs, "color_temp", state->color_temp);
    cpreferences_put_uchar(prefs, "color_mode", state->color_mode);
    cpreferences_put_string(prefs, "effect", state->effect);

    cpreferences_destroy(prefs);
}

bool desk_calibration_exists(void) {
    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return false;
    }

    if (!cpreferences_begin(prefs, NVS_DESK_CALIBRATION, true)) {
        cpreferences_destroy(prefs);
        return false;
    }

    bool exists =
        cpreferences_is_key(prefs, "up_mm") && cpreferences_is_key(prefs, "down_mm");
    cpreferences_destroy(prefs);
    return exists;
}

void desk_calibration_load(desk_calibration_data_t *data) {
    memset(data, 0, sizeof(desk_calibration_data_t));

    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return;
    }

    if (!cpreferences_begin(prefs, NVS_DESK_CALIBRATION, true)) {
        cpreferences_destroy(prefs);
        return;
    }

    data->up_correction_mm = cpreferences_get_ushort(prefs, "up_mm", 0);
    data->down_correction_mm = cpreferences_get_ushort(prefs, "down_mm", 0);

    cpreferences_destroy(prefs);
}

void desk_calibration_save(const desk_calibration_data_t *data) {
    cpreferences_t *prefs = cpreferences_create();
    if (prefs == NULL) {
        return;
    }

    if (!cpreferences_begin(prefs, NVS_DESK_CALIBRATION, false)) {
        cpreferences_destroy(prefs);
        return;
    }

    cpreferences_put_ushort(prefs, "up_mm", data->up_correction_mm);
    cpreferences_put_ushort(prefs, "down_mm", data->down_correction_mm);

    cpreferences_destroy(prefs);
}
