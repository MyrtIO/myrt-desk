#include "provisioning_web.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <lwjson/lwjson.h>
#include <lwjson/lwjson_serializer.h>
#include <attotime.h>
#include <cupdate.h>
#include <cwebserver.h>
#include <cwifi_manager.h>

#include "bekant_desk.h"
#include "provisioning_page.h"
#include "config.h"

#define LK(s) (s), (sizeof(s) - 1)
#define PARSE_TOKENS 32
#define JSON_BUF_SIZE 768
#define IP_BUF_SIZE 16
#define OTA_ERROR_BUF_SIZE 96
#define OTA_HEADER_BUF_SIZE 128
#define OTA_FILENAME_BUF_SIZE 64
#define OTA_RESTART_DELAY_MS 1500UL

static persistent_data_t *s_pdata;
static light_config_t *s_light_cfg;
static web_configuration_changed_cb_t s_on_configuration_changed;
static char json_buf[JSON_BUF_SIZE];
static const char *csrf_headers[] = {"Origin", "Host"};
static bool s_ota_upload_started = false;
static bool s_ota_upload_finished = false;
static bool s_ota_restart_pending = false;
static unsigned long s_ota_restart_at_ms = 0;
static char s_ota_error[OTA_ERROR_BUF_SIZE];

/* --- Serializer helpers --- */

static bool ser_finalize(lwjson_serializer_t *ser) {
    size_t len = 0;
    if (lwjson_serializer_finalize(ser, &len) != lwjsonOK) {
        return false;
    }
    if (len < sizeof(json_buf)) {
        json_buf[len] = '\0';
    }
    return true;
}

static bool send_json_message(int code, const char *field, const char *message) {
    lwjson_serializer_t ser;
    if (lwjson_serializer_init(&ser, json_buf, sizeof(json_buf) - 1) != lwjsonOK) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return false;
    }

    lwjson_serializer_start_object(&ser, NULL, 0);
    lwjson_serializer_add_string(&ser, field, strlen(field), message,
                                 strlen(message));
    lwjson_serializer_end_object(&ser);

    if (!ser_finalize(&ser)) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return false;
    }

    cwebserver_send(code, "application/json", json_buf);
    return true;
}

static void send_json_error(int code, const char *message) {
    send_json_message(code, "error", message);
}

static void send_ota_success(void) {
    lwjson_serializer_t ser;
    if (lwjson_serializer_init(&ser, json_buf, sizeof(json_buf) - 1) != lwjsonOK) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return;
    }

    lwjson_serializer_start_object(&ser, NULL, 0);
    lwjson_serializer_add_string(
        &ser, LK("message"),
        LK("Прошивка загружена. Устройство перезагрузится через несколько секунд."));
    lwjson_serializer_add_bool(&ser, LK("rebooting"), 1);
    lwjson_serializer_end_object(&ser);

    if (!ser_finalize(&ser)) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return;
    }

    cwebserver_send(200, "application/json", json_buf);
}

/* --- Request helpers --- */

static size_t request_body(char *buf, size_t buf_size) {
    return cwebserver_body(buf, buf_size);
}

/* --- Parser helpers --- */

static void copy_json_string(const lwjson_token_t *t, char *dst, size_t dst_size) {
    size_t slen;
    const char *val = lwjson_get_val_string(t, &slen);
    if (val == NULL)
        return;
    size_t n = slen < dst_size - 1 ? slen : dst_size - 1;
    memcpy(dst, val, n);
    dst[n] = '\0';
}

static const lwjson_token_t *find_field(lwjson_t *parser,
                                        const lwjson_token_t *parent,
                                        const char *key) {
    if (parent != NULL) {
        return lwjson_find_ex(parser, parent, key);
    }
    return lwjson_find(parser, key);
}

static void parse_light_fields(lwjson_t *parser, const lwjson_token_t *parent) {
    const lwjson_token_t *t;

    t = find_field(parser, parent, "led_count");

    t = find_field(parser, parent, "color_correction");
    if (t != NULL && t->type == LWJSON_TYPE_NUM_INT)
        s_pdata->color_correction = (uint32_t)lwjson_get_val_int(t);
}

/* --- Light config rebuild --- */

