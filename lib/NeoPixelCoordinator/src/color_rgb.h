#pragma once

#include <stdint.h>

typedef uint8_t fract8;
typedef uint16_t accum88;
#define saccum87 int16_t
typedef uint16_t accum88;

uint8_t blend8(uint8_t first, uint8_t second, uint8_t ratio);

uint8_t lerp8by8(uint8_t a, uint8_t b, fract8 frac);

uint8_t scale8(uint8_t i, fract8 scale);

uint8_t scale8_video(uint8_t i, fract8 scale);

enum TGradientDirectionCode {
    FORWARD_HUES, ///< Hue always goes clockwise around the color wheel
    BACKWARD_HUES, ///< Hue always goes counter-clockwise around the color wheel
    SHORTEST_HUES, ///< Hue goes whichever way is shortest
    LONGEST_HUES ///< Hue goes whichever way is longest
};

#define FORCE_REFERENCE(var) asm volatile("" : : "r"(var))
#define K255 255
#define K171 171
#define K170 170
#define K85 85

struct HSV {
    union {
        uint8_t h;
        uint8_t hue;
    };
    union {
        uint8_t s;
        uint8_t saturation;
    };
    union {
        uint8_t v;
        uint8_t value;
    };

    inline HSV() __attribute__((always_inline)) = default;

    constexpr HSV(uint8_t ih, uint8_t is, uint8_t iv) __attribute__((always_inline))
    : h(ih), s(is), v(iv) {
    }

    inline bool operator==(const HSV& other) const __attribute__((always_inline)) {
        return h == other.h && s == other.s && v == other.v;
    }

    inline bool operator!=(const HSV& other) const __attribute__((always_inline)) {
        return h != other.h || s != other.s || v != other.v;
    }

    inline HSV& operator=(const HSV& other) __attribute__((always_inline)) {
        h = other.h;
        s = other.s;
        v = other.v;
        return *this;
    }
};

struct RGB {
    union {
        struct {
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
        };
    };

    static RGB fromHSV(const HSV& hsv) {
        RGB rgb;
        // Yellow has a higher inherent brightness than
        // any other color; 'pure' yellow is perceived to
        // be 93% as bright as white.  In order to make
        // yellow appear the correct relative brightness,
        // it has to be rendered brighter than all other
        // colors.
        // Level Y1 is a moderate boost, the default.
        // Level Y2 is a strong boost.
        const uint8_t Y1 = 1;
        const uint8_t Y2 = 0;

        // G2: Whether to divide all greens by two.
        // Depends GREATLY on your particular LEDs
        const uint8_t G2 = 0;

        // Gscale: what to scale green down by.
        // Depends GREATLY on your particular LEDs
        const uint8_t Gscale = 0;

        uint8_t hue = hsv.hue;
        uint8_t sat = hsv.saturation;
        uint8_t val = hsv.value;

        uint8_t offset = hue & 0x1F; // 0..31

        // offset8 = offset * 8
        uint8_t offset8 = offset;
        offset8 <<= 3;

        uint8_t third = scale8(offset8, (256 / 3)); // max = 85

        uint8_t r, g, b;

        if (!(hue & 0x80)) {
            // 0XX
            if (!(hue & 0x40)) {
                // 00X
                // section 0-1
                if (!(hue & 0x20)) {
                    // 000
                    // case 0: // R -> O
                    r = K255 - third;
                    g = third;
                    b = 0;
                    FORCE_REFERENCE(b);
                } else {
                    // 001
                    // case 1: // O -> Y
                    if (Y1) {
                        r = K171;
                        g = K85 + third;
                        b = 0;
                        FORCE_REFERENCE(b);
                    }
                    if (Y2) {
                        r = K170 + third;
                        // uint8_t twothirds = (third << 1);
                        uint8_t twothirds = scale8(offset8, ((256 * 2) / 3)); // max=170
                        g = K85 + twothirds;
                        b = 0;
                        FORCE_REFERENCE(b);
                    }
                }
            } else {
                // 01X
                //  section 2-3
                if (!(hue & 0x20)) {
                    // 010
                    // case 2: // Y -> G
                    if (Y1) {
                        // uint8_t twothirds = (third << 1);
                        uint8_t twothirds = scale8(offset8, ((256 * 2) / 3)); // max=170
                        r = K171 - twothirds;
                        g = K170 + third;
                        b = 0;
                        FORCE_REFERENCE(b);
                    }
                    if (Y2) {
                        r = K255 - offset8;
                        g = K255;
                        b = 0;
                        FORCE_REFERENCE(b);
                    }
                } else {
                    // 011
                    // case 3: // G -> A
                    r = 0;
                    FORCE_REFERENCE(r);
                    g = K255 - third;
                    b = third;
                }
            }
        } else {
            // section 4-7
            // 1XX
            if (!(hue & 0x40)) {
                // 10X
                if (!(hue & 0x20)) {
                    // 100
                    // case 4: // A -> B
                    r = 0;
                    FORCE_REFERENCE(r);
                    // uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8(offset8, ((256 * 2) / 3)); // max=170
                    g = K171 - twothirds; // K170?
                    b = K85 + twothirds;

                } else {
                    // 101
                    // case 5: // B -> P
                    r = third;
                    g = 0;
                    FORCE_REFERENCE(g);
                    b = K255 - third;
                }
            } else {
                if (!(hue & 0x20)) {
                    // 110
                    // case 6: // P -- K
                    r = K85 + third;
                    g = 0;
                    FORCE_REFERENCE(g);
                    b = K171 - third;

                } else {
                    // 111
                    // case 7: // K -> R
                    r = K170 + third;
                    g = 0;
                    FORCE_REFERENCE(g);
                    b = K85 - third;
                }
            }
        }

        // This is one of the good places to scale the green down,
        // although the client can scale green down as well.
        if (G2)
            g = g >> 1;
        if (Gscale)
            g = scale8_video(g, Gscale);

        // Scale down colors if we're desaturated at all
        // and add the brightness_floor to r, g, and b.
        if (sat != 255) {
            if (sat == 0) {
                r = 255;
                b = 255;
                g = 255;
            } else {
                uint8_t desat = 255 - sat;
                desat = scale8_video(desat, desat);

                uint8_t satscale = 255 - desat;
                if (r)
                    r = scale8(r, satscale) + 1;
                if (g)
                    g = scale8(g, satscale) + 1;
                if (b)
                    b = scale8(b, satscale) + 1;
                uint8_t brightness_floor = desat;
                r += brightness_floor;
                g += brightness_floor;
                b += brightness_floor;
            }
        }

        // Now scale everything down if we're at value < 255.
        if (val != 255) {
            val = scale8_video(val, val);
            if (val == 0) {
                r = 0;
                g = 0;
                b = 0;
            } else {
                if (r)
                    r = scale8(r, val) + 1;
                if (g)
                    g = scale8(g, val) + 1;
                if (b)
                    b = scale8(b, val) + 1;
            }
        }

        rgb.r = r;
        rgb.g = g;
        rgb.b = b;

        return rgb;
    }

