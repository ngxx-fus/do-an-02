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
    #define __lcd32Err(...)       __sys_err(  __VA_ARGS__ )
#else 
    #define __lcd32Err(...)
#endif
#if LCD32_LOG_LEVEL >= 2
    /// Log warnings with timestamps
    #define __lcd32Warn(...)     __sys_warn(  __VA_ARGS__ )
    /// Log entry function with param (if any)
    #define __lcd32Entry(...)     __entry(  __VA_ARGS__ )
    /// Log exit function with return value (if any)
    #define __lcd32Exit(...)      __exit(  __VA_ARGS__ )
#else 
    /// Log warnings with timestamps
    #define __lcd32Warn(...)
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

#define __lcd32SetHighBrightLightPin(lcd32Dev)      (GPIO.out_w1ts = (1UL << (lcd32Dev)->BLPin))
#define __lcd32SetLowBrightLightPin(lcd32Dev)       (GPIO.out_w1tc = (1UL << (lcd32Dev)->BLPin))

/// Macro for check an obj is NULL neither or not NULL with the action if NULL
#define __lcd32NULLCheck(obj, objName, funcName, ...)         \
    do {                                                    \
        if (__is_null(obj)) {                               \
            __lcd32Err("[%s] %s is NULL!", funcName, objName); \
            __VA_OPT__(__VA_ARGS__)                         \
        }                                                   \
    } while (0)

#endif /// SECTION_LCD32_CTL_ACCESSORS

#ifdef SECTION_LCD32_OPERATIONS

#define __lcd32SetDataTransaction(dev)                      __p16SetHighRegSelPin(&(dev->p16));
#define __lcd32SetCommandTransaction(dev)                   __p16SetLowRegSelPin(&(dev->p16));
#define __lcd32StartTransaction(dev)                        __p16SetLowChipSelPin(&(dev->p16));
#define __lcd32StopTransaction(dev)                         __p16SetHighChipSelPin(&(dev->p16));

#define __lcd32WriteCommand(dev, cmd) \
    do{\
        uint16_t data16 = cmd;\
        __p16SetLowRegSelPin(&(dev->p16));\
        p16Write(&(dev->p16), data16);\
    }while(0);

#define __lcd32WriteData(dev, data) \
    do{\
        uint16_t data16 = data;\
        __p16SetLowRegSelPin(&(dev->p16));\
        p16Write(&(dev->p16), data16);\
    }while(0);

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