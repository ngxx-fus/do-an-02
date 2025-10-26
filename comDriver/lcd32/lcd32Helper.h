#ifndef __LCD32_HELPER_H__
#define __LCD32_HELPER_H__

#include "../../helper/general.h"                           /// General helper
#include "lcd32Def.h"                                       /// LCD 3.2" definitions

#define SECTION_LOG                                         1  /// Enable log section in helper
#define SECTION_LCD_CONTROLS                                1  /// Enable LCD controls section in helper
#define SECTION_LCD32_CTL_ACCESSORS                         1  /// Enable LCD accessors section in helper
#define SECTION_LCD32_OPERATIONS                            1  /// Enable LCD operations section in helper

#ifdef SECTION_LOG

#ifndef LCD32_LOG_LEVEL
    /*
    *  Set level of log, there are the description of the leves:
    *  Noted that: the greater value of log includes all lower log types!:
    *      0 : No log
    *      1 : Only log error
    *      2 : Log entry (+ params) and exit (+ return status)
    *      3 : Log state inside the function, ...
    *      4 : The lowest level of logs, it log all things, includes every byte will be sent!
    */
    #define LCD32_LOG_LEVEL 4
#endif

#if LCD32_LOG_LEVEL >= 1
    #define lcd32Err(...)       __sys_err(  __VA_ARGS__ )
#else 
    #define lcd32Err(...)
#endif
#if LCD32_LOG_LEVEL >= 2
    /// Log warnings with timestamps
    #define lcd32Warn(...)     __sys_warn(  __VA_ARGS__ )
    /// Log entry function with param (if any)
    #define __lcd32Entry(...)     __entry(  __VA_ARGS__ )
    /// Log exit function with return value (if any)
    #define __lcd32Exit(...)      __exit(  __VA_ARGS__ )
#else 
    /// Log warnings with timestamps
    #define lcd32Warn(...)
    /// Log entry function with params (if any)
    #define __lcd32Entry(...)
    /// Log exit function with return value (if any)
    #define __lcd32Exit(...)
#endif 
#if LCD32_LOG_LEVEL >= 3
    /// Log note from author
    #define lcd32Info(...)      __sys_info(  __VA_ARGS__ )
    /// Log note from author
    #define __lcd32Log(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log note from author
    #define __lcd32Log(...)
#endif 
#if LCD32_LOG_LEVEL >= 4
    /// Log more details
    #define lcd32Log1(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log more details
    #define lcd32Log1(...)
#endif 

#endif /// SECTION_LOG

#ifdef SECTION_LCD_CONTROLS

#define __lcd32SetHighReadPin(lcd32Dev)             (GPIO.out_w1ts = (1UL << (lcd32Dev)->controlPin.r))
#define __lcd32SetHighWritePin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->controlPin.w))
#define __lcd32SetHighRegSelPin(lcd32Dev)           (GPIO.out_w1ts = (1UL << (lcd32Dev)->controlPin.rs))
#define __lcd32SetHighResetPin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->controlPin.rst))
#define __lcd32SetHighChipSelPin(lcd32Dev)          (GPIO.out_w1ts = (1UL << (lcd32Dev)->controlPin.cs))
#define __lcd32SetHighBrightLightPin(lcd32Dev)      (GPIO.out_w1ts = (1UL << (lcd32Dev)->controlPin.bl))

#define __lcd32SetLowReadPin(lcd32Dev)              (GPIO.out_w1tc = (1UL << (lcd32Dev)->controlPin.r))
#define __lcd32SetLowWritePin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->controlPin.w))
#define __lcd32SetLowRegSelPin(lcd32Dev)            (GPIO.out_w1tc = (1UL << (lcd32Dev)->controlPin.rs))
#define __lcd32SetLowResetPin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->controlPin.rst))
#define __lcd32SetLowChipSelPin(lcd32Dev)           (GPIO.out_w1tc = (1UL << (lcd32Dev)->controlPin.cs))
#define __lcd32SetLowBrightLightPin(lcd32Dev)       (GPIO.out_w1tc = (1UL << (lcd32Dev)->controlPin.bl))

