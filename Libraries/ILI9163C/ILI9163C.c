#include "ILI9163C.h"
#include "ILI9163C_regs.h"
#include "gpio.h"
#include "utility.h"
#include "spi.h"
#include "glcdfont.c"

#define ILI9163C_CS_HIGH digital_high(PA12)
#define ILI9163C_CS_LOW digital_low(PA12)

#define ILI9163C_A0_HIGH digital_high(PF6)
#define ILI9163C_A0_LOW digital_low(PF6)

#define ILI9163C_RST_HIGH digital_high(PA15)
#define ILI9163C_RST_LOW digital_low(PA15)

#define spiwrite(x) SPI1_Transfer(x)

uint8_t _Mactrl_Data;//container for the memory access control data
uint8_t _colorspaceData;

///////////////////////////
// From Adafruit_GFX
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
const int16_t WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
int16_t _width, _height; // Display w/h as modified by current rotation
int16_t cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t textsize, rotation;
uint8_t wrap;  // If set, 'wrap' text at right edge of display
///////////////////////////

void write_command(uint8_t c) {
    ILI9163C_A0_LOW;
    ILI9163C_CS_LOW;
    spiwrite(c);
    ILI9163C_CS_HIGH;
}

void write_data(uint8_t c) {
    ILI9163C_A0_HIGH;
    ILI9163C_CS_LOW;
    spiwrite(c);
    ILI9163C_CS_HIGH;
} 

void write_data16(uint16_t d) {
    ILI9163C_A0_HIGH;
    ILI9163C_CS_LOW;
    spiwrite(d >> 8);
    spiwrite(d);
    ILI9163C_CS_HIGH;
}

