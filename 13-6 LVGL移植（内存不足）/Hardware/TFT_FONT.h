#ifndef __TFT_FONT_H
#define __TFT_FONT_H
#include "stm32f10x.h"                  // Device header
#include <stdint.h>

typedef struct {
    const uint8_t width;
    const uint8_t height;
    const uint32_t *data;
} FontDef;

extern const FontDef Font_7x10;
extern const FontDef Font_11x18;
extern const FontDef Font_Custom;

#endif


