/// @file helper.h
/// @brief Common helper macros for loops, bit manipulation, math, and pointer handling.

#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdint.h>
#include <stddef.h>

/* ============================================================================================== */
/* LOOPS                                                                                          */
/* ============================================================================================== */

/// @brief Loop forward from `a` to `b-1` (Integer types).
/// @param type Loop variable type (e.g., int, uint8_t).
/// @param i    Loop variable name.
/// @param a    Start value (inclusive).
/// @param b    End value (exclusive).
#define REPT(type, i, a, b) for(type i = (a); (i) < (b); ++(i))

/// @brief Loop backward from `a` to `b+1` (Integer types).
/// @param type Loop variable type.
/// @param i    Loop variable name.
/// @param a    Start value (inclusive).
/// @param b    End value (exclusive).
#define REVT(type, i, a, b) for(type i = (a); (i) > (b); --(i))

/// @brief Loop forward using size_t (Standard for array indexing).
/// @param i Loop variable name.
/// @param a Start value (inclusive).
/// @param b End value (exclusive).
#define REP(i, a, b)        for(size_t i = (a); (i) < (b); ++(i))

/// @brief Loop backward using size_t.
/// @param i Loop variable name.
/// @param a Start value (inclusive).
/// @param b End value (exclusive).
#define REV(i, a, b)        for(size_t i = (a); (i) > (b); --(i))

/* ============================================================================================== */
/* MATH & LOGIC UTILS                                                                             */
/* ============================================================================================== */

#define __max(a, b)         ((a) > (b) ? (a) : (b))     ///< Return maximum of a and b.
#define __min(a, b)         ((a) < (b) ? (a) : (b))     ///< Return minimum of a and b.
#define __abs(x)            ((x) >= 0 ? (x) : -(x))     ///< Return absolute value.

#define __is_null(ptr)      ((ptr) == NULL)             ///< Check if pointer is NULL.
#define __isnot_null(ptr)   ((ptr) != NULL)             ///< Check if pointer is NOT NULL.

/* ============================================================================================== */
/* BIT MASKING (SINGLE BIT)                                                                       */
/* ============================================================================================== */

#define __mask8(i)          ((uint8_t)(1U << (i)))      ///< Generate 8-bit mask with bit i set.
#define __mask16(i)         ((uint16_t)(1U << (i)))     ///< Generate 16-bit mask with bit i set.
#define __mask32(i)         ((uint32_t)(1UL << (i)))    ///< Generate 32-bit mask with bit i set.
#define __mask64(i)         ((uint64_t)(1ULL << (i)))   ///< Generate 64-bit mask with bit i set.

#define __inv_mask8(i)      ((uint8_t)(~(1U << (i))))   ///< Generate 8-bit mask with bit i cleared.
#define __inv_mask16(i)     ((uint16_t)(~(1U << (i))))  ///< Generate 16-bit mask with bit i cleared.
#define __inv_mask32(i)     ((uint32_t)(~(1UL << (i)))) ///< Generate 32-bit mask with bit i cleared.
#define __inv_mask64(i)     ((uint64_t)(~(1ULL << (i))))///< Generate 64-bit mask with bit i cleared.

/* ============================================================================================== */
/* VALUE CHECKS                                                                                   */
/* ============================================================================================== */

#define __is_zero(x)        ((x) == 0)      ///< Check if x is zero.
#define __is_positive(x)    ((x) > 0)       ///< Check if x is strictly positive (> 0).
#define __is_negative(x)    ((x) < 0)       ///< Check if x is strictly negative (< 0).
#define __isnot_zero(x)     ((x) != 0)      ///< Check if x is non-zero.
#define __isnot_positive(x) ((x) <= 0)      ///< Check if x is not positive (<= 0).
#define __isnot_negative(x) ((x) >= 0)      ///< Check if x is not negative (>= 0).

/* ============================================================================================== */
/* CONVERSION & CASTING                                                                           */
/* ============================================================================================== */

/// @brief Convert macro argument x to string literal.
#define STR(x)              #x

/// @brief Cast x to strictly 1 (true) or 0 (false).
#define boolCast(x)         (((x) != 0) ? (0x1) : (0x0))

/* ============================================================================================== */
/* VARIADIC BIT MASKING (INTERNAL HELPERS)                                                        */
/* ============================================================================================== */

/* Note: These macros are internal helpers for __masksX macros. 
   They perform OR operations on a variable number of arguments. */