#define __lcd32SetHighData0Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__0))
#define __lcd32SetHighData1Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__1))
#define __lcd32SetHighData2Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__2))
#define __lcd32SetHighData3Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__3))
#define __lcd32SetHighData4Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__4))
#define __lcd32SetHighData5Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__5))
#define __lcd32SetHighData6Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__6))
#define __lcd32SetHighData7Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__7))
#define __lcd32SetHighData8Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__8))
#define __lcd32SetHighData9Pin(lcd32Dev)            (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__9))
#define __lcd32SetHighData10Pin(lcd32Dev)           (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__10))
#define __lcd32SetHighData11Pin(lcd32Dev)           (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__11))
#define __lcd32SetHighData12Pin(lcd32Dev)           (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__12))
#define __lcd32SetHighData13Pin(lcd32Dev)           (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__13))
#define __lcd32SetHighData14Pin(lcd32Dev)           (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__14))
#define __lcd32SetHighData15Pin(lcd32Dev)           (GPIO.out_w1ts = (1UL << (lcd32Dev)->dataPin.__15))

#define __lcd32SetLowData0Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__0))
#define __lcd32SetLowData1Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__1))
#define __lcd32SetLowData2Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__2))
#define __lcd32SetLowData3Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__3))
#define __lcd32SetLowData4Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__4))
#define __lcd32SetLowData5Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__5))
#define __lcd32SetLowData6Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__6))
#define __lcd32SetLowData7Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__7))
#define __lcd32SetLowData8Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__8))
#define __lcd32SetLowData9Pin(lcd32Dev)             (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__9))
#define __lcd32SetLowData10Pin(lcd32Dev)            (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__10))
#define __lcd32SetLowData11Pin(lcd32Dev)            (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__11))
#define __lcd32SetLowData12Pin(lcd32Dev)            (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__12))
#define __lcd32SetLowData13Pin(lcd32Dev)            (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__13))
#define __lcd32SetLowData14Pin(lcd32Dev)            (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__14))
#define __lcd32SetLowData15Pin(lcd32Dev)            (GPIO.out_w1tc = (1UL << (lcd32Dev)->dataPin.__15))

#define __lcd32GetReadPin(lcd32Dev)            ((GPIO.in >> (lcd32Dev)->controlPin.r) & 0x1)
#define __lcd32GetWritePin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->controlPin.w) & 0x1)
#define __lcd32GetRegSelPin(lcd32Dev)          ((GPIO.in >> (lcd32Dev)->controlPin.rs) & 0x1)
#define __lcd32GetResetPin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->controlPin.rst) & 0x1)
#define __lcd32GetChipSelPin(lcd32Dev)         ((GPIO.in >> (lcd32Dev)->controlPin.cs) & 0x1)


#define __lcd32GetData0Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__0) & 0x1)
#define __lcd32GetData1Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__1) & 0x1)
#define __lcd32GetData2Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__2) & 0x1)
#define __lcd32GetData3Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__3) & 0x1)
#define __lcd32GetData4Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__4) & 0x1)
#define __lcd32GetData5Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__5) & 0x1)
#define __lcd32GetData6Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__6) & 0x1)
#define __lcd32GetData7Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__7) & 0x1)
#define __lcd32GetData8Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__8) & 0x1)
#define __lcd32GetData9Pin(lcd32Dev)           ((GPIO.in >> (lcd32Dev)->dataPin.__9) & 0x1)
#define __lcd32GetData10Pin(lcd32Dev)          ((GPIO.in >> (lcd32Dev)->dataPin.__10) & 0x1)
#define __lcd32GetData11Pin(lcd32Dev)          ((GPIO.in >> (lcd32Dev)->dataPin.__11) & 0x1)
#define __lcd32GetData12Pin(lcd32Dev)          ((GPIO.in >> (lcd32Dev)->dataPin.__12) & 0x1)
#define __lcd32GetData13Pin(lcd32Dev)          ((GPIO.in >> (lcd32Dev)->dataPin.__13) & 0x1)
#define __lcd32GetData14Pin(lcd32Dev)          ((GPIO.in >> (lcd32Dev)->dataPin.__14) & 0x1)
#define __lcd32GetData15Pin(lcd32Dev)          ((GPIO.in >> (lcd32Dev)->dataPin.__15) & 0x1)

#endif /// SECTION_LCD_CONTROLS

#ifdef SECTION_LCD32_CTL_ACCESSORS

#define lcd32AssignZero(obj)                memset((void *)obj, 0, sizeof(*(obj)))