void ILI9163C_Init(void) {
    uint8_t i;

    // GPIO_Init
    /*
     * CS - PA12
     * A0 - PF6
     * RST - PA15
     */
    digital_init(PA12);
    digital_init(PA15);
    digital_init(PF6);

    ILI9163C_RST_HIGH;
    delay_ms(500);
    ILI9163C_RST_LOW;
    delay_ms(500);
    ILI9163C_RST_HIGH;
    delay_ms(500);

    _Mactrl_Data = 0;
    _colorspaceData = 1;

    // CHIP_Init
#if defined(__GAMMASET1)
    const uint8_t pGammaSet[15]= {0x36,0x29,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x12,0x0A,0x11,0x0B,0x06};
    const uint8_t nGammaSet[15]= {0x09,0x16,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x34,0x39};
#elif defined(__GAMMASET2)
    const uint8_t pGammaSet[15]= {0x3F,0x21,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x02,0x0A,0x01,0x00,0x00};
    const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
#elif defined(__GAMMASET3)
    const uint8_t pGammaSet[15]= {0x3F,0x26,0x23,0x30,0x28,0x10,0x55,0xB7,0x40,0x19,0x10,0x1E,0x02,0x01,0x00};
    const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
#else
    const uint8_t pGammaSet[15]= {0x3F,0x25,0x1C,0x1E,0x20,0x12,0x2A,0x90,0x24,0x11,0x00,0x00,0x00,0x00,0x00};
    const uint8_t nGammaSet[15]= {0x20,0x20,0x20,0x20,0x05,0x15,0x00,0xA7,0x3D,0x18,0x25,0x2A,0x2B,0x2B,0x3A};
#endif

    write_command(CMD_SWRESET);//software reset
    delay_ms(500);
    write_command(CMD_SLPOUT);//exit sleep
    delay_ms(5);
    write_command(CMD_PIXFMT);//Set Color Format 16bit   
    write_data(0x05);
    delay_ms(5);
    write_command(CMD_GAMMASET);//default gamma curve 3
    write_data(0x04);//0x04
    delay_ms(1);
    write_command(CMD_GAMRSEL);//Enable Gamma adj    
    write_data(0x01); 
    delay_ms(1);
    write_command(CMD_NORML);
    write_command(CMD_DFUNCTR);

    write_data(0b11111111);
    write_data(0b00000110);

    write_command(CMD_PGAMMAC);//Positive Gamma Correction Setting
    for (i=0;i<15;i++){
        write_data(pGammaSet[i]);
    }
    write_command(CMD_NGAMMAC);//Negative Gamma Correction Setting
    for (i=0;i<15;i++){
        write_data(nGammaSet[i]);
    }

    write_command(CMD_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
    write_data(0x08);//0x0C//0x08
    write_data(0x02);//0x14//0x08
    delay_ms(1);
    write_command(CMD_DINVCTR);//display inversion 
    write_data(0x07);
    delay_ms(1);
    write_command(CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD   
    write_data(0x0A);//4.30 - 0x0A
    write_data(0x02);//0x05
    delay_ms(1);
    write_command(CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL   
    write_data(0x02);
    delay_ms(1);
    write_command(CMD_VCOMCTR1);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML   
    write_data(0x50);//0x50
    write_data(99);//0x5b
    delay_ms(1);
    write_command(CMD_VCOMOFFS);
    write_data(0);//0x40
    delay_ms(1);

    write_command(CMD_CLMADRS);//Set Column Address  
    write_data16(0x00); 
    write_data16(_GRAMWIDTH); 

    write_command(CMD_PGEADRS);//Set Page Address  
    write_data16(0X00); 
    write_data16(_GRAMHEIGH);
    // set scroll area (thanks Masuda)
    write_command(CMD_VSCLLDEF);
    write_data16(__OFFSET);
    write_data16(_GRAMHEIGH - __OFFSET);
    write_data16(0);

    colorSpace(_colorspaceData);
    setRotation(0);

    write_command(CMD_DISPON);//display ON 
    delay_ms(1);
    write_command(CMD_RAMWR);//Memory Write

    delay_ms(1);

    clearScreen(BLACK);
}

////////////////////////////////////////////////////////

/*
 * Colorspace selection:
 * 0: RGB
 * 1: GBR
 */
void colorSpace(uint8_t cspace) {
    if (cspace < 1) {
        _Mactrl_Data &= 0b11110111;
    } else {
        _Mactrl_Data |= 0b00001000;
    }
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    write_command(CMD_CLMADRS); // Column
    if (rotation == 0 || rotation > 1){
        write_data16(x0);
        write_data16(x1);
    } else {
        write_data16(x0 + __OFFSET);
        write_data16(x1 + __OFFSET);
    }

    write_command(CMD_PGEADRS); // Page
    if (rotation == 0){
        write_data16(y0 + __OFFSET);
        write_data16(y1 + __OFFSET);
    } else {
        write_data16(y0);
        write_data16(y1);
    }
    write_command(CMD_RAMWR); //Into RAM
}

void clearScreen(uint16_t color) {
    int px;
    setAddrWindow(0x00, 0x00, _width - 1, _height - 1);
    for (px = 0; px < _GRAMSIZE; px++){
        write_data16(color);
    }
}

void setRotation(uint8_t m) {
    rotation = m % 4;
    switch (rotation) {
    case 0:
        _Mactrl_Data = 0b00001000;
        _width  = _TFTWIDTH;
        _height = _TFTHEIGHT;
        break;
    case 1:
        _Mactrl_Data = 0b01101000;
        _width  = _TFTHEIGHT;
        _height = _TFTWIDTH;
        break;
    case 2:
        _Mactrl_Data = 0b11001000;
        _width  = _TFTWIDTH;
        _height = _TFTHEIGHT;
        break;
    case 3:
        _Mactrl_Data = 0b10101000;
        _width  = _TFTWIDTH;
        _height = _TFTHEIGHT;
        break;
    }
    colorSpace(_colorspaceData);
    write_command(CMD_MADCTL);
    write_data(_Mactrl_Data);
}

uint8_t boundaryCheck(int16_t x, int16_t y) {
    if ((x >= _width) || (y >= _height))
        return 1;
    return 0;
}

/////////////////////
void drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (boundaryCheck(x,y))
        return;
    if ((x < 0) || (y < 0))
        return;

    setAddrWindow(x, y, x + 1, y + 1);
    write_data16(color);
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if (boundaryCheck(x, y))
        return;
    if (((y + h) - 1) >= _height)
        h = _height - y;
    setAddrWindow(x, y, x, y + h - 1);
    while (h-- > 1) {
        write_data16(color);
    }
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if (boundaryCheck(x, y))
        return;
    if ((x + w - 1) >= _width)
        w = _width-x;
    setAddrWindow(x, y, x + w - 1, y);
    while (w-- > 1) {
        write_data16(color);
    }
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (boundaryCheck(x, y))
        return;
    if ((x + w - 1) >= _width)
        w = _width  - x;
    if ((y + h - 1) >= _height)
        h = _height - y;
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    for (y = h; y > 0; y--) {
        for (x = w;x > 1;x--) {
            write_data16(color);
        }
    }
}

void invertDisplay(uint8_t i) {
    write_command(i ? CMD_DINVON : CMD_DINVOF);
}
/////////////////////
#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define abs(x) (x > 0 ? x : 0 - x)

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t dx, dy;
    int16_t ystep, err;
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }


    dx = x1 - x0;
    dy = abs(y1 - y0);

    err = dx / 2;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y + h - 1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x + w - 1, y, h, color);
}

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0  , y0+r, color);
    drawPixel(x0  , y0-r, color);
    drawPixel(x0+r, y0  , color);
    drawPixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
  
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        } 
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
        }
    }
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;
    int16_t dx01, dy01, dx02, dy02, dx12, dy12;
    int32_t sa, sb;
    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }
    if (y1 > y2) {
        swap(y2, y1); swap(x2, x1);
    }
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawFastHLine(a, y0, b-a+1, color);
        return;
    }

    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;
    sa = 0;
    sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
           a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
           b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) swap(a,b);
        drawFastHLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
           a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
           b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) swap(a,b);
        drawFastHLine(a, y, b-a+1, color);
    }
}

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    drawFastHLine(x+r  , y    , w-2*r, color); // Top
    drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawFastVLine(x    , y+r  , h-2*r, color); // Left
    drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    fillRect(x+r, y, w-2*r, h, color);

    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    int16_t i, j, byteWidth = (w + 7) / 8;

    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                drawPixel(x+i, y+j, color);
            }
        }
    }
}

