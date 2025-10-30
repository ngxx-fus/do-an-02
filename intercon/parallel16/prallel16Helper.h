#ifndef __PARALLEL16_LOG_H__
#define __PARALLEL16_LOG_H__

#include "../../helper/general.h"



/// LOG ///////////////////////////////////////////////////////////////////////////////////////////

#define P16_LOG_ENTRY_L0_EN     1
#define P16_LOG_EXIT_L0_EN      1
#define P16_LOG_ENTRY_L1_EN     0
#define P16_LOG_EXIT_L1_EN      0
#define P16_LOG_INFO_EN         0
#define P16_LOG_WARN_EN         0
#define P16_LOG_ERR_EN          1
#define P16_LOG_L0_EN           0
#define P16_LOG_L1_EN           0

#if (P16_LOG_ENTRY_L0_EN == 1)
    /// Log entry function with param (if any)
    #define __p16Entry(...)                             __entry(  __VA_ARGS__ )
#else 
    /// Log entry function with params (if any)
    #define __p16Entry(...)
#endif 

#if (P16_LOG_EXIT_L0_EN == 1)
    /// Log exit function with return value (if any)
    #define __p16Exit(...)                              __exit(  __VA_ARGS__ )
    #define __p16ReturnWithLog(returnValue, ...)        __exit(  __VA_ARGS__ ); return returnValue;
#else
    /// Log exit function with return value (if any)
    #define __p16Exit(...)
    #define __p16ReturnWithLog(returnValue, ...)        return returnValue;
#endif

#if (P16_LOG_ENTRY_L1_EN == 1)
    /// Log entry function with param (if any)
    #define __p16Entry1(...)                            __entry(  __VA_ARGS__ )
#else 
    /// Log entry function with params (if any)
    #define __p16Entry1(...)
#endif 

#if (P16_LOG_EXIT_L1_EN == 1)
    /// Log exit function with return value (if any)
    #define __p16Exit1(...)      __exit(  __VA_ARGS__ )
    #define __p16ReturnWithLog1(returnValue, ...)       __exit(  __VA_ARGS__ ); return returnValue;
#else
    /// Log exit function with return value (if any)
    #define __p16Exit1(...)
    #define __p16ReturnWithLog1(returnValue, ...)       return returnValue;
#endif

#if (P16_LOG_INFO_EN == 1)
    /// Log note from author
    #define __p16Info(...)      __sys_info(  __VA_ARGS__ )
#else
    /// Log note from author
    #define __p16Info(...)
#endif

#if (P16_LOG_WARN_EN == 1)
    /// Log warnings with timestamps
    #define __p16Warn(...)     __sys_warn(  __VA_ARGS__ )
#else
    /// Log warnings with timestamps
    #define __p16Warn(...)
#endif

#if (P16_LOG_ERR_EN == 1)
    /// Log errors with timestamps
    #define __p16Err(...)     __sys_err(  __VA_ARGS__ )
#else
    /// Log errors with timestamps
    #define __p16Err(...)
#endif

#if (P16_LOG_L0_EN == 1)
    /// Log note from author
    #define __p16Log(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log note from author
    #define __p16Log(...)
#endif 

#if (P16_LOG_L1_EN == 1)
    /// Log more details
    #define __p16Log1(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log more details
    #define __p16Log1(...)
#endif 


/// SINGLE PIN SETUP //////////////////////////////////////////////////////////////////////////////

#define __p16SetHighReadPin(p16Dev)     (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.pin.r))
#define __p16SetHighWritePin(p16Dev)    (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.pin.w))
#define __p16SetHighRegSelPin(p16Dev)   (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.pin.rs))
#define __p16SetHighChipSelPin(p16Dev)  (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.pin.cs))

#define __p16SetLowReadPin(p16Dev)      (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.pin.r))
#define __p16SetLowWritePin(p16Dev)     (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.pin.w))
#define __p16SetLowRegSelPin(p16Dev)    (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.pin.rs))
#define __p16SetLowChipSelPin(p16Dev)   (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.pin.cs))

#define __p16GetReadPin(p16Dev)         ((GPIO.in >> (p16Dev)->controlPin.pin.r) & 0x1)
#define __p16GetWritePin(p16Dev)        ((GPIO.in >> (p16Dev)->controlPin.pin.w) & 0x1)
#define __p16GetRegSelPin(p16Dev)       ((GPIO.in >> (p16Dev)->controlPin.pin.rs) & 0x1)
#define __p16GetChipSelPin(p16Dev)      ((GPIO.in >> (p16Dev)->controlPin.pin.cs) & 0x1)


#define __p16SetHighResetPin(p16Dev)    (GPIO.out_w1ts = (1UL << (p16Dev)->controlPin.pin.rst))
#define __p16SetLowResetPin(p16Dev)     (GPIO.out_w1tc = (1UL << (p16Dev)->controlPin.pin.rst))
#define __p16GetResetPin(p16Dev)        ((GPIO.in >> (p16Dev)->controlPin.pin.rst) & 0x1)

#define __p16SetHighDataPin(p16Dev, n)  (GPIO.out_w1ts = __mask32(((p16Dev)->dataPin).arr[n]))
#define __p16SetLowDataPin(p16Dev, n)   (GPIO.out_w1tc = __mask32(((p16Dev)->dataPin).arr[n]))
#define __p16GetDataPin(p16Dev, n)      ((GPIO.in >> (((p16Dev)->dataPin).arr[n])) & 0x1)

#define __p16SetParallelData(dev, data)        \
    do{ \
        uint16_t __data = data; \
        REP(i, 0, P16_DATA_PIN_NUM) if(data & __mask16(i)) __p16SetHighDataPin(dev, i); else __p16SetLowDataPin(dev, i);\
    } while(0)

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

/// DELAY /////////////////////////////////////////////////////////////////////////////////////////
#define P16_DELAY_HALF_PERIOD           0

#if P16_DELAY_HALF_PERIOD == 1
    #define __p16DelayHalfPeriod()    esp_rom_delay_us(50)
#else
    #define __p16DelayHalfPeriod()
#endif

#endif