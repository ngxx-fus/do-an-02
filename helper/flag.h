#ifndef __FLAG_H__
#define __FLAG_H__

#include "helper.h"             /// Custom helper macros (assumed to contain __mask32)

#include <stdio.h>              /// Standard input/output definitions
#include <stdint.h>             /// Fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdarg.h>             /// Variable arguments handling
#include <stdbool.h>            /// Boolean type definitions
#include <stdatomic.h>          /// Standard atomic operations for thread safety

/* --- UNSAFE FLAGS (Fast, Single Thread) --- */

/// @brief Standard 32-bit flag type for non-atomic bitwise operations
typedef uint32_t        flag_t;

#define __flagMask(i)           __mask32(i)                         /// Generate a 32-bit mask with the i-th bit set.
#define __flagInvMask(i)        __inv_mask32(i)                     /// Generate a 32-bit mask with the i-th bit cleared.

#define __setFlagBit(flag, i)   ((flag) |= __mask32(i))             /// Set the bit at position i in a 32-bit flag (Unsafe).
#define __clearFlagBit(flag, i) ((flag) &= __inv_mask32(i))         /// Clear the bit at position i in a 32-bit flag (Unsafe).
#define __toggleFlagBit(flag, i)((flag) ^= __mask32(i))             /// Toggle the bit at position i in a 32-bit flag (Unsafe).
#define __hasFlagBitSet(flag, i)(((flag) & __mask32(i)) != 0U)      /// Check if bit i is SET (returns true/non-zero).
#define __hasFlagBitClr(flag, i)(((flag) & __mask32(i)) == 0U)      /// Check if bit i is CLEARED (returns true/non-zero).

/* --- SAFE FLAGS (Thread-Safe, ISR-Safe) --- */

/// @brief Atomic 32-bit flag type, safe for use in ISRs and Multi-threaded environments
typedef volatile atomic_uint_fast32_t safeFlag_t;

/// @brief Atomically set a bit (Thread-safe)
/// @param f Pointer to the safeFlag_t variable
/// @param i Bit position (0-31)
void __safeFlagSet(safeFlag_t *f, uint32_t i);

/// @brief Atomically clear a bit (Thread-safe)
/// @param f Pointer to the safeFlag_t variable
/// @param i Bit position (0-31)
void __safeFlagClear(safeFlag_t *f, uint32_t i);

/// @brief Atomically toggle a bit (Thread-safe)
/// @param f Pointer to the safeFlag_t variable
/// @param i Bit position (0-31)
void __safeFlagToggle(safeFlag_t *f, uint32_t i);

/// @brief Atomically check if a bit is set (Thread-safe)
/// @param f The safeFlag_t variable (passed by value is fine for read, but ptr is consistent)
/// @param i Bit position (0-31)
/// @return true if set, false otherwise
bool __safeFlagHas(safeFlag_t *f, uint32_t i);



#endif