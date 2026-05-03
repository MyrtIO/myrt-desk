#include "app_log.h"

#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

namespace {

constexpr size_t kLogBufferSize = 192;

void app_log_emit(const char *tag, const char *message) {
    if (tag == nullptr || message == nullptr) {
        return;
    }

    Serial.print("[");
    Serial.print(tag);
    Serial.print("] ");
    Serial.println(message);
}

}  // namespace

extern "C" void app_log_print(const char *tag, const char *message) {
    app_log_emit(tag, message);
}

extern "C" void app_log_printf(const char *tag, const char *format, ...) {
    if (tag == nullptr || format == nullptr) {
        return;
    }

    char buffer[kLogBufferSize];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    app_log_emit(tag, buffer);
}
