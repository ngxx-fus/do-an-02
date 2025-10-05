#ifndef __OLED128x64_DEFINITIONS_H__
#define __OLED128x64_DEFINITIONS_H__

#include "../i2c/i2c.h"

#define __max_x                 63
#define __max_y                 127
#define __min_x                 0
#define __min_y                 0
#define __oled_h                64
#define __oled_w                128

#ifndef OLED_ADDR
    #define OLED_ADDR 0x3C
#endif 

typedef uint8_t byte_t;
typedef uint8_t cell_t;
typedef uint8_t color_t;
typedef int16_t xy_t;

extern cell_t oled_canvas[1024];

typedef struct oled128x64Dev_t {
    i2cDev_t *  i2cDev;
    cell_t *    oled_canvas;
    xy_t        height, width;
}oled128x64Dev_t;








#endif