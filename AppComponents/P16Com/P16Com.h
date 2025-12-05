/**
 * @file P16Com.h
 * @brief 16-bit Parallel Communication Driver Definitions
 * @details This driver manages a 16-bit parallel bus interface (8080/6800 style).
 * It handles pin configurations, read/write timing sequences, and buffer operations.
 * @author Nguyen Thanh Phu
 */

#ifndef __P16_COM_H__
#define __P16_COM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppComponents/P16Com/P16Com.h")
#endif /// PRINT_HEADER_COMPILE_MESSAGE

#include <stdint.h>
#include <stdlib.h>

#include "AppConfig/All.h"
#include "AppUtils/All.h"
#include "AppESPWrap/All.h"

#define P16COM_LOG_SECTION
#define P16COM_UTILS_SECTION

/// @brief Perform check before doing anything (Safety guard)
#define P16COM_INIT_CHECK_EN            1

#ifndef P16BlockingDelay
    /// @brief Wrapper for blocking delay (microseconds)
    #define P16BlockingDelay(usec)      ets_delay_us(usec)
#endif

#ifndef P16ClockCycle
    /// @brief Total cycle time for write/read operation (in micro-seconds)
    /// @note Adjust this based on the speed of the external device
    #define P16ClockCycle 2
#endif

#ifdef P16ClockCycle
    /// @brief Half cycle time for signal latching
    #define P16HalfClockCycle  ((P16ClockCycle)>>1)
#endif

typedef uint16_t    P16Data_t;
typedef int32_t     P16Size_t;

/* --- LOGGING CONFIGURATION --- */
/// @brief Enable standard logging for this module
#define P16COM_LOG_EN                   1
/// @brief Enable verbose/detailed logging
#define P16COM_LOG1_EN                  1
/// @brief Enable error logging
#define P16COM_ERR_EN                   1
/// @brief Enable function entry tracing
#define P16COM_LOG_ENTRY                1
/// @brief Enable function exit tracing
#define P16COM_LOG_EXIT                 1

/* --- PIN CONFIGURATION --- */
/// @brief Number of actual Control Pins used (Read, Write, CS, RS, Reset)
#define P16COM_CTL_PIN_NUM              5

/// @brief Number of Parallel Data Pins (16-bit bus)
#define P16COM_DAT_PIN_NUM              16

/// @brief Total Control Array size including padding (Aligned to 8)
#define P16COM_CTL_PIN_NUM_W_PADDING    8

/// @brief Total Data Array size (Aligned to 16)
#define P16COM_DAT_PIN_NUM_W_PADDING    16

/// @brief Status flags for the driver
enum P16ComPositiveStatusFlag_e {
    P16COM_INITIALIZED = 0x00000001, ///< Driver is initialized and ready
};

/// @brief Pinout structure for 16-bit parallel communication
/// @details Uses anonymous unions to allow access via specific names (e.g., .Read) 
///          or via arrays (e.g., .CtlPinArr[i]) for bulk configuration.
///          Memory is organized into two contiguous blocks: Control and Data.
typedef struct P16ComPin_t {
    /// @brief Control Pins Group (Union)
    union {
        /// @brief Individual Control Pin Access
        struct {
            Pin_t   Read;       ///< Read signal pin (Active Low)
            Pin_t   Write;      ///< Write signal pin (Active Low)
            Pin_t   ChipSel;    ///< Chip Select pin (Active Low)
            Pin_t   RegSel;     ///< Register Select / Command-Data pin
            Pin_t   Reset;      ///< Reset signal pin (Active Low)
            Pin_t   Reserved0;  ///< Reserved/Padding pin
            Pin_t   Reserved1;  ///< Reserved/Padding pin
            Pin_t   Reserved2;  ///< Reserved/Padding pin
        };
        /// @brief Array access for Control Pins (Size: 8)
        Pin_t CtlPinArr[P16COM_CTL_PIN_NUM_W_PADDING];
    };

    /// @brief Data Pins Group (Union)
    union {
        /// @brief Individual Data Pin Access (D0-D15)
        struct {
            Pin_t   D0, D1, D2, D3;     ///< Data bits 0-3
            Pin_t   D4, D5, D6, D7;     ///< Data bits 4-7
            Pin_t   D8, D9, D10, D11;   ///< Data bits 8-11
            Pin_t   D12, D13, D14, D15; ///< Data bits 12-15
        };
        /// @brief Array access for Data Pins (Size: 16)
        Pin_t DatPinArr[P16COM_DAT_PIN_NUM_W_PADDING];
    };
    /// @brief Internal status flags
    uint32_t StatusFlag;
} P16ComPin_t;

/* --- FUNCTION PROTOTYPES --- */

/// @brief Allocates memory for a new P16ComPin_t object
/// @return Pointer to the new object or NULL if malloc fails
P16ComPin_t * P16ComNew();

/// @brief Initializes the GPIOs for the parallel interface
/// @param Dev Pointer to the P16ComPin_t object
/// @return STAT_OKE on success, STAT_ERR on failure
DefaultRet_t        P16ComInit(P16ComPin_t * Dev);

/// @brief Performs a hardware reset sequence using the Reset pin
/// @param Dev Pointer to the P16ComPin_t object
void                P16ComMakeReset(P16ComPin_t * Dev);

/// @brief Writes a single 16-bit value to the bus
/// @param Dev Pointer to the P16ComPin_t object
/// @param Data The 16-bit value to write
void                P16ComWrite(P16ComPin_t * Dev, P16Data_t Data);

