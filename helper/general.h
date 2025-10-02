#ifndef __GENERAL_HEADER_H__
#define __GENERAL_HEADER_H__

#include <stdio.h>              /// for printf-like formatting
#include <stdarg.h>             /// for variadic macros
#include <stdint.h>             /// fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdlib.h>             /// malloc...
#include <stdbool.h>            /// boolean type (true/false)
#include <stdarg.h>             /// variable arguments (va_list, va_start, va_end)
#include <string.h>             /// memset, strlen

#include "../config/pinConfig.h"
#include "flag.h"
#include "param.h"
#include "return.h"
#include "espGPIOWrapper.h"
#include "espLogWrapper.h"
#include "espRTOSWrapper.h"

#endif