static void rebuild_config(void) {
    s_light_cfg->pin = CONFIG_LIGHT_CTL_GPIO;
    s_light_cfg->color_correction = s_pdata->color_correction;
    s_light_cfg->kelvin_warm = CONFIG_LIGHT_COLOR_KELVIN_WARM;
    s_light_cfg->kelvin_cold = CONFIG_LIGHT_COLOR_KELVIN_COLD;
    s_light_cfg->kelvin_initial = CONFIG_LIGHT_COLOR_KELVIN_INITIAL;
    s_light_cfg->transition_ms = CONFIG_LIGHT_TRANSITION_COLOR;
}

/* --- OTA helpers --- */

static void ota_reset_state(void) {
    s_ota_upload_started = false;
    s_ota_upload_finished = false;
    s_ota_error[0] = '\0';
}

static void ota_set_error(const char *message) {
    snprintf(s_ota_error, sizeof(s_ota_error), "%s", message);
}

static void ota_store_update_error(void) {
    ota_set_error(cupdate_error_string());
}

static bool ota_origin_valid(void) {
    char origin[OTA_HEADER_BUF_SIZE];
    size_t origin_len = cwebserver_header("Origin", origin, sizeof(origin));
    if (origin_len == 0) {
        return true;
    }

    char host[OTA_HEADER_BUF_SIZE];
    size_t host_len = cwebserver_header("Host", host, sizeof(host));
    if (host_len == 0) {
        return false;
    }

    char expected_http[OTA_HEADER_BUF_SIZE];
    char expected_https[OTA_HEADER_BUF_SIZE];
    snprintf(expected_http, sizeof(expected_http), "http://%s", host);
    snprintf(expected_https, sizeof(expected_https), "https://%s", host);
    return strcmp(origin, expected_http) == 0 || strcmp(origin, expected_https) == 0;
}

static void ota_schedule_restart(void) {
    s_ota_restart_pending = true;
    s_ota_restart_at_ms = atto_now() + OTA_RESTART_DELAY_MS;
}

/* --- API handlers --- */

static void handle_page(void *user_data) {
    (void)user_data;
    cwebserver_send_header("Content-Encoding", "gzip", false);
    cwebserver_send_progmem(200, "text/html", provisioning_page,
                            provisioning_page_len);
}

static void handle_get_configuration(void *user_data) {
    (void)user_data;
    lwjson_serializer_t ser;
    if (lwjson_serializer_init(&ser, json_buf, sizeof(json_buf) - 1) != lwjsonOK) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return;
    }

    lwjson_serializer_start_object(&ser, NULL, 0);

    lwjson_serializer_start_object(&ser, LK("wifi"));
    lwjson_serializer_add_string(&ser, LK("ssid"), s_pdata->wifi_ssid,
                                 strlen(s_pdata->wifi_ssid));
    lwjson_serializer_add_string(&ser, LK("password"), s_pdata->wifi_password,
                                 strlen(s_pdata->wifi_password));
    lwjson_serializer_end_object(&ser);

    lwjson_serializer_start_object(&ser, LK("mqtt"));
    lwjson_serializer_add_string(&ser, LK("host"), s_pdata->mqtt_host,
                                 strlen(s_pdata->mqtt_host));
    lwjson_serializer_add_uint(&ser, LK("port"), s_pdata->mqtt_port);
    lwjson_serializer_add_string(&ser, LK("username"), s_pdata->mqtt_username,
                                 strlen(s_pdata->mqtt_username));
    lwjson_serializer_add_string(&ser, LK("password"), s_pdata->mqtt_password,
                                 strlen(s_pdata->mqtt_password));
    lwjson_serializer_end_object(&ser);

    lwjson_serializer_start_object(&ser, LK("light"));
    lwjson_serializer_add_uint(&ser, LK("color_correction"),
                               s_pdata->color_correction);
    lwjson_serializer_end_object(&ser);

    lwjson_serializer_end_object(&ser);

    if (!ser_finalize(&ser)) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return;
    }
    cwebserver_send(200, "application/json", json_buf);
}