/// Or valid `pin` to `destBitMask`. (Note: A valid pin it not negative)
#define __lcd32AddGPIO(destBitMask, pin, ...)               \
    if(__isnot_negative(pin) && (pin < 32))                 \
        destBitMask |= __mask64(pin);                       \
    else {                                                  \
        lcd32Err("GPIO_Err: %d", pin);                      \
        __VA_OPT__(__VA_ARGS__)                             \
    }

/// Macro for check an obj is NULL neither or not NULL with the action if NULL
#define __lcd32NULLCheck(obj, objName, funcName, ...)         \
    do {                                                    \
        if (__is_null(obj)) {                               \
            lcd32Err("[%s] %s is NULL!", funcName, objName); \
            __VA_OPT__(__VA_ARGS__)                         \
        }                                                   \
    } while (0)

#endif /// SECTION_LCD32_CTL_ACCESSORS

#ifdef SECTION_LCD32_OPERATIONS

#define __lcd32SetDataTransaction(dev)                      __lcd32SetHighRegSelPin(dev);
#define __lcd32SetCommandTransaction(dev)                   __lcd32SetLowRegSelPin(dev);
#define __lcd32StartTransaction(dev)                        __lcd32SetLowChipSelPin(dev);
#define __lcd32StopTransaction(dev)                         __lcd32SetHighChipSelPin(dev);

#define __LCD32_WRITE_SIG(dev) \
    do { \
        __lcd32SetLowWritePin(dev); \
        __lcd32SetHighWritePin(dev); \
    } while(0)

#define __LCD32_READ_SIG(dev) \
    do { \
        __lcd32SetLowReadPin(dev); \
        __lcd32SetHighReadPin(dev); \
    } while(0)

//         __asm__ __volatile__("nop; nop; nop; nop;");\ \\\ for timing adjustment if needed

#define __lcd32SetOutputData16(dev) \
    do { \
        GPIOConfigOutputMask(dev->dataPinMask);\
        /*esp_rom_delay_us(5);*/\
    } while(0)

#define __lcd32SetInputData16(dev) \
    do { \
        GPIOConfigInputMask(dev->dataPinMask);\
        /*esp_rom_delay_us(5);*/\
    } while(0)

#define __lcd32SetParallelData(dev, data)        \
    do{ \
        uint16_t __data = data; \
        (__data&0x0001) ? __lcd32SetHighData0Pin(dev) : __lcd32SetLowData0Pin(dev); \
        (__data&0x0002) ? __lcd32SetHighData1Pin(dev) : __lcd32SetLowData1Pin(dev); \
        (__data&0x0004) ? __lcd32SetHighData2Pin(dev) : __lcd32SetLowData2Pin(dev); \
        (__data&0x0008) ? __lcd32SetHighData3Pin(dev) : __lcd32SetLowData3Pin(dev); \
        (__data&0x0010) ? __lcd32SetHighData4Pin(dev) : __lcd32SetLowData4Pin(dev); \
        (__data&0x0020) ? __lcd32SetHighData5Pin(dev) : __lcd32SetLowData5Pin(dev); \
        (__data&0x0040) ? __lcd32SetHighData6Pin(dev) : __lcd32SetLowData6Pin(dev); \
        (__data&0x0080) ? __lcd32SetHighData7Pin(dev) : __lcd32SetLowData7Pin(dev); \
        (__data&0x0100) ? __lcd32SetHighData8Pin(dev) : __lcd32SetLowData8Pin(dev); \
        (__data&0x0200) ? __lcd32SetHighData9Pin(dev) : __lcd32SetLowData9Pin(dev); \
        (__data&0x0400) ? __lcd32SetHighData10Pin(dev) : __lcd32SetLowData10Pin(dev); \
        (__data&0x0800) ? __lcd32SetHighData11Pin(dev) : __lcd32SetLowData11Pin(dev); \
        (__data&0x1000) ? __lcd32SetHighData12Pin(dev) : __lcd32SetLowData12Pin(dev); \
        (__data&0x2000) ? __lcd32SetHighData13Pin(dev) : __lcd32SetLowData13Pin(dev); \
        (__data&0x4000) ? __lcd32SetHighData14Pin(dev) : __lcd32SetLowData14Pin(dev); \
        (__data&0x8000) ? __lcd32SetHighData15Pin(dev) : __lcd32SetLowData15Pin(dev); \
    } while(0)

