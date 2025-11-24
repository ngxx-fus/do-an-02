/// @file   lcd32Helper.h
/// @brief  Helper functions and macros for LCD 3.2"; Includes logging, GPIO setup, pin operations, data/command read/write, and canvas utilities.

#ifndef __LCD32_HELPER_H__
#define __LCD32_HELPER_H__

#include "../../helper/general.h"

/// @cond
/// LOG ///////////////////////////////////////////////////////////////////////////////////////////
/// @endcond

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

/// @cond
/// SET-UP GPIO MACROS ////////////////////////////////////////////////////////////////////////////
/// @endcond

#include "lcd32Def.h"

/// @brief [Internal] Configure GPIO pins as `output` based on a 64-bit mask.
esp_err_t __lcd32GPIOConfigOutputM(uint64_t mask);

/// @brief [Internal] Configure GPIO pins as `input` based on a 64-bit mask.
esp_err_t __lcd32GPIOConfigInputM(uint64_t mask);

/// @brief [Internal] [Fast] Configure GPIO pins as `output` based on a 64-bit mask
void __lcd32FastSetDataBusInputM(uint64_t mask);

/// @brief [Internal] [Fast] Configure GPIO pins as `input` based on a 64-bit mask 
void __lcd32FastSetDataBusOutputM(uint64_t mask);

/// @cond
/// SET GPIO STATE MACROS /////////////////////////////////////////////////////////////////////////
/// @endcond

#define __lcd32SetDataTransaction(dev)              \
    __lcd32SetRegSelPin(dev, 1);

#define __lcd32SetCommandTransaction(dev)           \
    __lcd32SetRegSelPin(dev, 0);

#define __lcd32StartTransaction(dev)                \
    __lcd32SetChipSelPin((dev), 0);

#define __lcd32StopTransaction(dev)                 \
    __lcd32SetChipSelPin((dev), 1);

/// Or valid `pin` to `destBitMask`. (Note: A valid pin it not negative)
#define lcd32AddGPIO(destBitMask, pin, failedAction)      if(__isnot_negative(pin)) destBitMask |= __mask64(pin); else failedAction;

/// Macro for check an obj is NULL neither or not NULL with the action if NULL
#define lcd32NULLCheck(obj, objName, funcName, ...)   \
    do {                                              \
        if (__is_null(obj)) {                         \
            lcd32Err("[%s] %s is NULL!", funcName, objName); \
            __VA_OPT__(__VA_ARGS__)                   \
        }                                             \
    } while (0)

/// @brief [Internal] Set GPIO output state (supports GPIO0–GPIO47).
static inline __attribute__((always_inline))
void __lcd32GPIOSetState(uint32_t pin, uint32_t state) {
    if (pin < 32) {
        if (state)
            GPIO.out_w1ts = (1UL << pin);      // Set bit
        else
            GPIO.out_w1tc = (1UL << pin);      // Clear bit
    } else {
        uint32_t shift = pin - 32;
        if (state)
            GPIO.out1_w1ts.val = (1UL << shift);
        else
            GPIO.out1_w1tc.val = (1UL << shift);
    }
}

/// @brief [Internal] Get GPIO state via GPIO struct with range check.
/// @param pin GPIO number (0–47)
/// @return 1 if HIGH, 0 if LOW.
static inline __attribute__((always_inline))
def __lcd32GPIOGetState(uint32_t pin) {
    if (pin < 32) {
        return (GPIO.in >> pin) & 0x1;
    } else {
        return (GPIO.in1.val >> (pin - 32)) & 0x1;
    }
}

/// @brief [Internal] Set state of WRITE pin
static inline __attribute__((always_inline)) 
void __lcd32SetWritePin(lcd32Dev_t *dev, uint32_t state){
    __lcd32GPIOSetState(dev->controlPin.w, state);
}

/// @brief [Internal] Set state of READ pin
static inline  __attribute__((always_inline))
void __lcd32SetReadPin(lcd32Dev_t *dev, uint32_t state){
    __lcd32GPIOSetState(dev->controlPin.r, state);
}

/// @brief [Internal] Set state of RS (register select) pin
static inline  __attribute__((always_inline))
void __lcd32SetRegSelPin(lcd32Dev_t *dev, uint32_t state){
    __lcd32GPIOSetState(dev->controlPin.rs, state);
}

/// @brief [Internal] Set state of CS (chip select) pin
static inline  __attribute__((always_inline))
void __lcd32SetChipSelPin(lcd32Dev_t *dev, uint32_t state){
    __lcd32GPIOSetState(dev->controlPin.cs, state);
}

/// @brief [Internal] Set state of RST (reset) pin
static inline  __attribute__((always_inline))
void __lcd32SetResetPin(lcd32Dev_t *dev, uint32_t state){
    __lcd32GPIOSetState(dev->controlPin.rst, state);
}

/// @brief [Internal] Set state of BL (brightnessLight) pin
static inline  __attribute__((always_inline))
void __lcd32SetBrightnessLightPin(lcd32Dev_t *dev, uint32_t state){
    __lcd32GPIOSetState(dev->controlPin.bl, state);
}

