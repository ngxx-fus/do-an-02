/// @file general.h

/// @brief Common system-wide header providing core includes and helper dependencies.
/// This header centralizes standard C library includes and project-wide helper headers.
/// It is intended to be included by most modules (e.g., I2C, SPI, OLED) to ensure
/// consistent access to core types, logging, GPIO, and RTOS wrappers.

/// @note This file should be included **before** any hardware driver headers to ensure all 
/// shared types and macros are available.

/// @author Ngxx-Fus

#ifndef __GENERAL_HEADER_H__
#define __GENERAL_HEADER_H__

/// HEADERS ///////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>              /// for printf-like formatting
#include <stdarg.h>             /// for variadic macros
#include <stdint.h>             /// fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdlib.h>             /// malloc...
#include <stdbool.h>            /// boolean type (true/false)
#include <stdarg.h>             /// variable arguments (va_list, va_start, va_end)
#include <string.h>             /// memset, strlen

#include "../config/pinConfig.h"
#include "../fonts/fontUtils.h"
#include "flag.h"
#include "param.h"
#include "return.h"
#include "espGPIOWrapper.h"
#include "espLogWrapper.h"
#include "espRTOSWrapper.h"

/// DEFINITIONS ///////////////////////////////////////////////////////////////////////////////////

/// @brief Global system stage flag used to represent the overall runtime state.
/// @note Access to this variable must be protected by `systemStageMutex`.
extern volatile flag_t  systemStage;

/// @brief Mutex for synchronizing access to the system stage flag.
extern portMUX_TYPE     systemStageMutex;

/// @brief Enumeration defining the system operation stages.
enum ENUM_SYSTEM_STAGE {
    SYSTEM_INIT    = 0, /// System is initializing.
    SYSTEM_RUNNING = 1, /// System is currently running.
    SYSTEM_STOPPED = 2, /// System has been stopped or halted.
    SYSTEM_STAGE_COUNT,
};

/// @brief OLED control flag for requesting display operations.
/// @note Access to this variable must be protected by `oledFlagMutex`.
extern volatile flag_t  oledFlag;

/// @brief Mutex for synchronizing access to the OLED flag.
extern portMUX_TYPE     oledFlagMutex;

/// @brief Enumeration defining OLED operation request types.
enum ENUM_OLED_FLAG {
    OLED_REQ_FLUSH       = 0, /// Request to flush (update) the OLED display.
    OLED_REQ_CLR_CANVAS  = 1, /// Request to clear the OLED display canvas.
    OLED_REQ_COUNT,
};

/// @brief Global system mode flag representing active communication mode.
/// @note Access to this variable must be protected by `systemModeMutex`.
extern volatile flag_t  systemMode;

/// @brief Current protocol mode
extern volatile flag_t  currentSystemMode;

/// @brief Mutex for synchronizing access to the system mode flag.
extern portMUX_TYPE     systemModeMutex;

/// @brief Enumeration defining supported system communication modes.
enum ENUM_SYSTEM_MODE {
    SYSTEM_MODE_SPI    = 0, /// SPI communication mode.
    SYSTEM_MODE_I2C    = 1, /// I2C communication mode.
    SYSTEM_MODE_UART   = 2, /// UART communication mode
    SYSTEM_MODE_1WIRE  = 3, /// 1-Wire communication mode.
    SYSTEM_MODE_COUNT       /// Number of system modes
};

/// @brief Communication object
/// @note It can be assigned as I2C, SPI, ...
extern void* comObject;

/// @brief Contain 8-bit data frame 
extern volatile uint8_t byteData;

extern volatile flag_t  byteDataControlFlag;

extern portMUX_TYPE     byteDataControlMutex;

enum BYTE_DATA_CTRL {
    BYTEDATA_GEN_PLAYPAUSE = 0,         /// 0 - Play | 1 - Pause
    BYTEDATA_GEN_NOW = 0,               /// Set 1 to gen now
    BYTE_DATA_CTRL_NUM
};

#if ESP32_DEVICE_MODE == SENDER

/// @brief Mutex for synchronizing access to the sendControlMutex.
extern portMUX_TYPE     sendControlMutex;

/// @brief Send control flag
extern volatile flag_t  sendControlFlag;

/// @brief Enumeration defining SEND operation request types.
enum SEND_CONTROL_FLAG {
    SENDCTRL_PLAYPAUSE      = 0,        /// 0: Play / 1: Pause
    SENDCTRL_SEND_NOW       = 1,        /// 1: Immediately send data frame
    SENDCTRL_REPEAT_SEND    = 2,        /// 0: Random / 1: Stop random data frame
    SEND_CONTROL_FLAG_NUM
};

#endif  /// END SENDER MODE

#if ESP32_DEVICE_MODE == RECEIVER

/// @brief Received control flag
extern volatile flag_t  rcvdControlFlag;

/// @brief Mutex for synchronizing access to the rcvdControlFlag.
extern portMUX_TYPE     rcvdControlMutex;

/// @brief Enumeration defining SEND operation request types.
enum RECEIVE_CONTROL_FLAG {
    RCVCTRL_HAS_DATA       = 0,        /// 1: Has received a byte data frame
    RCVCTRL_NEW_DATA       = 1,        /// 1: Assign new sendback data, reset buffer
    RECEIVE_CONTROL_FLAG_NUM
};

#endif  /// END RECEIVER MODE


#endif  /// END FILE