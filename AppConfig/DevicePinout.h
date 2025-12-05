#ifndef __PINOUT_H__
#define __DEVICE_PINOUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

/// @brief Type of a GPIO
typedef int8_t Pin_t;

/// @brief States of pin
enum PIN_STATE {
    PIN_UNUSED = -1,
    PIN_INVALID = -2,
    PIN_NOT_FOUND = -3
};

#if FIRMWARE_TYPE == TYPE_SENDER

    #define PIN0            ((Pin_t)4)
    #define PIN1            ((Pin_t)16)
    #define PIN2            ((Pin_t)5)
    #define PIN3            ((Pin_t)17)

    #define BTN0            ((Pin_t)14)

    #define OLED_SDA        ((Pin_t)21)
    #define OLED_SCL        ((Pin_t)22)

#endif 

#if FIRMWARE_TYPE == TYPE_RECEIVER

    #define PIN0            PIN_UNUSED
    #define PIN1            PIN_UNUSED
    #define PIN2            PIN_UNUSED
    #define PIN3            PIN_UNUSED

    #define BTN0            PIN_UNUSED

    #define OLED_SCL        PIN_UNUSED
    #define OLED_SDA        PIN_UNUSED

#endif 

#if FIRMWARE_TYPE == TYPE_ANALYZER_MASTER

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

#if FIRMWARE_TYPE == TYPE_ANALYZER_READER
    /// Reserved

#endif


#ifdef __cplusplus
}
#endif

#endif /// __DEVICE_PINOUT_H__