static void handle_post_configuration(void *user_data) {
    (void)user_data;
    size_t body_len = request_body(json_buf, sizeof(json_buf));

    lwjson_token_t tokens[PARSE_TOKENS];
    lwjson_t parser;
    lwjson_init(&parser, tokens, LWJSON_ARRAYSIZE(tokens));

    if (lwjson_parse_ex(&parser, json_buf, body_len) != lwjsonOK) {
        cwebserver_send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    const lwjson_token_t *wifi = lwjson_find(&parser, "wifi");
    if (wifi != NULL && wifi->type == LWJSON_TYPE_OBJECT) {
        const lwjson_token_t *t;
        t = lwjson_find_ex(&parser, wifi, "ssid");
        if (t != NULL && t->type == LWJSON_TYPE_STRING)
            copy_json_string(t, s_pdata->wifi_ssid, sizeof(s_pdata->wifi_ssid));
        t = lwjson_find_ex(&parser, wifi, "password");
        if (t != NULL && t->type == LWJSON_TYPE_STRING)
            copy_json_string(t, s_pdata->wifi_password,
                             sizeof(s_pdata->wifi_password));
    }

    const lwjson_token_t *mqtt = lwjson_find(&parser, "mqtt");
    if (mqtt != NULL && mqtt->type == LWJSON_TYPE_OBJECT) {
        const lwjson_token_t *t;
        t = lwjson_find_ex(&parser, mqtt, "host");
        if (t != NULL && t->type == LWJSON_TYPE_STRING)
            copy_json_string(t, s_pdata->mqtt_host, sizeof(s_pdata->mqtt_host));
        t = lwjson_find_ex(&parser, mqtt, "port");
        if (t != NULL && t->type == LWJSON_TYPE_NUM_INT)
            s_pdata->mqtt_port = (uint16_t)lwjson_get_val_int(t);
        t = lwjson_find_ex(&parser, mqtt, "username");
        if (t != NULL && t->type == LWJSON_TYPE_STRING)
            copy_json_string(t, s_pdata->mqtt_username,
                             sizeof(s_pdata->mqtt_username));
        t = lwjson_find_ex(&parser, mqtt, "password");
        if (t != NULL && t->type == LWJSON_TYPE_STRING)
            copy_json_string(t, s_pdata->mqtt_password,
                             sizeof(s_pdata->mqtt_password));
    }

    const lwjson_token_t *light = lwjson_find(&parser, "light");
    if (light != NULL && light->type == LWJSON_TYPE_OBJECT) {
        parse_light_fields(&parser, light);
    }

    lwjson_free(&parser);
    persistent_data_save(s_pdata);

    cwebserver_send(204, "text/plain", "");
    if (s_on_configuration_changed != NULL) {
        s_on_configuration_changed();
    }
}

static void handle_post_light_configuration(void *user_data) {
    (void)user_data;
    size_t body_len = request_body(json_buf, sizeof(json_buf));

    lwjson_token_t tokens[PARSE_TOKENS];
    lwjson_t parser;
    lwjson_init(&parser, tokens, LWJSON_ARRAYSIZE(tokens));

    if (lwjson_parse_ex(&parser, json_buf, body_len) != lwjsonOK) {
        cwebserver_send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    parse_light_fields(&parser, NULL);
    lwjson_free(&parser);

    rebuild_config();
    light_update_config(s_light_cfg);

    cwebserver_send(204, "text/plain", "");
}

static void handle_post_light_test(void *user_data) {
    (void)user_data;
    size_t body_len = request_body(json_buf, sizeof(json_buf));

    lwjson_token_t tokens[PARSE_TOKENS];
    lwjson_t parser;
    lwjson_init(&parser, tokens, LWJSON_ARRAYSIZE(tokens));

    if (lwjson_parse_ex(&parser, json_buf, body_len) != lwjsonOK) {
        cwebserver_send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    light_cmd_t cmd;
    uint8_t test_r = 0;
    uint8_t test_g = 0;
    uint8_t test_b = 0;
    uint8_t test_brightness = 128;
    cmd.type = LIGHT_CMD_COLOR;
    cmd.color.r = 0;
    cmd.color.g = 0;
    cmd.color.b = 0;

    const lwjson_token_t *t;
    t = lwjson_find(&parser, "r");
    if (t != NULL && t->type == LWJSON_TYPE_NUM_INT)
        cmd.color.r = (uint8_t)lwjson_get_val_int(t);
    t = lwjson_find(&parser, "g");
    if (t != NULL && t->type == LWJSON_TYPE_NUM_INT)
        cmd.color.g = (uint8_t)lwjson_get_val_int(t);
    t = lwjson_find(&parser, "b");
    if (t != NULL && t->type == LWJSON_TYPE_NUM_INT)
        cmd.color.b = (uint8_t)lwjson_get_val_int(t);
    test_r = cmd.color.r;
    test_g = cmd.color.g;
    test_b = cmd.color.b;
    light_send_cmd(&cmd);

    cmd.type = LIGHT_CMD_BRIGHTNESS;
    cmd.brightness = 128;
    t = lwjson_find(&parser, "brightness");
    if (t != NULL && t->type == LWJSON_TYPE_NUM_INT)
        cmd.brightness = (uint8_t)lwjson_get_val_int(t);
    test_brightness = cmd.brightness;
    light_send_cmd(&cmd);

    lwjson_free(&parser);
    cwebserver_send(204, "text/plain", "");
}

static bool send_desk_calibration_status(int code) {
    bekant_desk_calibration_status_t status;
    if (!bekant_desk_get_calibration_status(&status)) {
        cwebserver_send(500, "application/json", "{\"error\":\"desk status\"}");
        return false;
    }

    lwjson_serializer_t ser;
    if (lwjson_serializer_init(&ser, json_buf, sizeof(json_buf) - 1) != lwjsonOK) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return false;
    }

    const char *phase_name = bekant_desk_calibration_phase_name(status.phase);

    lwjson_serializer_start_object(&ser, NULL, 0);
    lwjson_serializer_add_bool(&ser, LK("running"), status.running);
    lwjson_serializer_add_string(&ser, LK("phase"), phase_name, strlen(phase_name));
    lwjson_serializer_add_uint(&ser, LK("sample_index"), status.sample_index);
    lwjson_serializer_add_uint(&ser, LK("sample_total"), status.sample_total);
    lwjson_serializer_add_bool(&ser, LK("has_calibration"), status.has_calibration);
    lwjson_serializer_add_uint(&ser, LK("up_correction_mm"),
                               status.up_correction_mm);
    lwjson_serializer_add_uint(&ser, LK("down_correction_mm"),
                               status.down_correction_mm);
    lwjson_serializer_add_int(&ser, LK("last_error"), status.last_error_mm);
    lwjson_serializer_end_object(&ser);

    if (!ser_finalize(&ser)) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return false;
    }

    cwebserver_send(code, "application/json", json_buf);
    return true;
}

static void handle_get_desk_calibration(void *user_data) {
    (void)user_data;
    send_desk_calibration_status(200);
}

static void handle_post_desk_calibration(void *user_data) {
    (void)user_data;

    if (bekant_desk_start_calibration()) {
        send_json_message(202, "message", "Калибровка стола запущена.");
        return;
    }

    if (bekant_desk_is_calibrating()) {
        send_json_error(409, "Калибровка уже выполняется.");
        return;
    }

    if (!bekant_desk_has_height()) {
        send_json_error(409, "Текущая высота стола недоступна.");
        return;
    }

    send_json_error(409, "Стол занят или ещё не остановился.");
}

static void handle_get_system(void *user_data) {
    (void)user_data;
    lwjson_serializer_t ser;
    if (lwjson_serializer_init(&ser, json_buf, sizeof(json_buf) - 1) != lwjsonOK) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return;
    }

    lwjson_serializer_start_object(&ser, NULL, 0);

    const char *ver = __DATE__ " " __TIME__;
    lwjson_serializer_add_string(&ser, LK("build_version"), ver, strlen(ver));

    const char *network_mode = cwifi_network_mode_string();
    lwjson_serializer_add_string(&ser, LK("network_mode"), network_mode,
                                 strlen(network_mode));

    char sta_ip[IP_BUF_SIZE];
    size_t sta_ip_len = cwifi_sta_ip_string(sta_ip, sizeof(sta_ip));
    lwjson_serializer_add_string(&ser, LK("sta_ip"), sta_ip, sta_ip_len);

    char ap_ip[IP_BUF_SIZE];
    size_t ap_ip_len = cwifi_ap_ip_string(ap_ip, sizeof(ap_ip));
    lwjson_serializer_add_string(&ser, LK("ap_ip"), ap_ip, ap_ip_len);

    uint8_t mac[6];
    cwifi_mac_address(mac);
    lwjson_serializer_start_array(&ser, LK("mac_address"));
    for (int i = 0; i < 6; i++) {
        lwjson_serializer_add_uint(&ser, NULL, 0, mac[i]);
    }
    lwjson_serializer_end_array(&ser);

    lwjson_serializer_end_object(&ser);

    if (!ser_finalize(&ser)) {
        cwebserver_send(500, "application/json", "{\"error\":\"serializer\"}");
        return;
    }
    cwebserver_send(200, "application/json", json_buf);
}

