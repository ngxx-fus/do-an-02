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


#define __max(a, b) ((a) > (b) ? (a) : (b))  /// Return max(a,b).
#define __min(a, b) ((a) < (b) ? (a) : (b))  /// Return min(a,b).
#define __abs(x)    ((x) >= 0 ? (x) : -(x))  /// Return absolute value.

#define __is_null(ptr)      ((ptr) == NULL)  /// Check pointer == NULL.
#define __isnot_null(ptr)   ((ptr) != NULL)  /// Check pointer != NULL.

#define __mask8(i)   ((uint8_t)(1U << (i)))     /// Mask 1 bit in uint8_t.
#define __mask16(i)  ((uint16_t)(1U << (i)))    /// Mask 1 bit in uint16_t.
#define __mask32(i)  ((uint32_t)(1UL << (i)))   /// Mask 1 bit in uint32_t.
#define __mask64(i)  ((uint64_t)(1ULL << (i)))  /// Mask 1 bit in uint64_t.

#define __inv_mask8(i)   ((uint8_t)(~(1U << (i))))     /// Inverted mask uint8_t.
#define __inv_mask16(i)  ((uint16_t)(~(1U << (i))))    /// Inverted mask uint16_t.
#define __inv_mask32(i)  ((uint32_t)(~(1UL << (i))))   /// Inverted mask uint32_t.
#define __inv_mask64(i)  ((uint64_t)(~(1ULL << (i))))  /// Inverted mask uint64_t.

#define __is_zero(x)         ((x) == 0)   /// Check x == 0.
#define __is_positive(x)     ((x) > 0)    /// Check x > 0.
#define __is_negative(x)     ((x) < 0)    /// Check x < 0.
#define __isnot_zero(x)      ((x) != 0)   /// Check x != 0.
#define __isnot_positive(x)  ((x) <= 0)   /// Check x <= 0.
#define __isnot_negative(x)  ((x) >= 0)   /// Check x >= 0.

/// Convert x to string using # in macro!
#define STR(x)  #x

/// Convert x to 1 (true) or 0 (false)
#define boolCast(x) (((x) != 0) ? (0x1) : (0x0))

#define __mask_or1(f,a)        (f(a))
#define __mask_or2(f,a,b)      (f(a) | f(b))
#define __mask_or3(f,a,b,c)    (f(a) | f(b) | f(c))
#define __mask_or4(f,a,b,c,d)  (f(a) | f(b) | f(c) | f(d))
#define __mask_or5(f,a,b,c,d,e) (f(a) | f(b) | f(c) | f(d) | f(e))
#define __mask_or6(f,a,b,c,d,e,f2) (f(a) | f(b) | f(c) | f(d) | f(e) | f(f2))
#define __mask_or7(f,a,b,c,d,e,f2,g) (f(a) | f(b) | f(c) | f(d) | f(e) | f(f2) | f(g))
#define __mask_or8(f,a,b,c,d,e,f2,g,h) (f(a) | f(b) | f(c) | f(d) | f(e) | f(f2) | f(g) | f(h))

#define __mask_overload(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME

#define __masks_generic(f, ...) \
    (__mask_overload(__VA_ARGS__, \
        __mask_or8, __mask_or7, __mask_or6, __mask_or5, \
        __mask_or4, __mask_or3, __mask_or2, __mask_or1)(f, __VA_ARGS__))

#define __masks8(...)   __masks_generic(__mask8,  __VA_ARGS__)
#define __masks16(...)  __masks_generic(__mask16, __VA_ARGS__)
#define __masks32(...)  __masks_generic(__mask32, __VA_ARGS__)
#define __masks64(...)  __masks_generic(__mask64, __VA_ARGS__)

/// @brief Generates a pseudo-random number in the range [0, 9999].
///        If the input seed is negative or repeated, the function reuses the last known random state.
///        Otherwise, it generates a new pseudo-random value based on the input seed.
/// @param seed_input The input seed used to initialize randomness. 
///        If seed_input < 0 or equals the previous seed, the last valid random state is used instead.
/// @return A pseudo-random integer in the range [0, 9999].
int generateRandom(int seed_input);



#endif