#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include "color.h"

uint16_t rgb_2_565(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t red = r >> 3;
    uint8_t green = g >> 2;
    uint8_t blue = b >> 3;

    uint16_t color = (red << (5 + 6)) | (green << 5) | blue;
    return color;
}