    inline RGB() __attribute__((always_inline)) = default;

    inline bool operator==(const RGB& other) const __attribute__((always_inline)) {
        return r == other.r && g == other.g && b == other.b;
    }

    inline bool operator!=(const RGB& other) const __attribute__((always_inline)) {
        return r != other.r || g != other.g || b != other.b;
    }

    inline RGB& operator=(const RGB& other) __attribute__((always_inline)) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }

    inline RGB& operator=(const HSV& other) __attribute__((always_inline)) {
        RGB c = this->fromHSV(other);
        r = c.r;
        g = c.g;
        b = c.b;
        return *this;
    }

    constexpr RGB(uint8_t ir, uint8_t ig, uint8_t ib) : r(ir), g(ig), b(ib) {
    }

    constexpr RGB(uint32_t colorcode) __attribute__((always_inline))
    : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF),
      b((colorcode >> 0) & 0xFF) {
    }

    uint32_t u32() const {
        return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
    }

    inline void applyBrightness(uint8_t brightness) {
        uint16_t scale_fixed;
        scale_fixed = brightness + 1;
        r = (((uint16_t)r) * scale_fixed) >> 8;
        g = (((uint16_t)g) * scale_fixed) >> 8;
        b = (((uint16_t)b) * scale_fixed) >> 8;
    }

    enum {
        Black = 0x000000,
        Red = 0xFF0000,
        Green = 0x00FF00,
        Blue = 0x0000FF,
        White = 0xFFFFFF
    };
};

typedef uint8_t fract8;

RGB blendColors(RGB first, RGB second, uint8_t ratio);

void fill_gradient(
RGB* targetArray,
uint16_t startpos,
HSV startcolor,
uint16_t endpos,
HSV endcolor,
TGradientDirectionCode directionCode = SHORTEST_HUES
);

void fill_gradient(RGB* targetArray, uint16_t numLeds, const HSV& c1, const HSV& c2, TGradientDirectionCode directionCode = SHORTEST_HUES);

void fill_gradient(
RGB* targetArray,
uint16_t numLeds,
const HSV& c1,
const HSV& c2,
const HSV& c3,
TGradientDirectionCode directionCode = SHORTEST_HUES
);

void fill_gradient(
RGB* targetArray,
uint16_t numLeds,
const HSV& c1,
const HSV& c2,
const HSV& c3,
const HSV& c4,
TGradientDirectionCode directionCode = SHORTEST_HUES
);
