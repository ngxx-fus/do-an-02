#ifndef __FLAG_H__
#define __FLAG_H__

#include "helper.h"

#include <stdio.h>      /// for printf, sprintf, etc.
#include <stdint.h>     /// for fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdarg.h>     /// for variadic arguments (va_list, va_start, va_end)

/// Type of a flag
typedef uint32_t        flag_t;

#define __flagMask(i)             __mask32(i)                         /// Mask 1 bit in uint32_t.
#define __flagInvMask(i)          __inv_mask32(i)                     /// Inverted mask uint32_t.
#define __setFlagBit(flag, i)     ((flag) |= __mask32(i))             /// Set bit at position i in 32-bit flag.
#define __clearFlagBit(flag, i)   ((flag) &= __inv_mask32(i))         /// Clear bit at position i in 32-bit flag.
#define __toggleFlagBit(flag, i)  ((flag) ^= __mask32(i))             /// Toggle bit at position i in 32-bit flag.
#define __hasFlagBitSet(flag, i)   (((flag) & __mask32(i)) != 0U)      /// Check if bit at position i is set in 32-bit flag (nonzero if true).
#define __hasFlagBitClr(flag, i) (((flag) & __mask32(i)) == 0U)      /// Check if bit at position i is clear in 32-bit flag (nonzero if true).

#endif