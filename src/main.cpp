#include <Arduino.h>
#include <cpubsub.h>
#include <attotime.h>
#include <bekant_desk.h>

#include "config.h"
#include "persistent_data.h"
#include "light.h"
#include "cwifi_manager.h"
#include "mqtt_ha.h"
#include "provisioning_web.h"

static persistent_data_t pdata;
static light_saved_state_t saved_state;

static light_config_t light_cfg;
static bekant_desk_config_t desk_cfg;
static cwifi_runtime_config_t wifi_cfg;
static cpubsub_config_t mqtt_cfg;

static bool manual_provisioning = false;
static bool runtime_reconfigure_pending = false;

static bool provisioning_enabled(void) {
    return manual_provisioning || !persistent_data_is_configured(&pdata);
}

static void build_light_config(void) {
    light_cfg = {
        .pin = CONFIG_LIGHT_CTL_GPIO,
        .led_count = CONFIG_LIGHT_LED_COUNT,
        .led_skip = 0,
        .color_correction = pdata.color_correction,
        .color_order = CONFIG_LIGHT_COLOR_ORDER,
        .kelvin_warm = CONFIG_LIGHT_COLOR_KELVIN_WARM,
        .kelvin_cold = CONFIG_LIGHT_COLOR_KELVIN_COLD,
        .kelvin_initial = CONFIG_LIGHT_COLOR_KELVIN_INITIAL,
        .transition_ms = CONFIG_LIGHT_TRANSITION_COLOR,
        .brightness = CONFIG_LIGHT_BRIGHTNESS_MIN,
        .brightness_max = CONFIG_LIGHT_BRIGHTNESS_MAX,
    };
}

static void build_wifi_config(void) {
    wifi_cfg = {
        .ssid = pdata.wifi_ssid,
        .password = pdata.wifi_password,
        .hostname = device_hostname(),
        .ap_ssid = device_hostname(),
        .reconnect_interval_ms = 15000,
    };
}

static void build_desk_config(void) {
    desk_cfg = {
        .pin_lin_tx = CONFIG_BEKANT_PIN_LIN_TX,
        .pin_lin_rx = CONFIG_BEKANT_PIN_LIN_RX,
        .pin_button_up = CONFIG_BEKANT_PIN_BUTTON_UP,
        .pin_button_down = CONFIG_BEKANT_PIN_BUTTON_DOWN,
        .height_min_mm = CONFIG_BEKANT_HEIGHT_MIN,
        .height_max_mm = CONFIG_BEKANT_HEIGHT_MAX,
        .height_slope = CONFIG_BEKANT_HEIGHT_SLOPE,
        .height_bias = CONFIG_BEKANT_HEIGHT_BIAS,
    };
}

static void build_mqtt_config(void) {
    mqtt_cfg = {
        .client_id = device_id(),
        .host = pdata.mqtt_host,
        .port = (uint16_t)(pdata.mqtt_port != 0 ? pdata.mqtt_port : 1883),
        .buffer_size = CONFIG_MQTT_BUFFER_SIZE,
        .reconnect_delay = CONFIG_MQTT_RECONNECT_DELAY_MS,
        .username = pdata.mqtt_username,
        .password = pdata.mqtt_password,
    };
}

static void rebuild_runtime_config(void) {
    build_light_config();
    build_wifi_config();
    build_mqtt_config();
}

static void on_configuration_changed(void) {
    runtime_reconfigure_pending = true;
    Serial.println("[Main] runtime reconfigure scheduled");
}

static void apply_runtime_config(void) {
    rebuild_runtime_config();
    light_update_config(&light_cfg);
    cwifi_reconfigure(&wifi_cfg, provisioning_enabled());
    cpubsub_reconfigure(&mqtt_cfg);
}

static void set_default_light_state(void) {
    light_saved_state_t state = {};
    state.power = true;
    state.brightness = light_cfg.brightness;
    state.color_temp = light_cfg.kelvin_initial;
    state.color_mode = LIGHT_MODE_WHITE;
    state.r = 255;
    state.g = 255;
    state.b = 255;
    snprintf(state.effect, sizeof(state.effect), "%s", "static");

    light_restore_state(&state);
    light_state_save(&state);
}

static void toggle_provisioning(void) {
    if (!persistent_data_is_configured(&pdata)) {
        Serial.println(
            "[Main] provisioning stays enabled until Wi-Fi and MQTT are configured");
        return;
    }

    manual_provisioning = !manual_provisioning;
    cwifi_set_provisioning(provisioning_enabled());
}

static uint32_t now_u32(void) {
    return (uint32_t)millis();
}

void setup() {
    Serial.begin(115200);
    delay(50);
    Serial.println("Booting...");

    Serial.println("Initializing base");
    atto_init(now_u32);
    build_desk_config();

    Serial.println("Initializing desk");
    if (!bekant_desk_init(&desk_cfg)) {
        Serial.println("[Desk] initialization failed");
    }

    Serial.println("Loading persistent data");
    persistent_data_load(&pdata);
    rebuild_runtime_config();
    bool has_saved_light_state = light_state_exists();

    Serial.println("Initializing network");
    cwifi_init(&wifi_cfg, provisioning_enabled());
    cpubsub_init(&mqtt_cfg);

    Serial.println("Initializing light");
    light_init(&light_cfg);
    if (has_saved_light_state) {
        light_state_load(&saved_state);
        light_restore_state(&saved_state);
    }
    light_start();
    if (!has_saved_light_state) {
        set_default_light_state();
    }

    Serial.println("Initializing web");
    mqtt_ha_init();
    web_init(&pdata, &light_cfg, on_configuration_changed);
    web_start();
    Serial.println("Boot complete");
}

void loop() {
    web_loop();

    if (runtime_reconfigure_pending) {
        runtime_reconfigure_pending = false;
        apply_runtime_config();
    }

    cwifi_loop();
    cpubsub_loop();
    bekant_desk_loop();
    mqtt_ha_loop();
}
