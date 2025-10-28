#ifndef __PARALLEL16_LOG_H__
#define __PARALLEL16_LOG_H__

#include "../../helper/general.h"

/// LOG ///////////////////////////////////////////////////////////////////////////////////////////

#if (P16_LOG_LEVEL >= 1)
    #define __p16Err(...)       __sys_err(  __VA_ARGS__ )
#else 
    #define __p16Err(...)
#endif
#if (P16_LOG_LEVEL >= 2)
    /// Log warnings with timestamps
    #define __p16Warn(...)     __sys_warn(  __VA_ARGS__ )
    /// Log entry function with param (if any)
    #define __p16Entry(...)     __entry(  __VA_ARGS__ )
    /// Log exit function with return value (if any)
    #define __p16Exit(...)      __exit(  __VA_ARGS__ )
#else 
    /// Log warnings with timestamps
    #define __p16Warn(...)
    /// Log entry function with params (if any)
    #define __p16Entry(...)
    /// Log exit function with return value (if any)
    #define __p16Exit(...)
#endif 
#if (P16_LOG_LEVEL >= 3)
    /// Log note from author
    #define __p16Info(...)      __sys_info(  __VA_ARGS__ )
    /// Log note from author
    #define __p16Log(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log note from author
    #define __p16Log(...)
#endif 
#if (P16_LOG_LEVEL >= 4)
    /// Log more details
    #define __p16Log1(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log more details
    #define __p16Log1(...)
#endif 

/// SINGLE PIN SETUP //////////////////////////////////////////////////////////////////////////////

#define __p16SetHighReadPin(p16Dev)             (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.r))
#define __p16SetHighWritePin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.w))
#define __p16SetHighRegSelPin(p16Dev)           (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.rs))
#define __p16SetHighResetPin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.rst))
#define __p16SetHighChipSelPin(p16Dev)          (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.cs))

#define __p16SetLowReadPin(p16Dev)              (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.r))
#define __p16SetLowWritePin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.w))
#define __p16SetLowRegSelPin(p16Dev)            (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.rs))
#define __p16SetLowResetPin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.rst))
#define __p16SetLowChipSelPin(p16Dev)           (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.cs))

#define __p16SetHighData0Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__0))
#define __p16SetHighData1Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__1))
#define __p16SetHighData2Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__2))
#define __p16SetHighData3Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__3))
#define __p16SetHighData4Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__4))
#define __p16SetHighData5Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__5))
#define __p16SetHighData6Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__6))
#define __p16SetHighData7Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__7))
#define __p16SetHighData8Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__8))
#define __p16SetHighData9Pin(p16Dev)            (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__9))
#define __p16SetHighData10Pin(p16Dev)           (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__10))
#define __p16SetHighData11Pin(p16Dev)           (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__11))
#define __p16SetHighData12Pin(p16Dev)           (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__12))
#define __p16SetHighData13Pin(p16Dev)           (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__13))
#define __p16SetHighData14Pin(p16Dev)           (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__14))
#define __p16SetHighData15Pin(p16Dev)           (GPIO.out_w1ts = (1UL << (p16Dev)->dataPin.__15))

#define __p16SetLowData0Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__0))
#define __p16SetLowData1Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__1))
#define __p16SetLowData2Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__2))
#define __p16SetLowData3Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__3))
#define __p16SetLowData4Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__4))
#define __p16SetLowData5Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__5))
#define __p16SetLowData6Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__6))
#define __p16SetLowData7Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__7))
#define __p16SetLowData8Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__8))
#define __p16SetLowData9Pin(p16Dev)             (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__9))
#define __p16SetLowData10Pin(p16Dev)            (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__10))
#define __p16SetLowData11Pin(p16Dev)            (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__11))
#define __p16SetLowData12Pin(p16Dev)            (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__12))
#define __p16SetLowData13Pin(p16Dev)            (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__13))
#define __p16SetLowData14Pin(p16Dev)            (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__14))
#define __p16SetLowData15Pin(p16Dev)            (GPIO.out_w1tc = (1UL << (p16Dev)->dataPin.__15))

#define __p16GetReadPin(p16Dev)            ((GPIO.in >> (p16Dev)->controlPin.r) & 0x1)
#define __p16GetWritePin(p16Dev)           ((GPIO.in >> (p16Dev)->controlPin.w) & 0x1)
#define __p16GetRegSelPin(p16Dev)          ((GPIO.in >> (p16Dev)->controlPin.rs) & 0x1)
#define __p16GetResetPin(p16Dev)           ((GPIO.in >> (p16Dev)->controlPin.rst) & 0x1)
#define __p16GetChipSelPin(p16Dev)         ((GPIO.in >> (p16Dev)->controlPin.cs) & 0x1)


#define __p16GetData0Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__0) & 0x1)
#define __p16GetData1Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__1) & 0x1)
#define __p16GetData2Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__2) & 0x1)
#define __p16GetData3Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__3) & 0x1)
#define __p16GetData4Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__4) & 0x1)
#define __p16GetData5Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__5) & 0x1)
#define __p16GetData6Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__6) & 0x1)
#define __p16GetData7Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__7) & 0x1)
#define __p16GetData8Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__8) & 0x1)
#define __p16GetData9Pin(p16Dev)           ((GPIO.in >> (p16Dev)->dataPin.__9) & 0x1)
#define __p16GetData10Pin(p16Dev)          ((GPIO.in >> (p16Dev)->dataPin.__10) & 0x1)
#define __p16GetData11Pin(p16Dev)          ((GPIO.in >> (p16Dev)->dataPin.__11) & 0x1)
#define __p16GetData12Pin(p16Dev)          ((GPIO.in >> (p16Dev)->dataPin.__12) & 0x1)
#define __p16GetData13Pin(p16Dev)          ((GPIO.in >> (p16Dev)->dataPin.__13) & 0x1)
#define __p16GetData14Pin(p16Dev)          ((GPIO.in >> (p16Dev)->dataPin.__14) & 0x1)
#define __p16GetData15Pin(p16Dev)          ((GPIO.in >> (p16Dev)->dataPin.__15) & 0x1)

/// Or valid `pin` to `destBitMask`. (Note: A valid pin it not negative)
#define __p16AddGPIO(destBitMask, pin, ...)               \
    if(__isnot_negative(pin) && (pin < 32))                 \
        destBitMask |= __mask64(pin);                       \
    else {                                                  \
        __p16Err("GPIO_Err: %d", pin);                      \
        __VA_OPT__(__VA_ARGS__)                             \
    }

/// Macro for check an obj is NULL neither or not NULL with the action if NULL
#define __p16NULLCheck(obj, objName, funcName, ...)         \
    do {                                                    \
        if (__is_null(obj)) {                               \
            __p16Err("[%s] %s is NULL!", funcName, objName); \
            __VA_OPT__(__VA_ARGS__)                         \
        }                                                   \
    } while (0)



#endif