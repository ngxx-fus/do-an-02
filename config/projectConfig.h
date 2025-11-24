/// @file   projectConfig.h
/// @brief  Project configurations; Determine the driver type. E.g: SENDER, RECEIVER, MONITOR; Defines GPIO, related values

#ifndef __GENERAL_CONFIG_H__
#define __GENERAL_CONFIG_H__

/// @cond
/// DEVICE SETUP //////////////////////////////////////////////////////////////////////////////////
/// @endcond

#define SENDER              0x0     ///< Device mode: Sender
#define RECEIVER            0x1     ///< Device mode: Receiver
#define MONITOR             0x2     ///< Device mode: Monitor

/// @brief Mode of source code (SENDER, RECEIVER, or MONITOR)
#define ESP32_DEVICE_MODE   MONITOR

/// @note To config font, check fontUtils.c in fonts/. Currently, the font is: TomThumb

/// @cond
/// COMPONENT SETUP ///////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief System log level set-up
/// - [1] : Only error can be logged 
/// - [2] : Error, entry, exit, can be logged 
/// - [3] : Error, entry, exit, message can be logged 
/// - [4] : Detail log
/// @see espLogWrapper.h
#define SYSTEM_LOG_LEVEL 3

//DEPRECATED//  /// To config the com protocol at begin,
//DEPRECATED//  /// See ENUM_SYSTEM_MODE enum in helper/general.h.
//DEPRECATED//  #define STARTUP_COM_MODE 0

/// @brief Slave address for I2C communication
#define SLAVE_ADDRE      0x39

/// @brief Time interval of generating data frame (Unit: tick)
#define DATAFR_INTERVAL  200

/// @brief Debouncing time for button (Unit: micro-sec)
#define DEBOUNCE_TIME   10000

/// @brief Short press threshold (Unit: micro-sec)
#define SHORTPRESS_TIME 500000

/// @brief Long press threshold (Unit: micro-sec)
#define LONGPRESS_TIME 2000000

/// @brief SPI Communication Frequency in Hz
#define COM_SPI_FREQ   100000

/// @brief SPI Configuration Preset (Mode, CPOL, CPHA)
#define COM_SPI_CONF   SPI_11_MASTER

/// @brief Display orientation (0, 1, 2, 3)
#define DISP_ORIENTATION 1   

/// @cond
/// GPIO CONFIG ///////////////////////////////////////////////////////////////////////////////////
/// @endcond

#include <stdio.h>      ///< For printf, sprintf, etc.
#include <stdint.h>     ///< For fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdarg.h>     ///< For variadic arguments (va_list, va_start, va_end)

/// @brief Type definition for a GPIO pin number
typedef int8_t pin_t;

#if (ESP32_DEVICE_MODE == SENDER)

    #define PIN0    ((pin_t)4)   ///< General purpose pin 0
    #define PIN1    ((pin_t)16)  ///< General purpose pin 1
    #define PIN2    ((pin_t)5)   ///< General purpose pin 2
    #define PIN3    ((pin_t)17)  ///< General purpose pin 3

    #define BTN0    ((pin_t)14)  ///< Button 0 input pin

    #define OLED_SDA ((pin_t)21) ///< OLED I2C SDA pin
    #define OLED_SCL ((pin_t)22) ///< OLED I2C SCL pin

#endif /// (ESP32_DEVICE_MODE == SENDER)

#if (ESP32_DEVICE_MODE == RECEIVER)

    #define PIN0    ((pin_t)0)   ///< Unused/Dummy pin
    #define PIN1    ((pin_t)0)   ///< Unused/Dummy pin
    #define PIN2    ((pin_t)0)   ///< Unused/Dummy pin
    #define PIN3    ((pin_t)0)   ///< Unused/Dummy pin

    #define BTN0    ((pin_t)0)   ///< Unused/Dummy pin

    #define OLED_SCL ((pin_t)0)  ///< Unused/Dummy pin
    #define OLED_SDA ((pin_t)0)  ///< Unused/Dummy pin

#endif /// (ESP32_DEVICE_MODE == RECEIVER)

#if ESP32_DEVICE_MODE == MONITOR

    #define LCD32_DB0       18   ///< LCD Data Bit 0
    #define LCD32_DB1       12   ///< LCD Data Bit 1
    #define LCD32_DB2       17   ///< LCD Data Bit 2
    #define LCD32_DB3       11   ///< LCD Data Bit 3
    #define LCD32_DB4       16   ///< LCD Data Bit 4
    #define LCD32_DB5       10   ///< LCD Data Bit 5
    #define LCD32_DB6       15   ///< LCD Data Bit 6
    #define LCD32_DB7       9    ///< LCD Data Bit 7
    #define LCD32_DB8       7    ///< LCD Data Bit 8
    #define LCD32_DB9       3    ///< LCD Data Bit 9
    #define LCD32_DB10      6    ///< LCD Data Bit 10
    #define LCD32_DB11      20   ///< LCD Data Bit 11
    #define LCD32_DB12      5    ///< LCD Data Bit 12
    #define LCD32_DB13      19   ///< LCD Data Bit 13
    #define LCD32_DB14      4    ///< LCD Data Bit 14
    #define LCD32_DB15      8    ///< LCD Data Bit 15


    #define LCD32_RST       0    ///< LCD Reset pin
    #define LCD32_WR        2    ///< LCD Write control pin
    #define LCD32_RD        14   ///< LCD Read control pin
    #define LCD32_RS        21   ///< LCD Register Select pin (Command/Data)
    #define LCD32_CS        13   ///< LCD Chip Select pin
    
    #define LCD32_MAX_ROW   240  ///< Maximum rows (height) of LCD
    #define LCD32_MAX_COL   320  ///< Maximum columns (width) of LCD

#endif /// (ESP32_DEVICE_MODE == MONITOR)

/// @brief States of pin definition
enum PIN_STATE {
    UNUSED = -1,    ///< Pin is unused
    INVALID = -2,   ///< Pin value is invalid
};

#endif