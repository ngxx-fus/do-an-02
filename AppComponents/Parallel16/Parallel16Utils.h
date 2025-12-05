/// @file   Parallel16Utils.h
/// @brief  Utility macros for GPIO control and Logging in Parallel16 driver.

#ifndef __PARALLEL16_UTILS_H__
#define __PARALLEL16_UTILS_H__

#include "soc/gpio_struct.h"    /// ESP32 GPIO Struct definition
#include "espLogWrapper.h"      /// Custom Logging wrapper
#include "Parallel16Def.h"      /// Driver Definitions

#include "../../AppUtils/All.h"
#include "../../AppESPWrap/All.h"

#define P16_LOG_ENTRY_L0_EN     1
#define P16_LOG_EXIT_L0_EN      1
#define P16_LOG_ENTRY_L1_EN     0
#define P16_LOG_EXIT_L1_EN      0
#define P16_LOG_INFO_EN         0
#define P16_LOG_WARN_EN         0
#define P16_LOG_ERR_EN          1
#define P16_LOG_L0_EN           0
#define P16_LOG_L1_EN           0
#define P16_DELAY_HALF_PERIOD   0

// L0: Standard Entry/Exit
#if (P16_LOG_ENTRY_L0_EN == 1)
    /// Log entry function with params
    #define P16Entry(...)           SysEntry(__VA_ARGS__)
#else 
    #define P16Entry(...)
#endif 

#if (P16_LOG_EXIT_L0_EN == 1)
    /// Log exit function with return value
    #define P16Exit(...)            SysExit(__VA_ARGS__)
    /// Log exit and return value
    #define P16ReturnWithLog(ret, ...)  SysExit(__VA_ARGS__); return ret;
#else
    #define P16Exit(...)
    #define P16ReturnWithLog(ret, ...)  return ret;
#endif

// L1: Detailed Entry/Exit
#if (P16_LOG_ENTRY_L1_EN == 1)
    /// Log detailed entry function
    #define P16Entry1(...)          SysEntryVer(__VA_ARGS__)
#else 
    #define P16Entry1(...)
#endif 

#if (P16_LOG_EXIT_L1_EN == 1)
    /// Log detailed exit function
    #define P16Exit1(...)           SysExitVer(__VA_ARGS__)
    /// Log detailed exit and return
    #define P16ReturnWithLog1(ret, ...) SysExitVer(__VA_ARGS__); return ret;
#else
    #define P16Exit1(...)
    #define P16ReturnWithLog1(ret, ...) return ret;
#endif

// Info/Warn/Err
#if (P16_LOG_INFO_EN == 1)
    /// Log info message
    #define P16Info(...)            SysInfo(__VA_ARGS__)
#else
    #define P16Info(...)
#endif

#if (P16_LOG_WARN_EN == 1)
    /// Log warning message
    #define P16Warn(...)            SysWarn(__VA_ARGS__)
#else
    #define P16Warn(...)
#endif

#if (P16_LOG_ERR_EN == 1)
    /// Log error message
    #define P16Err(...)             SysErr(__VA_ARGS__)
#else
    #define P16Err(...)
#endif

// General Logging Levels
#if (P16_LOG_L0_EN == 1)
    /// Log standard note
    #define P16Log(...)             SysLog(__VA_ARGS__)
#else 
    #define P16Log(...)
#endif 

#if (P16_LOG_L1_EN == 1)
    /// Log detailed note
    #define P16Log1(...)            SysLogVer(__VA_ARGS__)
#else 
    #define P16Log1(...)
#endif 


#define P16SetHighReadPin(p16Dev)     (GPIO.out_w1ts = (1UL << (p16Dev)->CtlPin.pin.r))
#define P16SetHighWritePin(p16Dev)    (GPIO.out_w1ts = (1UL << (p16Dev)->CtlPin.pin.w))
#define P16SetHighRegSelPin(p16Dev)   (GPIO.out_w1ts = (1UL << (p16Dev)->CtlPin.pin.rs))
#define P16SetHighChipSelPin(p16Dev)  (GPIO.out_w1ts = (1UL << (p16Dev)->CtlPin.pin.cs))