/// @brief Writes an array of 16-bit values to the bus (Burst Write)
/// @param Dev Pointer to the P16ComPin_t object
/// @param DataArr Pointer to the data array
/// @param Size Number of elements to write
void                P16ComWriteArray(P16ComPin_t * Dev, P16Data_t * DataArr, P16Size_t Size);

/// @brief Reads a single 16-bit value from the bus
/// @param Dev Pointer to the P16ComPin_t object
/// @return The 16-bit value read from the bus
P16Data_t           P16ComRead(P16ComPin_t * Dev);

/// @brief Reads an array of 16-bit values from the bus (Burst Read)
/// @param Dev Pointer to the P16ComPin_t object
/// @param pBuff Pointer to the buffer to store data
/// @param Size Number of elements to read
void                P16ComReadArray(P16ComPin_t * Dev, P16Data_t * pBuff, P16Size_t Size);

#ifdef P16COM_LOG_SECTION

    #if (P16COM_LOG_EN == 1)
        /// @brief Log standard info message
        #define P16Log(...)                 SysLog(__VA_ARGS__)
    #else 
        #define P16Log(...)
    #endif 

    #if (P16COM_ERR_EN == 1)
        /// @brief Log error message
        #define P16Err(...)                 SysErr(__VA_ARGS__)
    #else
        #define P16Err(...)
    #endif 

    #if (P16COM_LOG_EXIT == 1)
        /// @brief Log function exit
        #define P16Exit(...)                SysExit(__VA_ARGS__)
        
        /// @brief Log function exit and return a value
        #define P16ReturnWithLog(ret, ...)  SysExit(__VA_ARGS__); return ret;
    #else
        #define P16Exit(...)
        #define P16ReturnWithLog(ret, ...)  return ret;
    #endif 

    #if (P16COM_LOG_ENTRY == 1)
        /// @brief Log function entry
        #define P16Entry(...)               SysEntry(__VA_ARGS__)
    #else 
        #define P16Entry(...)
    #endif 

    #if (P16COM_LOG1_EN == 1)
        /// @brief Log verbose/detailed message
        #define P16Log1(...)                SysLogVer(__VA_ARGS__)
    #else 
        #define P16Log1(...)
    #endif 

#endif /// P16COM_LOG_SECTION

#ifdef P16COM_UTILS_SECTION

    /// @brief Set Read Pin High (Inactive)
    #define P16SetHighReadPin(p16Dev)     IOStandardSet(1ULL << (p16Dev)->Read)

    /// @brief Set Write Pin High (Inactive)
    #define P16SetHighWritePin(p16Dev)    IOStandardSet(1ULL << (p16Dev)->Write)

    /// @brief Set Register Select Pin High (Command)
    #define P16SetHighRegSelPin(p16Dev)   IOStandardSet(1ULL << (p16Dev)->RegSel)

    /// @brief Set Chip Select Pin High (Inactive)
    #define P16SetHighChipSelPin(p16Dev)  IOStandardSet(1ULL << (p16Dev)->ChipSel)

    /// @brief Set Read Pin Low (Active)
    #define P16SetLowReadPin(p16Dev)      IOStandardClr(1ULL << (p16Dev)->Read)

    /// @brief Set Write Pin Low (Active)
    #define P16SetLowWritePin(p16Dev)     IOStandardClr(1ULL << (p16Dev)->Write)

    /// @brief Set Register Select Pin Low (Data)
    #define P16SetLowRegSelPin(p16Dev)    IOStandardClr(1ULL << (p16Dev)->RegSel)

    /// @brief Set Chip Select Pin Low (Active)
    #define P16SetLowChipSelPin(p16Dev)   IOStandardClr(1ULL << (p16Dev)->ChipSel)

    /// @brief Get current state of Read Pin
    #define P16GetReadPin(p16Dev)         ((IOStandardGet() >> (p16Dev)->Read) & 0x1)

    /// @brief Get current state of Write Pin
    #define P16GetWritePin(p16Dev)        ((IOStandardGet() >> (p16Dev)->Write) & 0x1)

    /// @brief Get current state of Register Select Pin
    #define P16GetRegSelPin(p16Dev)       ((IOStandardGet() >> (p16Dev)->RegSel) & 0x1)

    /// @brief Get current state of Chip Select Pin
    #define P16GetChipSelPin(p16Dev)      ((IOStandardGet() >> (p16Dev)->ChipSel) & 0x1)

    /// @brief Set Reset Pin High (Inactive)
    #define P16SetHighResetPin(p16Dev)    IOStandardSet(1ULL << (p16Dev)->Reset)

    /// @brief Set Reset Pin Low (Active)
    #define P16SetLowResetPin(p16Dev)     IOStandardClr(1ULL << (p16Dev)->Reset)

    /// @brief Get current state of Reset Pin
    #define P16GetResetPin(p16Dev)        ((IOStandardGet() >> (p16Dev)->Reset) & 0x1)

    /// @brief Set a specific Data Pin High (by index 0-15)
    #define P16SetHighDataPin(p16Dev, n)  IOStandardSet(1ULL << (p16Dev)->DatPinArr[n])

    /// @brief Set a specific Data Pin Low (by index 0-15)
    #define P16SetLowDataPin(p16Dev, n)   IOStandardClr(1ULL << (p16Dev)->DatPinArr[n])

    /// @brief Get current state of a specific Data Pin (by index 0-15)
    #define P16GetDataPin(p16Dev, n)      ((IOStandardGet() >> (p16Dev)->DatPinArr[n]) & 0x1)

#endif /// P16COM_UTILS_SECTION

#ifdef __cplusplus
}
#endif

#endif /// __P16_COM_H__