#include "color.h"


uint16_t convert_rgb(Rgb *color) {
    uint16_t r = color->r >> 3;
    uint16_t g = color->g >> 2;
    uint16_t b = color->b >> 3;
    return (r << 11) | (g << 5) | b;
}

uint16_t convert_hex565(Hex565 *color) {
    return color->rgb;
}

uint16_t convert_hex888(Hex888 *color) { 
    uint16_t r = (color->rgb >> 19) & 0x1F;
    uint16_t g = (color->rgb >> 10) & 0x3F;
    uint16_t b = (color->rgb >> 3) & 0x1F;
    return (r << 11) | (g << 5) | b;
}
