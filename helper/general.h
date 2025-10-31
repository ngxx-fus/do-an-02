/// @file general.h

/// @brief Common system-wide header providing core includes and helper dependencies.
/// This header centralizes standard C library includes and project-wide helper headers.
/// It is intended to be included by most modules (e.g., I2C, SPI, OLED) to ensure
/// consistent access to core types, logging, GPIO, and RTOS wrappers.

/// @note This file should be included **before** any hardware driver headers to ensure all 
/// shared types and macros are available.

/// @note This header CAN NOT BE INCLUDED IN ANY .H file in this directory. Because it will make 
/// an infinity include loop!

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
#include <math.h>

#include "../config/pinConfig.h"
#include "../fonts/fontUtils.h"
#include "flag.h"
#include "param.h"
#include "return.h"
#include "espGPIOWrapper.h"
#include "espLogWrapper.h"
#include "espRTOSWrapper.h"
#include "esp_heap_caps.h"

#endif