#ifndef __HELPER_H__
#define __HELPER_H__

/// @brief Loop forward from `a` to `b-1`.
/// @param type Loop variable type.
/// @param i Loop variable.
/// @param a Start value (inclusive).
/// @param b End value (exclusive).
#define REPT(type, i, a, b) for(type i = (a); (i) < (b); ++(i))

/// @brief Loop backward from `a` to `b+1`.
/// @param type Loop variable type.
/// @param i Loop variable.
/// @param a Start value (inclusive).
/// @param b End value (exclusive).
#define REVT(type, i, a, b) for(type i = (a); (i) > (b); --(i))

/// @brief Loop forward using size_t.
/// @param i Loop variable.
/// @param a Start value (inclusive).
/// @param b End value (exclusive).
#define REP(i, a, b) for(size_t i = (a); (i) < (b); ++(i))

/// @brief Loop backward using size_t.
/// @param i Loop variable.
/// @param a Start value (inclusive).
/// @param b End value (exclusive).
#define REV(i, a, b) for(size_t i = (a); (i) > (b); --(i))


#define __max(a, b) ((a) > (b) ? (a) : (b))  ///< Return max(a,b).
#define __min(a, b) ((a) < (b) ? (a) : (b))  ///< Return min(a,b).
#define __abs(x)    ((x) >= 0 ? (x) : -(x))  ///< Return absolute value.

#define __is_null(ptr)      ((ptr) == NULL)  ///< Check pointer == NULL.
#define __isnot_null(ptr)   ((ptr) != NULL)  ///< Check pointer != NULL.

#define __mask8(i)   ((uint8_t)(1U << (i)))     ///< Mask 1 bit in uint8_t.
#define __mask16(i)  ((uint16_t)(1U << (i)))    ///< Mask 1 bit in uint16_t.
#define __mask32(i)  ((uint32_t)(1UL << (i)))   ///< Mask 1 bit in uint32_t.
#define __mask64(i)  ((uint64_t)(1ULL << (i)))  ///< Mask 1 bit in uint64_t.

#define __inv_mask8(i)   ((uint8_t)(~(1U << (i))))     ///< Inverted mask uint8_t.
#define __inv_mask16(i)  ((uint16_t)(~(1U << (i))))    ///< Inverted mask uint16_t.
#define __inv_mask32(i)  ((uint32_t)(~(1UL << (i))))   ///< Inverted mask uint32_t.
#define __inv_mask64(i)  ((uint64_t)(~(1ULL << (i))))  ///< Inverted mask uint64_t.

#define __is_zero(x)         ((x) == 0)   ///< Check x == 0.
#define __is_positive(x)     ((x) > 0)    ///< Check x > 0.
#define __is_negative(x)     ((x) < 0)    ///< Check x < 0.
#define __isnot_zero(x)      ((x) != 0)   ///< Check x != 0.
#define __isnot_positive(x)  ((x) <= 0)   ///< Check x <= 0.
#define __isnot_negative(x)  ((x) >= 0)   ///< Check x >= 0.

/// Convert x to string using # in macro!
#define STR(x)  #x

/// Convert x to 1 (true) or 0 (false)
#define boolCast(x) (((x) != 0) ? (0x1) : (0x0))

#endif