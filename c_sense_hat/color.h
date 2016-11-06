#ifndef COLOR_H
#define COLOR_H
#include "stdint.h"

uint16_t convert_rgb(uint8_t r, uint8_t g, uint8_t b);
uint16_t convert_hex565(uint16_t color);
uint16_t convert_hex888(uint32_t color);

#endif
