#ifndef _ILI9163C_H_
#define _ILI9163C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"

#define _TFTWIDTH 128 //the REAL W resolution of the TFT
#define _TFTHEIGHT 160 //the REAL H resolution of the TFT
#define _GRAMWIDTH 128
#define _GRAMHEIGH 160
#define _GRAMSIZE _GRAMWIDTH * _GRAMHEIGH //*see note 1
#define __COLORSPC 1 // 1:GBR - 0:RGB
#define __GAMMASET3 //uncomment for another gamma
#define __OFFSET 0

#define	BLACK 0x0000
#define	BLUE 0x001F
#define	RED 0xF800
#define	GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0  
#define WHITE 0xFFFF

#define fillScreen(x) clearScreen(x)

uint8_t write_string(char* str, uint8_t len);

void ILI9163C_Init(void);
void colorSpace(uint8_t cspace);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void clearScreen(uint16_t color);
void setRotation(uint8_t m);
uint8_t boundaryCheck(int16_t x, int16_t y);

void drawPixel(int16_t x, int16_t y, uint16_t color);

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
//void fillScreen(uint16_t color);
void invertDisplay(uint8_t i);

//////////
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void drawBitmapBg(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void setCursor(int16_t x, int16_t y);
void setTextColor(uint16_t c);
void setTextColorBg(uint16_t c, uint16_t bg);
void setTextSize(uint8_t s);
void setTextWrap(uint8_t w);

void write(uint8_t c);
int16_t height(void);
int16_t width(void);
uint8_t getRotation(void);

#ifdef __cplusplus
}
#endif
#endif /* _ILI9163C_H_ */