/// @brief [Internal] Set state of 16 parallel data/command pins
static inline  __attribute__((always_inline))
void __lcd32SetParallelData(lcd32Dev_t *dev, uint16_t data16){
    __lcd32GPIOSetState(dev->dataPin.__0, data16 & 0x0001);
    __lcd32GPIOSetState(dev->dataPin.__1, data16 & 0x0002);
    __lcd32GPIOSetState(dev->dataPin.__2, data16 & 0x0004);
    __lcd32GPIOSetState(dev->dataPin.__3, data16 & 0x0008);
    __lcd32GPIOSetState(dev->dataPin.__4, data16 & 0x0010);
    __lcd32GPIOSetState(dev->dataPin.__5, data16 & 0x0020);
    __lcd32GPIOSetState(dev->dataPin.__6, data16 & 0x0040);
    __lcd32GPIOSetState(dev->dataPin.__7, data16 & 0x0080);
    __lcd32GPIOSetState(dev->dataPin.__8, data16 & 0x0100);
    __lcd32GPIOSetState(dev->dataPin.__9, data16 & 0x0200);
    __lcd32GPIOSetState(dev->dataPin.__10, data16 & 0x0400);
    __lcd32GPIOSetState(dev->dataPin.__11, data16 & 0x0800);
    __lcd32GPIOSetState(dev->dataPin.__12, data16 & 0x1000);
    __lcd32GPIOSetState(dev->dataPin.__13, data16 & 0x2000);
    __lcd32GPIOSetState(dev->dataPin.__14, data16 & 0x4000);
    __lcd32GPIOSetState(dev->dataPin.__15, data16 & 0x8000);
}

/// @brief [Internal] Get state of 16 parallel data/command pins
static inline  __attribute__((always_inline))
def __lcd32GetParallelData(lcd32Dev_t *dev){
    uint16_t data16 = 0;
    if(__lcd32GPIOGetState(dev->dataPin.__0)) data16 |= 0x0001;
    if(__lcd32GPIOGetState(dev->dataPin.__1)) data16 |= 0x0002;
    if(__lcd32GPIOGetState(dev->dataPin.__2)) data16 |= 0x0004;
    if(__lcd32GPIOGetState(dev->dataPin.__3)) data16 |= 0x0008;
    if(__lcd32GPIOGetState(dev->dataPin.__4)) data16 |= 0x0010;
    if(__lcd32GPIOGetState(dev->dataPin.__5)) data16 |= 0x0020;
    if(__lcd32GPIOGetState(dev->dataPin.__6)) data16 |= 0x0040;
    if(__lcd32GPIOGetState(dev->dataPin.__7)) data16 |= 0x0080;
    if(__lcd32GPIOGetState(dev->dataPin.__8)) data16 |= 0x0100;
    if(__lcd32GPIOGetState(dev->dataPin.__9)) data16 |= 0x0200;
    if(__lcd32GPIOGetState(dev->dataPin.__10)) data16 |= 0x0400;
    if(__lcd32GPIOGetState(dev->dataPin.__11)) data16 |= 0x0800;
    if(__lcd32GPIOGetState(dev->dataPin.__12)) data16 |= 0x1000;
    if(__lcd32GPIOGetState(dev->dataPin.__13)) data16 |= 0x2000;
    if(__lcd32GPIOGetState(dev->dataPin.__14)) data16 |= 0x4000;
    if(__lcd32GPIOGetState(dev->dataPin.__15)) data16 |= 0x8000;
    return data16;
}

/// @brief [Internal] Make write operation from HOST to LCD (R: 1; W: 0->1) 
#define __LCD32_WRITE_SIG(dev)                        \
    do { \
        __lcd32SetReadPin(dev, 1);\
        __lcd32SetWritePin(dev, 0);\
        __lcd32SetWritePin(dev, 1);\
    } while(0)

/// @brief [Internal] Make read signal from LCD to HOST (R: 0->1; W: 1) 
#define __LCD32_READ_SIG(dev)                         \
    do { \
        __lcd32SetWritePin(dev, 1);\
        __lcd32SetReadPin(dev, 0);\
        __lcd32SetReadPin(dev, 1);\
    } while(0)

/// @brief [Internal] Write command to LCD    
static inline __attribute__((always_inline))
void __lcd32WriteCommand(lcd32Dev_t *dev, uint16_t cmd){
    // lcd32Log1("__lcd32WriteCommand(%p, 0x%02x)", dev, cmd);

    __lcd32SetCommandTransaction(dev);
    __lcd32StartTransaction(dev);
    __lcd32SetParallelData(dev, cmd);
    __LCD32_WRITE_SIG(dev);
    __lcd32StopTransaction(dev);

}

