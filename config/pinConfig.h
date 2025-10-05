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

enum PIN_STATE {
    UNUSED = -1,
    INVALID = -2,
    NOT_FOUND = -3
};

#endif /* __PIN_CONFIG_H__ */
