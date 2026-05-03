#include "light_hal.h"

#include <string.h>

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/time.h"
#include "hardware/sync.h"

#include "config.h"

#define WS2812_FREQ_HZ 800000u
#define WS2812_RESET_US 300u

static rgb_t raw_leds[CONFIG_LIGHT_LED_COUNT];

static PIO ws2812_pio = pio0;
static uint ws2812_sm = 0;
static uint ws2812_offset = 0;

// PIO program from the standard Pico WS2812 approach.
// Bit timing: T1=2, T2=5, T3=3, total 10 PIO cycles per bit.
// At 800 kHz WS2812 bit rate => 8 MHz PIO state machine clock.
static const uint16_t ws2812_program_instructions[] = {
    // .wrap_target
    0x6221,  // out    x, 1        side 0 [2]
    0x1123,  // jmp    !x, 3       side 1 [1]
    0x1400,  // jmp    0           side 1 [4]
    0xa442,  // nop                side 0 [4]
    // .wrap
};

static const struct pio_program ws2812_program = {
    .instructions = ws2812_program_instructions,
    .length = 4,
    .origin = -1,
};

static void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_sm_config c = pio_get_default_sm_config();

    sm_config_set_wrap(&c, offset + 0, offset + 3);
    sm_config_set_sideset(&c, 1, false, false);
    sm_config_set_sideset_pins(&c, pin);

    sm_config_set_out_shift(&c, false, true, 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    const float div = (float)clock_get_hz(clk_sys) / (WS2812_FREQ_HZ * 10.0f);
    sm_config_set_clkdiv(&c, div);

    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

static inline uint32_t rgb_to_u32(rgb_t c) {
    return ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | ((uint32_t)c.b);
}

static inline void ws2812_put_pixel(uint32_t grb) {
    pio_sm_put_blocking(ws2812_pio, ws2812_sm, grb << 8u);
}

void light_hal_init(void) {
    memset(raw_leds, 0, sizeof(raw_leds));

    ws2812_pio = pio0;
    ws2812_sm = pio_claim_unused_sm(ws2812_pio, true);
    ws2812_offset = pio_add_program(ws2812_pio, &ws2812_program);

    ws2812_program_init(ws2812_pio, ws2812_sm, ws2812_offset, CONFIG_LIGHT_CTL_GPIO);

    light_hal_show();
}

void light_hal_set_pixel(uint16_t i, rgb_t c) {
    if (i >= CONFIG_LIGHT_LED_COUNT) {
        return;
    }

    raw_leds[i] = c;
}

rgb_t light_hal_get_pixel(uint16_t i) {
    if (i >= CONFIG_LIGHT_LED_COUNT) {
        return (rgb_t){0, 0, 0};
    }

    return raw_leds[i];
}

void light_hal_show(void) {
    uint32_t irq_state = save_and_disable_interrupts();

    for (uint16_t i = 0; i < CONFIG_LIGHT_LED_COUNT; ++i) {
        ws2812_put_pixel(rgb_to_u32(raw_leds[i]));
    }

    while (!pio_sm_is_tx_fifo_empty(ws2812_pio, ws2812_sm)) {
        tight_loop_contents();
    }

    restore_interrupts(irq_state);

    busy_wait_us_32(WS2812_RESET_US);
}

void light_hal_clear(void) {
    memset(raw_leds, 0, sizeof(raw_leds));
    light_hal_show();
}
