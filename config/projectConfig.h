#ifndef __GENERAL_CONFIG_H__
#define __GENERAL_CONFIG_H__

/// DEVICE SETUP //////////////////////////////////////////////////////////////////////////////////

#define SENDER              0x0
#define RECEIVER            0x1
#define MONITOR             0x2

/// Mode of source code
#define ESP32_DEVICE_MODE   MONITOR

/// To config font, check fontUtils.c in fonts/
/// Currently, the font is: TomThumb

/// COMPONENT SETUP ///////////////////////////////////////////////////////////////////////////////

/// System log level set-up
/// - [1] : Only error can be logged 
/// - [2] : Error, entry, exit, can be logged 
/// - [3] : Error, entry, exit, message can be logged 
/// - [4] : Detail log
/// See code in espLogWrapper.h
#define SYSTEM_LOG_LEVEL 3

//DEPRECATED//  /// To config the com protocol at begin,
//DEPRECATED//  /// See ENUM_SYSTEM_MODE enum in helper/general.h.
//DEPRECATED//  #define STARTUP_COM_MODE 0

/// Slave address for I2C comunication
#define SLAVE_ADDRE      0x39

/// Time interval of generating data frame
/// Unit: tick 
#define DATAFR_INTERVAL  200

/// Debouncing time for button
/// Unit: micro-sec
#define DEBOUNCE_TIME   10000

/// Short press thresold
/// Unit: micro-sec
#define SHORTPRESS_TIME 500000

/// Long press thresold
/// Unit: micro-sec
#define LONGPRESS_TIME 2000000

#define COM_SPI_FREQ   100000
#define COM_SPI_CONF   SPI_11_MASTER

#define DISP_ORIENTATION 1   /// 0, 1, 2, 3

/// GPIO CONFIG ///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>      /// for printf, sprintf, etc.
#include <stdint.h>     /// for fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdarg.h>     /// for variadic arguments (va_list, va_start, va_end)

/// Type of a GPIO
typedef int8_t pin_t;

#if (ESP32_DEVICE_MODE == SENDER)

    #define PIN0    ((pin_t)4)
    #define PIN1    ((pin_t)16)
    #define PIN2    ((pin_t)5)
    #define PIN3    ((pin_t)17)

    #define BTN0    ((pin_t)14)

    #define OLED_SDA ((pin_t)21)
    #define OLED_SCL ((pin_t)22)

#endif /// (ESP32_DEVICE_MODE == SENDER)

#if (ESP32_DEVICE_MODE == RECEIVER)

    #define PIN0    ((pin_t)0)
    #define PIN1    ((pin_t)0)
    #define PIN2    ((pin_t)0)
    #define PIN3    ((pin_t)0)

    #define BTN0    ((pin_t)0)

    #define OLED_SCL ((pin_t)0)
    #define OLED_SDA ((pin_t)0)

#endif /// (ESP32_DEVICE_MODE == RECEIVER)

#if ESP32_DEVICE_MODE == MONITOR

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
    
    #define LCD32_MAX_ROW   240
    #define LCD32_MAX_COL   320

#endif /// (ESP32_DEVICE_MODE == MONITOR)

/// @brief States of pin
enum PIN_STATE {
    UNUSED = -1,
    INVALID = -2,
};

#endif