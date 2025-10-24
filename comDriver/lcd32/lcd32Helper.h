#ifndef __LCD32_HELPER_H__
#define __LCD32_HELPER_H__

#include "../../helper/general.h"

/// LOG ///////////////////////////////////////////////////////////////////////////////////////////

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
    #define lcd32Entry(...)     __entry(  __VA_ARGS__ )
    /// Log exit function with return value (if any)
    #define lcd32Exit(...)      __exit(  __VA_ARGS__ )
#else 
    /// Log warnings with timestamps
    #define lcd32Warn(...)
    /// Log entry function with params (if any)
    #define lcd32Entry(...)
    /// Log exit function with return value (if any)
    #define lcd32Exit(...)
#endif 
#if LCD32_LOG_LEVEL >= 3
    /// Log note from author
    #define lcd32Info(...)      __sys_info(  __VA_ARGS__ )
    /// Log note from author
    #define lcd32Log(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log note from author
    #define lcd32Log(...)
#endif 
#if LCD32_LOG_LEVEL >= 4
    /// Log more details
    #define lcd32Log1(...)      __sys_log(  __VA_ARGS__ )
#else 
    /// Log more details
    #define lcd32Log1(...)
#endif 

/// MACROS ////////////////////////////////////////////////////////////////////////////////////////

/// Or valid `pin` to `destBitMask`. (Note: A valid pin it not negative)
#define lcd32AddGPIO(destBitMask, pin)      if(__isnot_negative(pin)) destBitMask |= (pin);

/// Macro for check an obj is NULL neither or not NULL with the action if NULL
#define lcd32NULLCheck(obj, objName, funcName, ...)   \
    do {                                              \
        if (__is_null(obj)) {                         \
            lcd32Err("[%s] %s is NULL!", funcName, objName); \
            __VA_OPT__(__VA_ARGS__)                   \
        }                                             \
    } while (0)

#define lcd32AssignZero(ptr)                memset(ptr, 0, sizeof(ptr))

#define lcd32GetPixelPtr(dev, r, c) \
    (((r) >= 0 && (r) < (dev)->canvas.maxRow && \
      (c) >= 0 && (c) < (dev)->canvas.maxCol) ? \
        &((dev)->canvas.buff[(r)][(c)]) : NULL)

#define lcd32GetPixel(dev, r, c) \
    (((r) >= 0 && (r) < (dev)->canvas.maxRow && \
      (c) >= 0 && (c) < (dev)->canvas.maxCol) ? \
        ((dev)->canvas.buff[(r)][(c)]) : 0)

#define lcd32SetPixel(dev, r, c, color) \
    do { \
        if ((r) >= 0 && (r) < (dev)->canvas.maxRow && \
            (c) >= 0 && (c) < (dev)->canvas.maxCol) { \
            (dev)->canvas.buff[(r)][(c)] = (color); \
        } \
    } while(0)

#define lcd32SetPixelNC(dev, r, c, color) \
    ((dev)->canvas.buff[(r)][(c)] = (color))

#define lcd32GPIOSetState(pin, state)                  \
    if ((pin) < 32) {                                \
        if (state)                                   \
            GPIO.out_w1ts = (1UL << (pin));          \
        else                                         \
            GPIO.out_w1tc = (1UL << (pin));          \
    } else {                                         \
        if (state)                                   \
            GPIO.out1_w1ts.val = (1UL << ((pin) - 32)); \
        else                                         \
            GPIO.out1_w1tc.val = (1UL << ((pin) - 32)); \
    }

/// INLINE FUNCTION ///////////////////////////////////////////////////////////////////////////////

#include "lcd32Def.h"

static inline __attribute__((always_inline)) 
void __lcd32SetWritePin(lcd32Dev_t *dev, uint8_t state){

}

static inline  __attribute__((always_inline))
void __lcd32SetReadPin(lcd32Dev_t *dev, uint8_t state){
    lcd32GPIOSetState(dev->controlPin.r, state);
}

