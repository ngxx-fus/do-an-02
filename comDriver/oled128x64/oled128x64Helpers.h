#ifndef __OLED128x64_HELPERS_H__
#define __OLED128x64_HELPERS_H__

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








#endif