static void handle_post_ota_finished(void *user_data) {
    (void)user_data;
    cwebserver_send_header("Connection", "close", false);

    if (!s_ota_upload_started) {
        send_json_error(400, "Файл прошивки не получен.");
        return;
    }

    if (s_ota_error[0] != '\0') {
        send_json_error(500, s_ota_error);
        return;
    }

    if (!s_ota_upload_finished) {
        send_json_error(500, "Загрузка прошивки завершилась раньше времени.");
        return;
    }

    cwebserver_client_set_no_delay(true);
    send_ota_success();
    ota_schedule_restart();
}

static void handle_post_ota_upload(void *user_data) {
    (void)user_data;
    cwebserver_upload_t upload;
    if (!cwebserver_get_upload(&upload)) {
        ota_set_error("Ошибка обработки загрузки прошивки.");
        return;
    }

    if (upload.status == CWEBUPLOAD_START) {
        ota_reset_state();
        s_ota_upload_started = true;

        if (!ota_origin_valid()) {
            ota_set_error("Некорректный источник запроса.");
            return;
        }

        char filename[OTA_FILENAME_BUF_SIZE];
        cwebserver_upload_filename(filename, sizeof(filename));

        uint32_t max_sketch_space = (cupdate_get_max_size() - 0x1000) & 0xFFFFF000;
        if (!cupdate_begin(max_sketch_space)) {
            ota_store_update_error();
        }
    } else if (upload.status == CWEBUPLOAD_WRITE) {
        if (s_ota_error[0] != '\0') {
            return;
        }

        if (cupdate_write(upload.buf, upload.current_size) != upload.current_size) {
            ota_store_update_error();
        }
    } else if (upload.status == CWEBUPLOAD_END) {
        if (s_ota_error[0] == '\0' && !cupdate_end(true)) {
            ota_store_update_error();
        }

        if (s_ota_error[0] == '\0') {
            s_ota_upload_finished = true;
        }
    } else if (upload.status == CWEBUPLOAD_ABORTED) {
        cupdate_abort();
        ota_set_error("Загрузка была прервана.");
    }
}

