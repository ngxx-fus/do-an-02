#include "oled128x64Commands.h"
#include "oled128x64Helpers.h"
#include "oled128x64Log.h"
#include "oled128x64Definitions.h"

#pragma message("oled128x64.c is compiling...")

def createOLEDDevice(oled128x64Dev_t ** oled){
    __oledEntry("createOLEDDevice(%p)", oled);

    if(__is_null(oled)) goto ReturnERR_NULL;

    (*oled) = (oled128x64Dev_t*) malloc(sizeof(oled128x64Dev_t));
    if(__is_null(*oled)) goto ReturnERR_NULL;
    
    (*oled)->height = __oled_h;
    (*oled)->width  = __oled_w;

    (*oled)->oled_canvas = (cell_t*) malloc(sizeof(cell_t) * ((*oled)->width) * ((*oled)->height));
    if(__is_null((*oled)->oled_canvas)) goto ReturnERR_NULL;
    
    if( createI2CDevice(&((*oled)->i2cDev)) != OKE ) goto ReturnERR;
    
    __oledExit("createOLEDDevice() : %s", STR(OKE));
    return OKE;

    ReturnERR:
        __oledExit("createOLEDDevice() : %s", STR(ERR));
        return ERR;

    ReturnERR_NULL:
        __oledExit("createOLEDDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
        
    // ReturnERR_MALLOC_FAILED:
    //     __oledExit("createOLEDDevice() : %s", STR(ERR_MALLOC_FAILED));
    //     return ERR_MALLOC_FAILED;
}

def configOLEDDevice(oled128x64Dev_t * oled, pin_t scl, pin_t sda){
    __oledEntry("configOLEDDevice(%p)", oled);

    if(__is_null(oled)) goto configOLED_ReturnERR_NULL;
    if(__is_null(oled->i2cDev)) goto configOLED_ReturnERR_NULL;

    /// Config SCL, SDA, as MASTER mode + FAST mode speed 
    if( configI2CDevice(oled->i2cDev, OLED_ADDR, scl, sda, I2C_FAST_MODE) != OKE)
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




// I2CCommunication* oled;

// cell_t oled_canvas[1024]= {};

// void oled_init() {
//     oled = createNewI2CCommunication(OLED_SDA, OLED_SCL);
//     uint8_t init_commands[] = {
//         SSD1306_COMMAND,
//         SSD1306_DISPLAY_OFF,
//         SSD1306_SET_DISPLAY_CLOCK_DIV,      0x80,
//         SSD1306_SET_MULTIPLEX_RATIO,        0x3F,
//         SSD1306_SET_DISPLAY_OFFSET,         0x00,
//         SSD1306_SET_START_LINE(0),
//         SSD1306_CHARGE_PUMP,                0x14,
//         SSD1306_SET_MEMORY_MODE,            0x00,
//         SSD1306_SET_COM_PINS,               0x12,
//         SSD1306_SET_CONTRAST,               0xCF,
//         SSD1306_SET_PRECHARGE,              0xF1,
//         SSD1306_SET_VCOM_DETECT,            0x40,
//         // SSD1306_INVERT_DISPLAY,
//         SSD1306_NORMAL_DISPLAY,
//         SSD1306_ENTIRE_DISPLAY_ON
//     };
//     i2c_send_byte_array(oled, OLED_ADDR, init_commands, sizeof(init_commands)/sizeof(uint8_t));
// }

// void oled_turn_on_all_pixels(){
//     i2c_send_word(oled, OLED_ADDR, SSD1306_COMMAND, SSD1306_DISPLAY_ON);
//     i2c_send_word(oled, OLED_ADDR, SSD1306_COMMAND, SSD1306_ENTIRE_DISPLAY_ON);
// }

// void oled_show_RAM_content(){
//     i2c_send_word(oled, OLED_ADDR, SSD1306_COMMAND, SSD1306_DISPLAY_ON);
//     i2c_send_word(oled, OLED_ADDR, SSD1306_COMMAND, SSD1306_ENTIRE_DISPLAY_RESUME);
// }

// void oled_on(){
//     i2c_send_word(oled, OLED_ADDR, SSD1306_COMMAND, SSD1306_DISPLAY_ON);
// }

// void oled_off(){
//     i2c_send_word(oled, OLED_ADDR, SSD1306_COMMAND, SSD1306_DISPLAY_OFF);
// }

// void oled_flush(){
//     uint8_t cmds[] = {
//         SSD1306_INVERT_DISPLAY,
//         SSD1306_ENTIRE_DISPLAY_RESUME,
//         // SSD1306_DISPLAY_ON
//     };
//     i2c_send_byte_array_w_ctl_byte(oled, OLED_ADDR, SSD1306_COMMAND, cmds, sizeof(cmds));
//     i2c_send_byte_array_w_ctl_byte(oled, OLED_ADDR, SSD1306_DATA, oled_canvas, sizeof(oled_canvas));
// }

// void oled_canvas_fill(color_t cl){
//     REPT(xy_t, i, 0, 1024) oled_canvas[i] = (cl)?(0xFF):0x0;
// }

// void oled_flush_area(uint8_t seg_start, uint8_t seg_end, uint8_t col_start, uint8_t col_end) {
//     // if (seg_end > 7) seg_end = 7;
//     // if (col_end > 127) col_end = 127;

//     // for (uint8_t page = seg_start; page <= seg_end; page++) {
        
//     //     uint8_t cmd_page[3] = {
//     //         (uint8_t)(0xB0 | page),
//     //         (uint8_t)(0x00 | (col_start & 0x0F)),
//     //         (uint8_t)(0x10 | (col_start >> 4))
//     //     };
//     //     i2c_send_byte_array_w_ctl_byte(oled, OLED_ADDR, SSD1306_COMMAND, cmd_page, 3);

//     //     uint16_t offset = page * 128 + col_start;
//     //     uint16_t len = col_end - col_start + 1;

//     //     i2c_send_byte_array_w_ctl_byte(oled, OLED_ADDR, SSD1306_DATA, &oled_canvas[offset], len);
//     // }
// }

// void  oled_draw_empty_rect(xy_t xTL, xy_t yTL, xy_t xBR, xy_t yBR, xy_t edge_size, color_t color){
//     if(edge_size < 1)           return;
//     if(!__xy_check(xTL, yTL))   return;
//     if(xTL > xBR || yTL > yBR)  return;
//     xBR = __min(xBR, __max_x);  
//     yBR = __min(yBR, __max_y);
//     __rep(xy_t, x, xTL, xBR+1){
//         __write_at(x, yTL, color);        
//         __write_at(x, yBR, color);        
//     }
//     __rep(xy_t, y, yTL, yBR+1){
//         __write_at(xTL, y, color);        
//         __write_at(xBR, y, color);        
//     }
//     oled_draw_empty_rect(xTL+1,yTL+1,xBR-1, yBR-1, edge_size-1, color);
// }

// void oled_draw_char(xy_t x, xy_t y, char c, color_t cl) {
//     // __entry("oled_draw_char(%d, %d, %c, %d)", x, y, c, cl);
//     if (c < 0x20 || c > 0x7E) return;

//     GFXglyph glyph = theFont.glyph[c - 0x20];

//     uint16_t bo = glyph.bitmapOffset;
//     xy_t     w  = glyph.width;
//     xy_t     h  = glyph.height;
//     xy_t     xo = glyph.xOffset;
//     xy_t     yo = glyph.yOffset;

//     xy_t bitmap_top_x = x + yo;
//     xy_t bitmap_left_y = y + xo;
//     byte_t bits = 0;
//     byte_t bitCount = 0;

//     REPT(xy_t, yy, 0, h) {
//         REPT(xy_t, xx, 0, w) {
//             if (bitCount == 0) {
//                 bits = theFont.bitmap[bo++];
//                 bitCount = 8;
//             }
//             if (bits & 0x80) {
//                 xy_t pixel_x = bitmap_top_x + yy;
//                 xy_t pixel_y = bitmap_left_y + xx;

//                 if (pixel_x >= __min_x && pixel_x < screenH &&
//                     pixel_y >= __min_y && pixel_y < screenW) {
//                     __write_at(pixel_x, pixel_y, cl);
//                 }
//             }
//             bits <<= 1;
//             bitCount--;
//         }
//     }
// }

// void oled_draw_text(xy_t x, xy_t y, const char* str, color_t cl, bool wrap, byte_t lineHeight) {
//     // __entry("oled_draw_char(%d, %d, %s, %d, %d, %d)", x, y, str, cl, wrap, lineHeight);
//     if (!str) return;

//     xy_t cursor_x = x;
//     xy_t cursor_y = y;

//     byte_t finalLineHeight = MAX(lineHeight, defaultTextH(theFont));
    
//     while (*str) {
//         char c = *str++;
//         if (c == '\n') {
//             cursor_y = y;
//             cursor_x += finalLineHeight;
//             continue;
//         }
//         if (c < 0x20 || c > 0x7E) continue;
        
//         GFXglyph glyph = theFont.glyph[c - 0x20];

//         if (wrap && (cursor_y + (xy_t)glyph.xAdvance > screenW)) {
//             cursor_y = y;
//             cursor_x += finalLineHeight;
//         }

//         oled_draw_char(cursor_x, cursor_y, c, cl);
        
//         cursor_y += glyph.xAdvance;
//     }
// }

