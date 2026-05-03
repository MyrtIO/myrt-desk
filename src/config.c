#include "config.h"
#include <stdio.h>
#include <stdint.h>

const char *id_buf = "myrtio_desk";
const char *hostname_buf = "myrtio-desk";
const char *name_buf = "MyrtIO Стол";

const char *device_id() {
    return id_buf;
}

const char *device_hostname() {
    return hostname_buf;
}

const char *device_name() {
    return name_buf;
}