#define __mask_or1(f,a0)                                            (f(a0))
#define __mask_or2(f,a0,a1)                                         (f(a0) | f(a1))
#define __mask_or3(f,a0,a1,a2)                                      (f(a0) | f(a1) | f(a2))
#define __mask_or4(f,a0,a1,a2,a3)                                   (f(a0) | f(a1) | f(a2) | f(a3))
#define __mask_or5(f,a0,a1,a2,a3,a4)                                (f(a0) | f(a1) | f(a2) | f(a3) | f(a4))
#define __mask_or6(f,a0,a1,a2,a3,a4,a5)                             (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5))
#define __mask_or7(f,a0,a1,a2,a3,a4,a5,a6)                          (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6))
#define __mask_or8(f,a0,a1,a2,a3,a4,a5,a6,a7)                       (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7))
#define __mask_or9(f,a0,a1,a2,a3,a4,a5,a6,a7,a8)                    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8))
#define __mask_or10(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9)                (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9))
#define __mask_or11(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)            (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10))
#define __mask_or12(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)        (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11))
#define __mask_or13(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12))
#define __mask_or14(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13)(f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13))
#define __mask_or15(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14))
#define __mask_or16(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15))
#define __mask_or17(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16))
#define __mask_or18(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17))
#define __mask_or19(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18))
#define __mask_or20(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19))
#define __mask_or21(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20))
#define __mask_or22(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21))
#define __mask_or23(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22))
#define __mask_or24(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23))
#define __mask_or25(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24))
#define __mask_or26(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24) | f(a25))
#define __mask_or27(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24) | f(a25) | f(a26))
#define __mask_or28(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24) | f(a25) | f(a26) | f(a27))
#define __mask_or29(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27,a28) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24) | f(a25) | f(a26) | f(a27) | f(a28))
#define __mask_or30(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27,a28,a29) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24) | f(a25) | f(a26) | f(a27) | f(a28) | f(a29))
#define __mask_or31(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,a30) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24) | f(a25) | f(a26) | f(a27) | f(a28) | f(a29) | f(a30))
#define __mask_or32(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,a30,a31) \
    (f(a0) | f(a1) | f(a2) | f(a3) | f(a4) | f(a5) | f(a6) | f(a7) | f(a8) | f(a9) | f(a10) | f(a11) | f(a12) | f(a13) | f(a14) | f(a15) | f(a16) | f(a17) | f(a18) | f(a19) | f(a20) | f(a21) | f(a22) | f(a23) | f(a24) | f(a25) | f(a26) | f(a27) | f(a28) | f(a29) | f(a30) | f(a31))

#define __mask_overload( \
    _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16, \
    _17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32, NAME, ...) NAME

/* ============================================================================================== */
/* VARIADIC BIT MASKING (USER MACROS)                                                             */
/* ============================================================================================== */

#define __masks_generic(f, ...) \
    (__mask_overload(__VA_ARGS__, \
        __mask_or32, __mask_or31, __mask_or30, __mask_or29, \
        __mask_or28, __mask_or27, __mask_or26, __mask_or25, \
        __mask_or24, __mask_or23, __mask_or22, __mask_or21, \
        __mask_or20, __mask_or19, __mask_or18, __mask_or17, \
        __mask_or16, __mask_or15, __mask_or14, __mask_or13, \
        __mask_or12, __mask_or11, __mask_or10, __mask_or9,  \
        __mask_or8,  __mask_or7,  __mask_or6,  __mask_or5,  \
        __mask_or4,  __mask_or3,  __mask_or2,  __mask_or1)(f, __VA_ARGS__))

/// @brief Generate 8-bit mask from list of bit positions. E.g: __masks8(0, 2) -> (1<<0 | 1<<2)
#define __masks8(...)   __masks_generic(__mask8,  __VA_ARGS__)

/// @brief Generate 16-bit mask from list of bit positions.
#define __masks16(...)  __masks_generic(__mask16, __VA_ARGS__)

/// @brief Generate 32-bit mask from list of bit positions.
#define __masks32(...)  __masks_generic(__mask32, __VA_ARGS__)

/// @brief Generate 64-bit mask from list of bit positions.
#define __masks64(...)  __masks_generic(__mask64, __VA_ARGS__)

/* ============================================================================================== */
/* POINTER SHORTCUTS                                                                              */
/* ============================================================================================== */

#define PTR(what)       what * ///< Declare pointer type shortcut.
#define DATA(ptr)       (*(ptr))        ///< Dereference pointer (Access data).
#define DATA2(ptr)      (*(*(ptr)))     ///< Dereference double pointer.
#define REF(ptr)        (*(ptr))        ///< Alias for dereference.
#define REF2(ptr)       (*(*(ptr)))     ///< Alias for double dereference.
#define ADDR(obj)       (&(obj))        ///< Get address of variable.
#define ADDR2(obj)      (&(&(obj)))     ///< Get address of address (valid only if obj is lvalue result).
#define DEREF(obj)      (&(obj))        ///< Semantic alias for address of.
#define DEREF2(obj)     (&(&(obj)))     ///< Semantic alias for double address of.

/* ============================================================================================== */
/* MISC HELPER FUNCTIONS                                                                          */
/* ============================================================================================== */

/// @brief Generates a pseudo-random number in the range [0, 9999].
///        If the input seed is negative or repeated, the function reuses the last known random state.
/// @param seed_input The input seed used to initialize randomness. 
///        If seed_input < 0 or equals the previous seed, the last valid random state is used instead.
/// @return A pseudo-random integer in the range [0, 9999].
unsigned int genRandNum(unsigned int seed_input);

#endif