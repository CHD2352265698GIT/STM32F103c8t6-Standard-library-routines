#ifndef __TFT_LCD_H
#define __TFT_LCD_H
#include "stm32f10x.h"                  // Device header
#include "TFT_FONT.h"

#define ST7735_RST_GPIO_RCC			 RCC_APB2Periph_GPIOB
#define ST7735_RST_GPIO_Port     GPIOB
#define ST7735_RST_Pin					 GPIO_Pin_12
#define ST7735_DC_GPIO_RCC			 RCC_APB2Periph_GPIOB
#define	ST7735_DC_GPIO_Port      GPIOB
#define ST7735_DC_Pin            GPIO_Pin_10
#define ST7735_CS_GPIO_RCC			 RCC_APB2Periph_GPIOB
#define	ST7735_CS_GPIO_Port      GPIOB
#define ST7735_CS_Pin            GPIO_Pin_11
#define ST7735_BL_GPIO_RCC			 RCC_APB2Periph_GPIOA
#define	ST7735_BL_GPIO_Port      GPIOA
#define ST7735_BL_Pin            GPIO_Pin_8


//位置调整
#define ST7735_XSTART 0
#define ST7735_YSTART 0
//尺寸调整
#define ST7735_WIDTH  480
#define ST7735_HEIGHT 320

// Screen Direction 角度调整
#define ST7735_ROTATION 0
// Color Mode: RGB or BGR 
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MODE ST7735_MADCTL_RGB
// Color Inverse: 0=NO, 1=YES  反色显示
#define ST7735_INVERSE 0

// Color definitions
#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0x001F
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

void ST7735_Init(void);
void ST7735_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ST7735_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const FontDef *font);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image);

#endif
