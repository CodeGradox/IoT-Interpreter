#include "color.h"


uint16_t convert_rgb(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t _r = r >> 3;
    uint16_t _g = g >> 2;
    uint16_t _b = b >> 3;
    return (_r << 11) | (_g << 5) | _b;
}

uint16_t convert_hex565(uint16_t color) {
    return color;
}

uint16_t convert_hex888(uint32_t color) { 
    uint16_t r = (color >> 19) & 0x1F;
    uint16_t g = (color >> 10) & 0x3F;
    uint16_t b = (color >> 3) & 0x1F;
    return (r << 11) | (g << 5) | b;
}
