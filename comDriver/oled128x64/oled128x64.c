#include "../../helper/helper.h"

#include "oled128x64Commands.h"
#include "oled128x64Helpers.h"
#include "oled128x64Log.h"
#include "oled128x64Definitions.h"
#include "oled128x64.h"


#pragma message("oled128x64.c is compiling...")

def createOLEDDevice(oled128x64Dev_t ** oled){
    __oledEntry("createOLEDDevice(%p)", oled);

    if(__is_null(oled)) goto ReturnERR_NULL;

    (*oled) = (oled128x64Dev_t*) malloc(sizeof(oled128x64Dev_t));
    if(__is_null(*oled)) goto ReturnERR_NULL;
    
    (*oled)->height = __oled_h;
    (*oled)->width  = __oled_w;

    (*oled)->canvas = (cell_t*) malloc(sizeof(cell_t) * ((*oled)->width) * ((*oled)->height) / 8);
    if(__is_null((*oled)->canvas)) goto ReturnERR_MALLOC_FAILED;
    
    if( createI2CDevice(&((*oled)->i2cDev)) != OKE ) goto ReturnERR;
    
    __oledExit("createOLEDDevice() : %s", STR(OKE));
    return OKE;

    ReturnERR:
        __oledExit("createOLEDDevice() : %s", STR(ERR));
        return ERR;

    ReturnERR_NULL:
        __oledExit("createOLEDDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
        
    ReturnERR_MALLOC_FAILED:
        __oledExit("createOLEDDevice() : %s", STR(ERR_MALLOC_FAILED));
        return ERR_MALLOC_FAILED;
}

def configOLEDDevice(oled128x64Dev_t * oled, pin_t scl, pin_t sda, flag_t config){
    __oledEntry("configOLEDDevice(%p, %d, %d, 0x%02x)", oled, scl, sda, config);

    if(__is_null(oled)) goto configOLED_ReturnERR_NULL;
    if(__is_null(oled->i2cDev)) goto configOLED_ReturnERR_NULL;

    /// Config SCL, SDA, as MASTER mode + FAST mode speed 
    if( configI2CDevice(oled->i2cDev, OLED_ADDR, scl, sda, config) != OKE)
        goto configOLED_ReturnERR;

    __oledExit("configOLEDDevice() : %s", STR(OKE));
    return OKE;

    configOLED_ReturnERR:
        __oledExit("configOLEDDevice() : %s", STR(ERR));
        return ERR;

    configOLED_ReturnERR_NULL:
        __oledExit("configOLEDDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
}

def startupOLEDDevice(oled128x64Dev_t * oled){
    __oledEntry("startupOLEDDevice(%p)", oled);

    if(__is_null(oled)) goto startupOLED_ReturnERR_NULL;

    if(startupI2CDevice(oled->i2cDev) != OKE) goto startupOLED_ReturnERR;

    byte_t init_commands[] = {
        SSD1306_COMMAND,
        SSD1306_DISPLAY_OFF,
        SSD1306_SET_DISPLAY_CLOCK_DIV,      0x80,
        SSD1306_SET_MULTIPLEX_RATIO,        0x3F,
        SSD1306_SET_DISPLAY_OFFSET,         0x00,
        SSD1306_SET_START_LINE(0),
        SSD1306_CHARGE_PUMP,                0x14,
        SSD1306_SET_MEMORY_MODE,            0x00,
        SSD1306_SET_COM_PINS,               0x12,
        SSD1306_SET_CONTRAST,               0xCF,
        SSD1306_SET_PRECHARGE,              0xF1,
        SSD1306_SET_VCOM_DETECT,            0x40,
        // SSD1306_INVERT_DISPLAY,
        SSD1306_NORMAL_DISPLAY,
        SSD1306_DISPLAY_ON,
        SSD1306_ENTIRE_DISPLAY_ON
    };

    i2cSetTransmitBuffer(oled->i2cDev, init_commands, sizeof(init_commands));

    __oledLog("Send start-up sequence ...");
    if( i2cSendBuffer(oled->i2cDev, OLED_ADDR, I2C_WRITE_MODE, 
                __flagMask(I2C_RESET_RX_INDEX) | __flagMask(I2C_RESET_TX_INDEX)) != OKE)
        goto startupOLED_ReturnERR;

    __oledExit("startupOLEDDevice() : %s", STR(OKE));
        return OKE;

    startupOLED_ReturnERR:
        __oledExit("startupOLEDDevice() : %s", STR(ERR));
        return ERR;

    startupOLED_ReturnERR_NULL:
        __oledExit("startupOLEDDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
}

def oledResetView(oled128x64Dev_t *oled) {
    __oledEntry("oledResetView(%p)", oled);

    const uint8_t cmds[] = {
        SSD1306_COMMAND,
        // SSD1306_DISPLAY_OFF,
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_SET_MEMORY_MODE, 0x00,
        SSD1306_SET_COLUMN_ADDR, 0x00, 0x7F,
        SSD1306_SET_PAGE_ADDR, 0x00, 0x07,
        SSD1306_SET_START_LINE(0),
        SSD1306_SET_SEG_REMAP_0,
        SSD1306_SET_COM_SCAN_INC,
        SSD1306_SET_DISPLAY_OFFSET, 0x00,
        // SSD1306_NORMAL_DISPLAY,
        SSD1306_ENTIRE_DISPLAY_RESUME,
        SSD1306_DISPLAY_ON
    };

    def ret;

    ret = i2cSetTransmitBuffer(oled->i2cDev, cmds, sizeof(cmds));
    if(ret != OKE) goto oledFlush_ReturnERR;

    ret = i2cSendBuffer(oled->i2cDev, OLED_ADDR, 0, __mask32(I2C_RESET_TX_INDEX));
    if(ret != OKE) goto oledFlush_ReturnERR;
    
    __oledExit("oledResetView() : %s", STR(OKE));
    return OKE;

    oledFlush_ReturnERR:
        __oledExit("oledResetView() : %s", STR(ERR));
        return ERR;

}

def oledTurnOnAllPixels(oled128x64Dev_t * oled){
    __oledEntry("oledTurnOnAllPixels(%p)", oled);

    if(__is_null(oled)) {
        __exit("oledTurnOnAllPixels() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }
    i2cSendDoubleByte(oled->i2cDev, OLED_ADDR, 0, SSD1306_COMMAND, SSD1306_DISPLAY_ON, 0);
    
    __oledExit("oledTurnOnAllPixels() : %s", STR(OKE));
    return OKE;
}

def oledShowRAMContent(oled128x64Dev_t * oled){
    __oledEntry("oledShowRAMContent(%p)", oled);
    if(__is_null(oled)) {
        __oledExit("oledTurnOnAllPixels() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }
    
    def ret;

    ret = i2cSendDoubleByte(oled->i2cDev, OLED_ADDR, 0, SSD1306_COMMAND, SSD1306_DISPLAY_ON, 0);
    if(ret != OKE) goto oledShowRAMContent_ReturnERR;
    
    ret = i2cSendDoubleByte(oled->i2cDev, OLED_ADDR, 0, SSD1306_COMMAND, SSD1306_ENTIRE_DISPLAY_RESUME, 0);
    if(ret != OKE) goto oledShowRAMContent_ReturnERR;
    
    __oledExit("oledTurnOnAllPixels() : %s", STR(OKE));
    return OKE;

    oledShowRAMContent_ReturnERR:
        __oledExit("oledShowRAMContent() : %s", STR(ERR));
        return ERR;
}

def oledFlush(oled128x64Dev_t * oled){
    __oledEntry("oledFlush(%p)", oled);
    uint8_t cmds[] = {
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_SET_MEMORY_MODE, 0x00,
        SSD1306_SET_COLUMN_ADDR, 0x00, 0x7F,
        SSD1306_SET_PAGE_ADDR, 0x00, 0x07,
        SSD1306_SET_START_LINE(0),
        SSD1306_SET_SEG_REMAP_0,
        SSD1306_SET_COM_SCAN_INC,
        SSD1306_SET_DISPLAY_OFFSET, 0x00,
        // SSD1306_INVERT_DISPLAY,
        // SSD1306_ENTIRE_DISPLAY_RESUME,
        // SSD1306_DISPLAY_ON
    };

    def ret;

    ret = i2cSetTransmitBuffer(oled->i2cDev, cmds, sizeof(cmds));
    if(ret != OKE) goto oledFlush_ReturnERR;

    ret = i2cSendBuffer(oled->i2cDev, OLED_ADDR, 0, __mask32(I2C_RESET_TX_INDEX));
    if(ret != OKE) goto oledFlush_ReturnERR;
    
    ret = i2cSetTransmitBuffer(oled->i2cDev, oled->canvas, ((oled->height) * (oled->width) / 8));
    if(ret != OKE) goto oledFlush_ReturnERR;
    
    ret = i2cSendByte(oled->i2cDev, OLED_ADDR, 0, SSD1306_DATA, __mask32(I2C_DIS_STOP_CONDI));
    if(ret != OKE) goto oledFlush_ReturnERR;

    ret = i2cSendBuffer(oled->i2cDev, OLED_ADDR, 0, __mask32(I2C_SKIP_ADDR_FRAME) |  __mask32(I2C_DIS_START_CONDI) | __mask32(I2C_RESET_TX_INDEX));
    if(ret != OKE) goto oledFlush_ReturnERR;
    
    __oledExit("oledFlush() : %s", STR(OKE));
    return OKE;

    oledFlush_ReturnERR:
        __oledExit("oledFlush() : %s", STR(ERR));
        return ERR;
}

def oledOn(oled128x64Dev_t * oled){
    i2cSendDoubleByte(oled->i2cDev, OLED_ADDR, 0, SSD1306_COMMAND, SSD1306_DISPLAY_ON, 0);
    return OKE;
}

def oledOff(oled128x64Dev_t * oled){
    i2cSendDoubleByte(oled->i2cDev, OLED_ADDR, 0, SSD1306_COMMAND, SSD1306_DISPLAY_OFF, 0);
    return OKE;
}

def oledFillScreen(oled128x64Dev_t * oled, color_t color){
    REP(i, 0, (oled->height) * (oled->width) / 8){
        (oled->canvas)[i] = ((color)?(0xFF):(0x0));
    }
    return OKE;
}

def oledSetPixel(oled128x64Dev_t *oled, xy_t row, xy_t col, color_t c) {
    __oledLog1("** oledSetPixel(%p, %d, %d)", oled, row, col);

    // Validate coordinates
    if (row < 0 || row >= oled->height || col < 0 || col >= oled->width)
        return ERR_INVALID_ARG;

    // Each page = 8 rows (0–7)
    xy_t pageIndex = row >> 3;          /// Page index (0–7)
    xy_t bitIndex  = row & 0x7;         /// Bit position in the byte (0–7)

    // Compute byte index in framebuffer
    xy_t canvasIndex = pageIndex * oled->width + col;

    // Write bit in framebuffer
    if (c)
        oled->canvas[canvasIndex] |= __mask8(bitIndex);
    else
        oled->canvas[canvasIndex] &= __inv_mask8(bitIndex);

    return OKE;
}

def oledDrawEmptyRect(oled128x64Dev_t *oled, 
                      xy_t rTopLeft, xy_t cTopLeft,
                      xy_t rBottomRight, xy_t cBottomRight,
                      xy_t edgeSize, color_t color){
    if (edgeSize < 1) return OKE;

    if (rTopLeft < 0 || rTopLeft >= oled->height ||
        cTopLeft < 0 || cTopLeft >= oled->width ||
        rBottomRight < 0 || rBottomRight >= oled->height ||
        cBottomRight < 0 || cBottomRight >= oled->width)
        return ERR_INVALID_ARG;

    if (rTopLeft > rBottomRight || cTopLeft > cBottomRight)
        return ERR_INVALID_ARG;

    if (rBottomRight - rTopLeft <= 1 || cBottomRight - cTopLeft <= 1)
        return OKE;

    // Draw vertical edges
    REP(x, rTopLeft, rBottomRight + 1) {
        oledSetPixel(oled, x, cTopLeft, color);
        oledSetPixel(oled, x, cBottomRight, color);
    }

    // Draw horizontal edges
    REP(y, cTopLeft, cBottomRight + 1) {
        oledSetPixel(oled, rTopLeft, y, color);
        oledSetPixel(oled, rBottomRight, y, color);
    }

    // Recursive shrink
    return oledDrawEmptyRect(oled,
                             rTopLeft + 1, cTopLeft + 1,
                             rBottomRight - 1, cBottomRight - 1,
                             edgeSize - 1, color);
}

def oledDrawChar(oled128x64Dev_t *oled, xy_t r, xy_t y, char c, color_t cl) {
    // __oledEntry("oled_draw_char(%d, %d, %c, %d)", r, y, c, cl);
    if (c < 0x20 || c > 0x7E) {
        __oledExit("oledDrawChar() : ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    GFXglyph glyph = mainFont.glyph[c - 0x20];

    uint16_t bo = glyph.bitmapOffset;
    xy_t     w  = glyph.width;
    xy_t     h  = glyph.height;
    xy_t     xo = glyph.xOffset;
    xy_t     yo = glyph.yOffset;

    xy_t bitmap_top_x = r + yo;
    xy_t bitmap_left_y = y + xo;
    byte_t bits = 0;
    byte_t bitCount = 0;

    REPT(xy_t, yy, 0, h) {
        REPT(xy_t, xx, 0, w) {
            if (bitCount == 0) {
                bits = mainFont.bitmap[bo++];
                bitCount = 8;
            }
            if (bits & 0x80) {
                xy_t pixel_x = bitmap_top_x + yy;
                xy_t pixel_y = bitmap_left_y + xx;

                if (pixel_x >= __min_x && pixel_x < (oled->height) &&
                    pixel_y >= __min_y && pixel_y < (oled->width)) {
                    oledSetPixel(oled, pixel_x, pixel_y, cl);   
                }
            }
            bits <<= 1;
            bitCount--;
        }
    }

    // __oledExit("oledDrawChar() : OKE");
    return OKE;
}

def oledDrawText(oled128x64Dev_t *oled, xy_t r, xy_t c,
                 const char* str, color_t color, bool wrap, byte_t lineHeight)
{
    __oledEntry("oledDrawText(%d, %d, \"%s\", %d, wrap=%d, lh=%d)",
                r, c, str, color, wrap, lineHeight);

    if (!oled || !str)
        return ERR_INVALID_ARG;

    xy_t cursor_r = r;
    xy_t cursor_c = c;
    byte_t finalLineHeight = __max(lineHeight, defaultTextH(mainFont));

    while (*str) {
        char ch = *str++;

        if (ch == '\n') {
            cursor_c = 0;
            cursor_r += finalLineHeight;
            continue;
        }

        if (ch < 0x20 || ch > 0x7E)
            continue;

        GFXglyph glyph = mainFont.glyph[ch - 0x20];

        // Wrap if needed
        if (wrap && (cursor_c + glyph.xAdvance > oled->width)) {
            cursor_c = 0;
            cursor_r += finalLineHeight;
        }

        oledDrawChar(oled, cursor_r, cursor_c, ch, color);
        cursor_c += glyph.xAdvance;

    }

    __oledExit("oledDrawText() : OKE");
    return OKE;
}

void drawLineText(oled128x64Dev_t * oled, const char * text, flag_t conf){
    if(__hasFlagBitClr(conf, 16)) oledResetView(oled);
    if(__hasFlagBitClr(conf, 17)) oledFillScreen(oled, 0);
    if(__hasFlagBitClr(conf, 18)) oledShowRAMContent(oled);
    oledDrawText(oled, ((conf&0xFF)+1)*((conf>>8)&0xFF) + (conf&0xFF)*1, 0, text, 1, 1, ((conf>>8)&0xFF));
    if(__hasFlagBitClr(conf, 19)) oledFlush(oled);
}