#define __lcd32GetParallelData(dev, data)        \
    do { \
        uint32_t gpio_val = GPIO.in; \
        (data) = 0; \
        (data) |= ((gpio_val >> (dev)->dataPin.__0)  & 0x1) << 0;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__1)  & 0x1) << 1;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__2)  & 0x1) << 2;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__3)  & 0x1) << 3;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__4)  & 0x1) << 4;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__5)  & 0x1) << 5;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__6)  & 0x1) << 6;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__7)  & 0x1) << 7;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__8)  & 0x1) << 8;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__9)  & 0x1) << 9;  \
        (data) |= ((gpio_val >> (dev)->dataPin.__10) & 0x1) << 10; \
        (data) |= ((gpio_val >> (dev)->dataPin.__11) & 0x1) << 11; \
        (data) |= ((gpio_val >> (dev)->dataPin.__12) & 0x1) << 12; \
        (data) |= ((gpio_val >> (dev)->dataPin.__13) & 0x1) << 13; \
        (data) |= ((gpio_val >> (dev)->dataPin.__14) & 0x1) << 14; \
        (data) |= ((gpio_val >> (dev)->dataPin.__15) & 0x1) << 15; \
    } while(0)

#define __lcd32WriteCommand(dev, cmd)  \
    do { \
        __lcd32SetCommandTransaction(dev); \
        __lcd32SetParallelData(dev, cmd); \
        __LCD32_WRITE_SIG(dev); \
    } while(0)

#define __lcd32WriteData(dev, data) \
    do { \
        __lcd32SetDataTransaction(dev); \
        __lcd32SetParallelData(dev, data); \
        __LCD32_WRITE_SIG(dev); \
    } while(0)

#define __lcd32ReadData(dev, data) \
    do { \
        __LCD32_READ_SIG(dev); \
        __lcd32SetDataTransaction(dev); \
        __lcd32GetParallelData(dev, data); \
    } while(0)


#endif /// SECTION_LCD32_OPERATIONS

#ifdef SECTION_LCD32_OPERATIONS

/// @brief Set pixel at (r, c) to given color (no boundary check)
#define lcd32SetCanvasPixel(dev, r, c, color) \
    (dev)->canvas.buff[(r)][(c)] = (color)

/// @brief Get pixel color at (r, c) (no boundary check)
#define lcd32GetCanvasPixel(dev, r, c) \
    ((dev)->canvas.buff[(r)][(c)])

/// @brief Get pointer to pixel at (r, c) (no boundary check)
#define lcd32GetCanvasPixelPtr(dev, r, c) \
    (&((dev)->canvas.buff[(r)][(c)]))

/// @brief Set pixel at (r, c) with wrap-around indexing.
#define lcd32SetCanvasPixelWrap(dev, r, c, color) \
    do { \
        int __r = ((r) % (dev)->canvas.maxRow + (dev)->canvas.maxRow) % (dev)->canvas.maxRow; \
        int __c = ((c) % (dev)->canvas.maxCol + (dev)->canvas.maxCol) % (dev)->canvas.maxCol; \
        (dev)->canvas.buff[__r][__c] = (color); \
    } while(0)

/// @brief Get pixel color at (r, c) with wrap-around indexing.
#define lcd32GetCanvasPixelWrap(dev, r, c) \
    ({ \
        int __r = ((r) % (dev)->canvas.maxRow + (dev)->canvas.maxRow) % (dev)->canvas.maxRow; \
        int __c = ((c) % (dev)->canvas.maxCol + (dev)->canvas.maxCol) % (dev)->canvas.maxCol; \
        (dev)->canvas.buff[__r][__c]; \
    })

/// @brief Get pointer to pixel at (r, c) with wrap-around indexing.
#define lcd32GetCanvasPixelPtrWrap(dev, r, c) \
    ({ \
        int __r = ((r) % (dev)->canvas.maxRow + (dev)->canvas.maxRow) % (dev)->canvas.maxRow; \
        int __c = ((c) % (dev)->canvas.maxCol + (dev)->canvas.maxCol) % (dev)->canvas.maxCol; \
        &((dev)->canvas.buff[__r][__c]); \
    })

#endif /// SECTION_LCD32_OPERATIONS

#endif