#define P16SetLowReadPin(p16Dev)      (GPIO.out_w1tc = (1UL << (p16Dev)->CtlPin.pin.r))
#define P16SetLowWritePin(p16Dev)     (GPIO.out_w1tc = (1UL << (p16Dev)->CtlPin.pin.w))
#define P16SetLowRegSelPin(p16Dev)    (GPIO.out_w1tc = (1UL << (p16Dev)->CtlPin.pin.rs))
#define P16SetLowChipSelPin(p16Dev)   (GPIO.out_w1tc = (1UL << (p16Dev)->CtlPin.pin.cs))

#define P16GetReadPin(p16Dev)         ((GPIO.in >> (p16Dev)->CtlPin.pin.r) & 0x1)
#define P16GetWritePin(p16Dev)        ((GPIO.in >> (p16Dev)->CtlPin.pin.w) & 0x1)
#define P16GetRegSelPin(p16Dev)       ((GPIO.in >> (p16Dev)->CtlPin.pin.rs) & 0x1)
#define P16GetChipSelPin(p16Dev)      ((GPIO.in >> (p16Dev)->CtlPin.pin.cs) & 0x1)

#define P16SetHighResetPin(p16Dev)    (GPIO.out_w1ts = (1UL << (p16Dev)->CtlPin.pin.rst))
#define P16SetLowResetPin(p16Dev)     (GPIO.out_w1tc = (1UL << (p16Dev)->CtlPin.pin.rst))
#define P16GetResetPin(p16Dev)        ((GPIO.in >> (p16Dev)->CtlPin.pin.rst) & 0x1)

// Data Pin Control
#define P16SetHighDataPin(p16Dev, n)  (GPIO.out_w1ts = Mask32(((p16Dev)->DatPin).arr[n]))
#define P16SetLowDataPin(p16Dev, n)   (GPIO.out_w1tc = Mask32(((p16Dev)->DatPin).arr[n]))
#define P16GetDataPin(p16Dev, n)      ((GPIO.in >> (((p16Dev)->DatPin).arr[n])) & 0x1)

// PLEASE CHECK: Ensure REP and Mask16 are defined
#define P16SetParallelData(dev, data)        \
    do{ \
        uint16_t __data = data; \
        REP(i, 0, P16_DATA_PIN_NUM) \
            if(__data & Mask16(i)) P16SetHighDataPin(dev, i); else P16SetLowDataPin(dev, i);\
    } while(0)

/// UTILITY MACROS ////////////////////////////////////////////////////////////

// PLEASE CHECK: Ensure __isnot_negative and __mask64 are defined
/// Add valid pin to bitmask (0-31 only)
#define P16AddGPIO(destBitMask, pin, ...)               \
    if(__isnot_negative(pin) && (pin < 32))             \
        destBitMask |= __mask64(pin);                   \
    else {                                              \
        P16Err("GPIO_Err: %d", pin);                    \
        __VA_OPT__(__VA_ARGS__)                         \
    }

// PLEASE CHECK: Ensure IsNull is defined
/// Check if object is NULL and log error
#define P16NULLCheck(obj, objName, funcName, ...)       \
    do {                                                \
        if (IsNull(obj)) {                           \
            P16Err("[%s] %s is NULL!", funcName, objName); \
            __VA_OPT__(__VA_ARGS__)                     \
        }                                               \
    } while (0)

/// DELAY /////////////////////////////////////////////////////////////////////

#if P16_DELAY_HALF_PERIOD == 1
    #define P16DelayHalfPeriod()    esp_rom_delay_us(50)
#else
    #define P16DelayHalfPeriod()
#endif

#endif /// __PARALLEL16_UTILS_H__