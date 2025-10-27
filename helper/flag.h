#ifndef __FLAG_H__
#define __FLAG_H__

#include "helper.h"

#include <stdio.h>      /// for printf, sprintf, etc.
#include <stdint.h>     /// for fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdarg.h>     /// for variadic arguments (va_list, va_start, va_end)

/*
NOTE: By default, 1-bit is know as active state or trigger state, and 0-bit is known as ignore state or non-trigger state.
        This convention is used in most of the flag operations and macros defined in this file.
        If you want to use the opposite convention (i.e., 0-bit as active state and 1-bit as ignore state), you need to manually 
        invert the flag bits before using the macros.
*/

/// Type of a flag
typedef uint32_t        flag_t;

#define __flagMask(i)             __mask32(i)                         /// Mask 1 bit in uint32_t.
#define __flagInvMask(i)          __inv_mask32(i)                     /// Inverted mask uint32_t.
#define __setFlagBit(flag, i)     ((flag) |= __mask32(i))             /// Set bit at position i in 32-bit flag.
#define __clearFlagBit(flag, i)   ((flag) &= __inv_mask32(i))         /// Clear bit at position i in 32-bit flag.
#define __toggleFlagBit(flag, i)  ((flag) ^= __mask32(i))             /// Toggle bit at position i in 32-bit flag.
#define __hasFlagBitSet(flag, i)   (((flag) & __mask32(i)) != 0U)     /// Check if bit at position i is set in 32-bit flag (nonzero if true).
#define __hasFlagBitClr(flag, i) (((flag) & __mask32(i)) == 0U)       /// Check if bit at position i is clear in 32-bit flag (nonzero if true).

/// Flag operation with MUTEX! 
#define FLAG_OP_W_MUTEX(p2mutex, flagOp, flag, bitOrder)        \
        do {                                                    \
            vPortEnterCritical(p2mutex);                        \
            flagOp(flag, bitOrder);                             \
            vPortExitCritical(p2mutex);                         \
        } while (0)

/// Wrap a piece of code in a MUTEX!
#define PERFORM_IN_CRITICAL(p_mux, ...)                         \
        do {                                                    \
            vPortEnterCritical(p_mux);                          \
            __VA_OPT__(__VA_ARGS__)                             \
            vPortExitCritical(p_mux);                           \
        } while (0)

#define NONE_FLAG_SET                0x00000000U                     /// No flag is set (all bits are 0).
#define ALL_FLAG_SET                 0xFFFFFFFFU                     /// All flags are set (all bits are

#endif