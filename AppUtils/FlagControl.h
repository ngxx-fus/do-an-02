/// @file flag.h
/// @brief Bit-flag utilities; provides fast non-atomic flag ops and thread-safe atomic flag manipulation.

#ifndef __FLAG_H__
#define __FLAG_H__

#ifdef __cplusplus
extern "C" {
#endif

/// Standard input/output definitions
#include <stdio.h>
/// Fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdint.h>
/// Variable arguments handling
#include <stdarg.h>
/// Boolean type definitions
#include <stdbool.h>
/// Standard atomic operations for thread safety
#include <stdatomic.h>

#ifndef __mask32
/// @brief Macro to generate a 32-bit mask with bit 'i' set (1 << i)
/// @note Check definition/implementation here
#define __mask32(i) (1UL << (i))
#endif

#ifndef __inv_mask32
/// @brief Macro to generate a 32-bit mask with bit 'i' cleared (~(1 << i))
/// @note Check definition/implementation here
#define __inv_mask32(i) (~(1UL << (i)))
#endif


/* --- UNSAFE FLAGS (Fast, Single Thread) --- */

/// @brief Standard 32-bit flag type for non-atomic bitwise operations
typedef uint32_t Flag_t;

/// @brief Generate a 32-bit mask with the i-th bit set
#define __flagMask(i)           __mask32(i)

/// @brief Generate a 32-bit mask with the i-th bit cleared
#define __flagInvMask(i)        __inv_mask32(i)

/// @brief Set the bit at position i in a 32-bit flag (Unsafe)
#define __SetFlagBit(flag, i)   ((flag) |= __mask32(i))

/// @brief Clear the bit at position i in a 32-bit flag (Unsafe)
#define __ClearFlagBit(flag, i) ((flag) &= __inv_mask32(i))

/// @brief Toggle the bit at position i in a 32-bit flag (Unsafe)
#define __ToggleFlagBit(flag, i) ((flag) ^= __mask32(i))

/// @brief Check if bit i is SET (returns true/non-zero)
#define __HasFlagBitSet(flag, i) (((flag) & __mask32(i)) != 0U)

/// @brief Check if bit i is CLEARED (returns true/non-zero)
#define __HasFlagBitClr(flag, i) (((flag) & __mask32(i)) == 0U)


/* --- SAFE FLAGS (Thread-Safe, ISR-Safe) --- */

/// @brief Atomic 32-bit flag type, safe for use in ISRs and Multi-threaded environments
typedef volatile atomic_uint_fast32_t SafeFlag_t;

/// @brief Atomically set a bit (Thread-safe)
/// @param f Pointer to the SafeFlag_t variable
/// @param i Bit position (0-31)
void __SafeFlagSet(SafeFlag_t *f, uint32_t i);

/// @brief Atomically clear a bit (Thread-safe)
/// @param f Pointer to the SafeFlag_t variable
/// @param i Bit position (0-31)
void __SafeFlagClear(SafeFlag_t *f, uint32_t i);

/// @brief Atomically toggle a bit (Thread-safe)
/// @param f Pointer to the SafeFlag_t variable
/// @param i Bit position (0-31)
void __SafeFlagToggle(SafeFlag_t *f, uint32_t i);

/// @brief Atomically check if a bit is set (Thread-safe)
/// @param f The SafeFlag_t variable (pointer used for API consistency)
/// @param i Bit position (0-31)
/// @return true if set, false otherwise
bool __SafeFlagHas(SafeFlag_t *f, uint32_t i);

#ifdef __cplusplus
}
#endif

#endif // __FLAG_H__