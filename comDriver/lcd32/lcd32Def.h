/// @file   lcd32Def.h
/// @brief  LCD 3.2" type definitions; Includes color/dimension types, canvas, data/control pin structs, and device structure.

#ifndef __LCD32_DEF_H__
#define __LCD32_DEF_H__

#include "../../helper/general.h"

typedef uint16_t    color_t;        /// 16-bit color for a pixel
typedef int16_t     dim_t;          /// 16-bit signed for dimension

/// @cond
/// CANVAS ////////////////////////////////////////////////////////////////////////////////////////
/// @endcond

typedef struct lcd32Canvas_t{       /// Struct for LCD 3.2" canvas
    color_t **buff;                 /// 1D array to store buffer
    dim_t   maxRow, maxCol;         /// Dim info
} lcd32Canvas_t;

/// @cond
/// CONTROL/DATA PIN //////////////////////////////////////////////////////////////////////////////
/// @endcond

typedef struct lcd32DataPin_t{       /// Struct for data pin
    /// 16-bit data pin
    pin_t       __0, __1, __2,  __3,  __4,  __5,  __6,  __7, 
                __8, __9, __10, __11, __12, __13, __14, __15;
} lcd32DataPin_t;

typedef struct lcd32ControlPin_t{   /// Struct for data pint
    pin_t       r;                      /// Read 
    pin_t       w;                      /// Write
    pin_t       cs;                     /// Chip select
    pin_t       rs;                     /// Register select
    pin_t       rst;                    /// Reset
    pin_t       bl;                     /// Brightness light
} lcd32ControlPin_t;

/// @cond
/// LCD DEVICE ////////////////////////////////////////////////////////////////////////////////////
/// @endcond

typedef struct lcd32Dev_t{          /// Struct for LCD 3.2"

    lcd32DataPin_t      dataPin;    /// Data pin
    lcd32ControlPin_t   controlPin; /// Control pin
    lcd32Canvas_t       canvas;     /// Canvas           
    portMUX_TYPE        mutex;      /// Mutex
    uint32_t            transLevel; /// Transaction level, to skill dup CS
    uint64_t            dataPinMask;
    uint64_t            controlPinMask;
} lcd32Dev_t;


#endif