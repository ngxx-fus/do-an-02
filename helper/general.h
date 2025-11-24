/// @file general.h
/// @brief Common system-wide header providing core includes and helper dependencies.
/// This header centralizes standard C library includes and project-wide helper headers.
/// It is intended to be included by most modules (e.g., I2C, SPI, OLED) to ensure
/// consistent access to core types, logging, GPIO, and RTOS wrappers.
/// @brief This header includes all helper functions, macros, and standard libraries required by the project.
///
/// @note This file should be included **before** any hardware driver headers to ensure all 
/// shared types and macros are available.
///
/// @author Ngxx-Fus

#ifndef __GENERAL_HEADER_H__
#define __GENERAL_HEADER_H__

/// HEADERS ///////////////////////////////////////////////////////////////////////////////////////


/* Standard C Libraries */
#include <stdio.h>              /// Standard input/output definitions
#include <stdarg.h>             /// Macros for handling variable arguments
#include <stdint.h>             /// Standard fixed-width integer types
#include <stdlib.h>             /// Standard library definitions (malloc, free, etc.)
#include <stdbool.h>            /// Boolean type definitions (true, false)
#include <string.h>             /// String manipulation functions (memset, memcpy, etc.)

/* Project Helper Headers */
#include "../fonts/fontUtils.h" /// Font rendering utilities
#include "flag.h"               /// System flag definitions
#include "param.h"              /// System parameter definitions
#include "return.h"             /// Standard return codes and status definitions

/* ESP-IDF Wrapper Headers */
#include "espGPIOWrapper.h"     /// GPIO hardware abstraction wrapper
#include "espLogWrapper.h"      /// Custom system logging wrapper
#include "espRTOSWrapper.h"     /// FreeRTOS wrapper functions
#include "esp_heap_caps.h"      /// ESP32 heap memory capabilities management

#endif