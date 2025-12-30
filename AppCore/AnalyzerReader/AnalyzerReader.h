#ifndef __ANALYZER_READER_H__
#define __ANALYZER_READER_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppCore/AnalyzerReader/AnalyzerReader.h")
#endif

#include "../../AppConfig/All.h"
#include "../../AppUtils/All.h"
#include "../../AppESPWrap/All.h"

// Include Shared Definition (Contains Sizes, Types, Structs)
#include "../__Shared/All.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

/// @brief Define a log section for this component
#define ANALYZER_READER_LOG_SECTION

/// @brief Enable/Disable the Analyzer Reader (Slave) task.
#define ANALYZER_READER_COM_EN           1

/* --- SPI SLAVE CONFIGURATION --- */

#ifndef ANALYZER_READER_SPI_HOST
    /// @brief The SPI host to use for communication.
    #define ANALYZER_READER_SPI_HOST     SPI2_HOST
#endif
#ifndef ANALYZER_READER_SPI_SCLK
    #define ANALYZER_READER_SPI_SCLK     14
#endif
#ifndef ANALYZER_READER_SPI_MOSI
    #define ANALYZER_READER_SPI_MOSI     13
#endif
#ifndef ANALYZER_READER_SPI_MISO
    #define ANALYZER_READER_SPI_MISO     12
#endif
#ifndef ANALYZER_READER_SPI_CS
    #define ANALYZER_READER_SPI_CS       15
#endif
#ifndef ANALYZER_READER_PIN_READY
    /// @brief GPIO to signal the master that data is ready.
    #define ANALYZER_READER_PIN_READY    2
#endif

/* --- TOUCH SPI CONFIGURATION --- */
#if (EN_DRIVER_SPI_TOUCH == ENABLE)
    #ifndef ANALYZER_READER_LCD_T_CS
        #define ANALYZER_READER_LCD_T_CS    4
    #endif
    #ifndef ANALYZER_READER_LCD_CLK
        #define ANALYZER_READER_LCD_CLK     0
    #endif
    #ifndef ANALYZER_READER_LCD_PEN
        #define ANALYZER_READER_LCD_PEN     16
    #endif
    #ifndef ANALYZER_READER_LCD_F_CS
        #define ANALYZER_READER_LCD_F_CS    -1
    #endif
    #ifndef ANALYZER_READER_LCD_MISO
        #define ANALYZER_READER_LCD_MISO    5
    #endif
    #ifndef ANALYZER_READER_LCD_MOSI
        #define ANALYZER_READER_LCD_MOSI    17
    #endif
    #ifndef TOUCH_SPI_HOST
        #define TOUCH_SPI_HOST              SPI3_HOST
    #endif
#endif /// (EN_DRIVER_SPI_TOUCH == ENABLE)

/// @brief Global variable storing the latest X coordinate of touch.
extern volatile Word_t ScreenTouchX;

/// @brief Global variable storing the latest Y coordinate of touch.
extern volatile Word_t ScreenTouchY;

/// @brief Task to handle Touch Screen SPI communication.
void TaskScreenTouchSPICom(void * pv);

/// @brief Task to handle communication with the Analyzer Master (Slave Mode).
void TaskAnalyzerMasterCom(void * pv);

/* --- LOGGING MACROS --- */
#ifdef ANALYZER_READER_LOG_SECTION
    #define AR_LOG_EN           1
    #define AR_ERR_EN           1
    #define AR_LOG_ENTRY        1
    #define AR_LOG_EXIT         1

    #if (AR_LOG_EN == 1)
        #define ARLog(...)                 SysLog(__VA_ARGS__)
    #else 
        #define ARLog(...)
    #endif 

    #if (AR_ERR_EN == 1)
        #define ARErr(...)                 SysErr(__VA_ARGS__)
    #else
        #define ARErr(...)
    #endif 

    #if (AR_LOG_EXIT == 1)
        #define ARExit(...)                SysExit(__VA_ARGS__)
    #else
        #define ARExit(...)
    #endif 

    #if (AR_LOG_ENTRY == 1)
        #define AREntry(...)               SysEntry(__VA_ARGS__)
    #else 
        #define AREntry(...)
    #endif 
#endif /// ANALYZER_READER_LOG_SECTION

#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_READER_H__