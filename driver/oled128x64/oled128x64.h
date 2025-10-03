#ifndef __OLED128X64_H__
#define __OLED128X64_H__

#include "i2c_com.h"
#include "oled128x64_cmds.h"
#include "../text_utils/font_utils.h"

#ifndef OLED128X64_ADDR
    #define OLED128X64_ADDR 0x3C
#endif 

/*
      0 1 2  y    127
    0 ###############
    1 ###############
    x #######p#######
    7 ###############

    For 3D buffer:
        p(x, y) <-> frame[pgID][clID][rowID]
    For 1D buffer:
        + Byte access:      __byte_at(x, y): (interget(x/8)*127 + y)
        + Read operation:   __read_at(x, y):  (__byte_at(x, y) & (1<<(x%8)))
        + Write 1-bit operation: __write_1_at(x, y):  (__byte_at(x, y) |= (1<<(x%8)))
        + Write 0-bit operation: __write_0_at(x, y):  (__byte_at(x, y) &= (~(1<<(x%8))))
        + Write op: __write_at(x, y, bit): ((bit)?__write_1_at(x, y):__write_0_at(x, y))

*/

#define __max_x                 63
#define __max_y                 127
#define __min_x                 0
#define __min_y                 0

#ifndef __min
    #define __min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef __max
    #define __max(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef __rep
#define __rep(type, var, start, end) for(type var = (start); var < (end); ++var)
#endif
#ifndef __rev
#define __rev(type, var, start, end) for(type var = (start); var > (end); --var)
#endif

#define __frame_buffer          oled_canvas
#define __mask(i)               (1<<(i))
#define __inv_mask(i)           (~__mask(i))
#define __page_of(x)            ((x) >> 3)
#define __bit_order(x)          ((x) & 0x7)
#define __byte_at(x, y)         __frame_buffer[__page_of(x)*128 + y]
#define __read_at(x, y)         (__byte_at(x, y) & __mask(__bit_order(x)))
#define __write_1_at(x, y)      (__byte_at(x, y) |= __mask(__bit_order(x)))
#define __write_0_at(x, y)      (__byte_at(x, y) &= __inv_mask(__bit_order(x)))
#define __write_at(x, y, bit)   ((bit)?__write_1_at(x, y):__write_0_at(x, y))

#define __xy_check(x, y)        (((x) >= __min_x) && ((x) <= __max_x) && \
                                ((y) >= __min_y) && ((y) <= __max_y))
extern cell_t oled_canvas[1024];

void oled_init();
void oled_turn_on_all_pixels();
void oled_show_RAM_content();
void oled_on();
void oled_off();
void oled_flush();
void oled_canvas_fill(color_t cl);
void oled_flush_area(uint8_t seg_start, uint8_t seg_end, uint8_t col_start, uint8_t col_end);
void oled_draw_empty_rect(xy_t xTL, xy_t yTL, xy_t xBR, xy_t yBR, xy_t edge_size, color_t color);

void oled_draw_char(xy_t x, xy_t y, char c, color_t cl);
void oled_draw_text(xy_t x, xy_t y, const char* str, color_t cl, bool wrap, byte_t lineHeight);

#endif