#ifndef __PARALLEL16_DEF_H__
#define __PARALLEL16_DEF_H__

/// @file parallel16_def.h
/// @brief Type definitions for a 16-bit parallel interface driver.

#define P16_COMMAND_MODE    0       ///< Command mode identifier (for RS pin).
#define P16_DATA_MODE       1       ///< Data mode identifier (for RS pin).
#define P16_CTL_PIN_NUM     5       ///< Total number of control pins.
#define P16_DATA_PIN_NUM    16      ///< Total number of data pins (16-bit bus).

/// @brief Data type for the 16-bit parallel bus.
typedef uint16_t p16Data_t;

/// @brief Data type for the size of array (same with size_t of C)
typedef size_t  p16Size_t;

/// @brief Struct for 16-bit data pins (individual pin access).
typedef struct __data16Pin_t{
    pin_t       __0, __1, __2,  __3,  __4,  __5,  __6,  __7, 
                __8, __9, __10, __11, __12, __13, __14, __15;
} __data16Pin_t;

/// @brief Union for 16-bit data pins (array or struct access).
typedef union p16DataPin_t{
    pin_t           arr[P16_DATA_PIN_NUM];  ///< Access pins as an array.
    __data16Pin_t   pin;                    ///< Access pins as a struct.
} p16DataPin_t;

/// @brief Struct for control pins (individual pin access).
typedef struct __ctlPin_t{
    pin_t       r;                      ///< Read signal (RD).
    pin_t       w;                      ///< Write signal (WR).
    pin_t       cs;                     ///< Chip Select (CS).
    pin_t       rs;                     ///< Register Select (RS) or Data/Command (D/C).
    pin_t       rst;                    ///< Reset signal.
} __ctlPin_t;

/// @brief Union for control pins (array or struct access).
typedef union p16ControlPin_t{
    pin_t       arr[P16_CTL_PIN_NUM];   ///< Access pins as an array.
    __ctlPin_t pin;                     ///< Access pins as a struct.
} p16ControlPin_t;

/// @brief Main device structure for a 16-bit parallel interface.
typedef struct parallel16Dev_t{
    p16DataPin_t        dataPin;        ///< Data pin configuration union.
    p16ControlPin_t     controlPin;     ///< Control pin configuration union.
    flag_t              dataPinMask[P16_DATA_PIN_NUM+1];    ///< Bitmask for fast data port access.
    flag_t              controlPinMask[P16_CTL_PIN_NUM+1];  ///< Bitmask for fast control port access.
    portMUX_TYPE        mutex;          /// Mutex for shared driver

} parallel16Dev_t;

#endif // __PARALLEL16_DEF_H__