/* --- Public API --- */

void web_init(persistent_data_t *pdata,
              light_config_t *light_cfg,
              web_configuration_changed_cb_t on_configuration_changed) {
    s_pdata = pdata;
    s_light_cfg = light_cfg;
    s_on_configuration_changed = on_configuration_changed;
    rebuild_config();
    ota_reset_state();
    s_ota_restart_pending = false;

    cwebserver_init(80);
    cwebserver_collect_headers(csrf_headers,
                               sizeof(csrf_headers) / sizeof(csrf_headers[0]));
    cwebserver_on("/", WEB_METHOD_GET, handle_page, NULL);
    cwebserver_on("/api/configuration", WEB_METHOD_GET, handle_get_configuration,
                  NULL);
    cwebserver_on("/api/configuration", WEB_METHOD_POST, handle_post_configuration,
                  NULL);
    cwebserver_on("/api/configuration/light", WEB_METHOD_POST,
                  handle_post_light_configuration, NULL);
    cwebserver_on("/api/light/test", WEB_METHOD_POST, handle_post_light_test, NULL);
    cwebserver_on("/api/desk/calibration", WEB_METHOD_GET,
                  handle_get_desk_calibration, NULL);
    cwebserver_on("/api/desk/calibration", WEB_METHOD_POST,
                  handle_post_desk_calibration, NULL);
    cwebserver_on("/api/system", WEB_METHOD_GET, handle_get_system, NULL);
    cwebserver_on_with_upload("/api/ota", WEB_METHOD_POST, handle_post_ota_finished,
                              handle_post_ota_upload, NULL);
}

void web_start(void) {
    cwebserver_start();
}

void web_loop(void) {
    cwebserver_loop();

    if (s_ota_restart_pending && atto_now() >= s_ota_restart_at_ms) {
        cupdate_restart();
    }
}
