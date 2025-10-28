#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__

#include "generalConfig.h"

#include <stdio.h>      /// for printf, sprintf, etc.
#include <stdint.h>     /// for fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdarg.h>     /// for variadic arguments (va_list, va_start, va_end)

/// Type of a GPIO
typedef int8_t pin_t;

#if ESP32_DEVICE_MODE == SENDER

    #define PIN0    ((pin_t)4)
    #define PIN1    ((pin_t)16)
    #define PIN2    ((pin_t)5)
    #define PIN3    ((pin_t)17)

    #define BTN0    ((pin_t)14)

    #define OLED_SDA ((pin_t)21)
    #define OLED_SCL ((pin_t)22)

#endif 

#if ESP32_DEVICE_MODE == RECEIVER

    #define PIN0    ((pin_t)0)
    #define PIN1    ((pin_t)0)
    #define PIN2    ((pin_t)0)
    #define PIN3    ((pin_t)0)

    #define BTN0    ((pin_t)0)

    #define OLED_SCL ((pin_t)0)
    #define OLED_SDA ((pin_t)0)

#endif 

#if ESP32_DEVICE_MODE == MONITOR
    /// LCD 3.2" PIN CONFIGURATION
    /// NOTE: Only accepts GPIO which GPIO number less than 32

    #define LCD32_DB0       18    
    #define LCD32_DB1       12
    #define LCD32_DB2       17
    #define LCD32_DB3       11
    #define LCD32_DB4       16
    #define LCD32_DB5       10
    #define LCD32_DB6       15
    #define LCD32_DB7       9
    #define LCD32_DB8       7
    #define LCD32_DB9       3
    #define LCD32_DB10      6
    #define LCD32_DB11      20
    #define LCD32_DB12      5
    #define LCD32_DB13      19
    #define LCD32_DB14      4
    #define LCD32_DB15      8


    #define LCD32_RST       0
    #define LCD32_WR        2
    #define LCD32_RD        14
    #define LCD32_RS        21
    #define LCD32_CS        13
    #define LCD32_BL        1

#endif


/// @brief States of pin
enum PIN_STATE {
    UNUSED = -1,
    INVALID = -2,
    NOT_FOUND = -3
};

#endif /* __PIN_CONFIG_H__ */
