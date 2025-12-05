/// @file parallel16_def.h
/// @brief Type definitions for a 16-bit parallel interface driver.

#ifndef __PARALLEL16_DEF_H__
#define __PARALLEL16_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "AppConfig/All.h"
#include "AppUtils/All.h"


#define P16_COMMAND_MODE    0       ///< Command mode identifier (for RS Pin_t).
#define P16_DATA_MODE       1       ///< Data mode identifier (for RS Pin_t).
#define P16_CTL_PIN_NUM     5       ///< Total number of control Pin_ts.
#define P16_DATA_PIN_NUM    16      ///< Total number of data Pin_ts (16-bit bus).
#define P16_MUTEX_EN        0

/// @brief Data type for the 16-bit parallel bus.
typedef uint16_t P16Data_t;

/// @brief Data type for the size of array (same with size_t of C)
typedef size_t  P16Size_t;

/// @brief Struct for 16-bit data Pin_ts (individual Pin_t access).
typedef struct __Data16Pin_t{
    Pin_t       __0, __1, __2,  __3,  __4,  __5,  __6,  __7, 
                __8, __9, __10, __11, __12, __13, __14, __15;
} __Data16Pin_t;

/// @brief Union for 16-bit data Pin_ts (array or struct access).
typedef union P16DatPin_t{
    Pin_t           arr[P16_DATA_PIN_NUM];      ///< Access Pin_ts as an array.
    __Data16Pin_t   pin;                      ///< Access Pin_ts as a struct.
} P16DatPin_t;

/// @brief Struct for control Pin_ts (individual Pin_t access).
typedef struct __ctlPin_t{
    Pin_t           r;                      ///< Read signal (RD).
    Pin_t           w;                      ///< Write signal (WR).
    Pin_t           cs;                     ///< Chip Select (CS).
    Pin_t           rs;                     ///< Register Select (RS) or Data/Command (D/C).
    Pin_t           rst;                    ///< Reset signal.
} __ctlPin_t;

/// @brief Union for control Pin_ts (array or struct access).
typedef union P16CtlPin_t{
    Pin_t           arr[P16_CTL_PIN_NUM];   ///< Access Pin_ts as an array.
    __ctlPin_t      pin;                    ///< Access Pin_ts as a struct.
} P16CtlPin_t;

/// @brief Main device structure for a 16-bit parallel interface.
typedef struct parallel16Dev_t{
    P16DatPin_t     DatPin;        ///< Data Pin_t configuration union.
    P16CtlPin_t     CtlPin;        ///< Control Pin_t configuration union.
    Flag_t          DatPinMask[P16_DATA_PIN_NUM+1];     ///< Bitmask for fast data port access.
    Flag_t          CtlPinMask[P16_CTL_PIN_NUM+1];      ///< Bitmask for fast control port access.
    #if (P16_MUTEX_EN == 1)
        portMUX_TYPE        mutex;         /// Mutex for shared driver
    #endif

} P16Dev_t;

#ifdef __cplusplus
}
#endif

#endif // __PARALLEL16_DEF_H__