static inline  __attribute__((always_inline))
void __lcd32SetRegSelPin(lcd32Dev_t *dev, uint8_t state){
    lcd32GPIOSetState(dev->controlPin.rs, state);
}

static inline  __attribute__((always_inline))
void __lcd32SetChipSelPin(lcd32Dev_t *dev, uint8_t state){
    lcd32GPIOSetState(dev->controlPin.cs, state);
}

static inline  __attribute__((always_inline))
void __lcd32SetResetPin(lcd32Dev_t *dev, uint8_t state){
    lcd32GPIOSetState(dev->controlPin.rst, state);
}

static inline  __attribute__((always_inline))
void __lcd32SetBrightnessLightPin(lcd32Dev_t *dev, uint8_t state){
    lcd32GPIOSetState(dev->controlPin.bl, state);
}

static inline  __attribute__((always_inline))
void __lcd32SetParallelData(lcd32Dev_t *dev, uint16_t data16){
    lcd32GPIOSetState(dev->dataPin.__0, data16 & 0x0001);
    lcd32GPIOSetState(dev->dataPin.__1, data16 & 0x0002);
    lcd32GPIOSetState(dev->dataPin.__2, data16 & 0x0004);
    lcd32GPIOSetState(dev->dataPin.__3, data16 & 0x0008);
    lcd32GPIOSetState(dev->dataPin.__4, data16 & 0x0010);
    lcd32GPIOSetState(dev->dataPin.__5, data16 & 0x0020);
    lcd32GPIOSetState(dev->dataPin.__6, data16 & 0x0040);
    lcd32GPIOSetState(dev->dataPin.__7, data16 & 0x0080);
    lcd32GPIOSetState(dev->dataPin.__8, data16 & 0x0100);
    lcd32GPIOSetState(dev->dataPin.__9, data16 & 0x0200);
    lcd32GPIOSetState(dev->dataPin.__10, data16 & 0x0400);
    lcd32GPIOSetState(dev->dataPin.__11, data16 & 0x0800);
    lcd32GPIOSetState(dev->dataPin.__12, data16 & 0x1000);
    lcd32GPIOSetState(dev->dataPin.__13, data16 & 0x2000);
    lcd32GPIOSetState(dev->dataPin.__14, data16 & 0x4000);
    lcd32GPIOSetState(dev->dataPin.__15, data16 & 0x8000);
}

static inline __attribute__((always_inline))
void __lcd32WriteCommand(lcd32Dev_t *dev, uint16_t cmd){
    lcd32Log1("__lcd32WriteCommand(%p, 0x%02x)", dev, cmd);

    __lcd32SetRegSelPin(dev, 0);     // RS = 0 → Command
    __lcd32SetChipSelPin(dev, 0);    // CS = 0 → Select LCD

    __lcd32SetParallelData(dev, cmd);

    __lcd32SetReadPin(dev, 1);

    esp_rom_delay_us(5);
    __lcd32SetWritePin(dev, 0);     // WR falling edge → latch
    esp_rom_delay_us(5);
    __lcd32SetWritePin(dev, 1);
    esp_rom_delay_us(5);

    __lcd32SetChipSelPin(dev, 1);    // CS = 1 → Deselect
}

static inline __attribute__((always_inline))
void __lcd32WriteData(lcd32Dev_t *dev, uint16_t data){
    lcd32Log1("__lcd32WriteData(%p, 0x%02x)", dev, data);

    __lcd32SetRegSelPin(dev, 1);     // RS = 1 → Data
    __lcd32SetChipSelPin(dev, 0);    // CS = 0 → Select LCD

    __lcd32SetParallelData(dev, data);

    __lcd32SetReadPin(dev, 1);

    esp_rom_delay_us(5);
    __lcd32SetWritePin(dev, 0);      // WR falling edge → latch
    esp_rom_delay_us(5);
    __lcd32SetWritePin(dev, 1);
    esp_rom_delay_us(5);

    __lcd32SetChipSelPin(dev, 1);    // CS = 1 → Deselect
}

#endif