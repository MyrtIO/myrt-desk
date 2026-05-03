#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void app_log_print(const char *tag, const char *message);
void app_log_printf(const char *tag, const char *format, ...);

#ifdef __cplusplus
}
#endif