void drawBitmapBg(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
    int16_t i, j, byteWidth = (w + 7) / 8;
  
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                drawPixel(x+i, y+j, color);
            }
            else {
                drawPixel(x+i, y+j, bg);
            }
        }
    }
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    int16_t i, j, byteWidth = (w + 7) / 8;
  
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
                drawPixel(x+i, y+j, color);
            }
        }
    }
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
    int8_t i, j;
    if((x >= _width)            || // Clip right
       (y >= _height)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left
       ((y + 8 * size - 1) < 0))   // Clip top
        return;

    for (i=0; i<6; i++ ) {
        uint8_t line;
        if (i == 5) 
            line = 0x0;
        else 
            line = pgm_read_byte(font+(c*5)+i);
        for (j = 0; j<8; j++) {
            if (line & 0x1) {
                if (size == 1) // default size
                    drawPixel(x+i, y+j, color);
                else {  // big size
                    fillRect(x+(i*size), y+(j*size), size, size, color);
                } 
            } else if (bg != color) {
                if (size == 1) // default size
                    drawPixel(x+i, y+j, bg);
                else {  // big size
                    fillRect(x+i*size, y+j*size, size, size, bg);
                }
            }
            line >>= 1;
        }
    }
}

void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

void setTextColor(uint16_t c) {
    textcolor = textbgcolor = c;
}

void setTextColorBg(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
}

void setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}

void setTextWrap(uint8_t w) {
    wrap = w;
}

void write(uint8_t c) {
    if (c == '\n') {
        cursor_y += textsize * 8;
        cursor_x  = 0;
    } else if (c == '\r') {
        // skip em
    } else {
        drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        cursor_x += textsize*6;
        if (wrap && (cursor_x > (_width - textsize*6))) {
            cursor_y += textsize*8;
            cursor_x = 0;
        }
    }
}

int16_t height(void) {
    return _height;
}

int16_t width(void) {
    return _width;
}

uint8_t getRotation(void) {
    return rotation;
}

uint8_t write_string(char* str, uint8_t len) {
    uint8_t i;
    for (i = 0; i < len; i++)
        write(str[i]);
    return i;
}
