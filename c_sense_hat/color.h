#ifndef COLOR_H
#define COLOR_H
#include "stdint.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Rgb;

typedef struct {
    uint16_t rgb;
} Hex565;

typedef struct {
    uint32_t rgb;
} Hex888;

uint16_t convert_rgb(Rgb *color);
uint16_t convert_hex565(Hex565 *color);
uint16_t convert_hex888(Hex888 *color);

#endif
