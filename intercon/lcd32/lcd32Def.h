#ifndef __LCD32_DEF_H__
#define __LCD32_DEF_H__

#include "../../helper/general.h"

typedef uint16_t    color_t;        /// 16-bit color for a pixel
typedef int16_t     dim_t;          /// 16-bit signed for dimension

/// CANVAS ////////////////////////////////////////////////////////////////////////////////////////

typedef struct lcd32Canvas_t{       /// Struct for LCD 3.2" canvas (only accepts GPIO number less than 32)
    color_t **buff;                 /// 1D array to store buffer
    dim_t   maxRow, maxCol;         /// Dim info
} lcd32Canvas_t;

/// LCD DEVICE ////////////////////////////////////////////////////////////////////////////////////

#include "../parallel16/parallel16.h"

typedef struct lcd32Dev_t{          /// Struct for LCD 3.2"
    pin_t               BLPin;
    parallel16Dev_t     p16;        /// Parallel comunication device
    lcd32Canvas_t       canvas;     /// Canvas           
    portMUX_TYPE        mutex;      /// Mutex
} lcd32Dev_t;

#endif