/// @brief [Internal] Write 16 bits data to LCD    
static inline __attribute__((always_inline))
void __lcd32WriteData(lcd32Dev_t *dev, uint16_t data){
    // lcd32Log1("__lcd32WriteData(%p, 0x%02x)", dev, data);

    __lcd32SetDataTransaction(dev);
    __lcd32StartTransaction(dev);
    __lcd32SetParallelData(dev, data);
    __LCD32_WRITE_SIG(dev);
    __lcd32StopTransaction(dev);
}

/// @brief [Internal] Send a command with 16bit data to LCD
static inline __attribute__((always_inline))
void __lcd32WriteRegister(lcd32Dev_t *dev, uint16_t cmd, uint16_t data){
    // lcd32Log1("__lcd32WriteData(%p, 0x%02x)", dev, data);

    __lcd32StartTransaction(dev);

    __lcd32SetCommandTransaction(dev);
    __lcd32SetParallelData(dev, cmd);
    __LCD32_WRITE_SIG(dev);
    
    __lcd32SetDataTransaction(dev);
    __lcd32SetParallelData(dev, data);
    __LCD32_WRITE_SIG(dev);

    __lcd32StopTransaction(dev);
}

/// @brief [Internal] Send a commend the read 16bit from LCD
static inline __attribute__((always_inline))
def __lcd32ReadRegister(lcd32Dev_t *dev, uint16_t cmd){
    // lcd32Log1("__lcd32WriteData(%p, 0x%02x)", dev, data);

    uint16_t receivedData = 0;

    __lcd32StartTransaction(dev);

    __lcd32SetCommandTransaction(dev);
    __lcd32SetParallelData(dev, cmd);
    __LCD32_WRITE_SIG(dev);
    
    __lcd32GPIOConfigInputM(dev->dataPinMask);
    __lcd32SetDataTransaction(dev);
    __LCD32_READ_SIG(dev);
    __LCD32_READ_SIG(dev);
    receivedData = __lcd32GetParallelData(dev);
    __lcd32GPIOConfigOutputM(dev->dataPinMask);

    __lcd32StopTransaction(dev);
    return receivedData;
}

/// @brief [Internal] Start continue reading 16bit from LCD (keep transaction open, keep input mode)
static inline __attribute__((always_inline))
def __lcd32ContReadRegisterStart(lcd32Dev_t *dev, uint16_t cmd){
    // lcd32Log1("__lcd32WriteData(%p, 0x%02x)", dev, data);

    uint16_t receivedData = 0;

    __lcd32StartTransaction(dev);

    __lcd32SetCommandTransaction(dev);
    __lcd32SetParallelData(dev, cmd);
    __LCD32_WRITE_SIG(dev);
    
    __lcd32GPIOConfigInputM(dev->dataPinMask);
    __lcd32SetDataTransaction(dev);
    __LCD32_READ_SIG(dev);
    receivedData = __lcd32GetParallelData(dev);

    return receivedData;
}

/// @brief [Internal] Read new one word (16 bits) from LCD 
static inline __attribute__((always_inline))
def __lcd32ContReadRegister(lcd32Dev_t *dev){
    uint16_t receivedData = 0;

    __LCD32_READ_SIG(dev);
    receivedData = __lcd32GetParallelData(dev);

    return receivedData;
}

/// @brief [Internal] Stop continue reading 16bit from LCD (close transaction)
static inline __attribute__((always_inline))
void __lcd32ContReadRegisterStop(lcd32Dev_t *dev){
    __lcd32StopTransaction(dev);
    __lcd32GPIOConfigOutputM(dev->dataPinMask);
}

/// @cond
/// CANVAS UTILS //////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief [Internal] Perform zero assign using memset
#define lcd32AssignZero(ptr)                memset(ptr, 0, sizeof(ptr))

/// @brief [Internal] Get pointer to a pixel in the canvas (with bounds check)
static inline __attribute__((always_inline))
color_t *__lcd32GetPixelPtr(lcd32Dev_t *dev, dim_t r, dim_t c) {
    if (r >= 0 && r < dev->canvas.maxRow &&
        c >= 0 && c < dev->canvas.maxCol) {
        return &dev->canvas.buff[r][c];
    }
    return NULL;
}

/// @brief [Internal] Get pixel value from canvas (with bounds check)
static inline __attribute__((always_inline))
color_t __lcd32GetPixel(const lcd32Dev_t *dev, dim_t r, dim_t c) {
    if (r >= 0 && r < dev->canvas.maxRow &&
        c >= 0 && c < dev->canvas.maxCol) {
        return dev->canvas.buff[r][c];
    }
    return 0;
}

/// @brief [Internal] Set pixel in canvas (with bounds check)
static inline __attribute__((always_inline))
void __lcd32SetPixel(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color) {
    if (r >= 0 && r < dev->canvas.maxRow &&
        c >= 0 && c < dev->canvas.maxCol) {
        dev->canvas.buff[r][c] = color;
    }
}

/// @brief [Internal] Set pixel in canvas (no check, faster)
static inline __attribute__((always_inline))
void __lcd32SetPixelNC(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color) {
    dev->canvas.buff[r][c] = color;
}

#endif