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

#include "../__Shared/AnalyzerConfig.h"


#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

/// @brief Define a log section for this component
#define ANALYZER_READER_LOG_SECTION

/// @brief Enable/Disable the Analyzer Reader (Slave) task.
#define ANALYZER_READER_COM_EN           1

/* --- SPI SLAVE CONFIGURATION (Must match Master's config) --- */

#ifndef ANALYZER_READER_SPI_HOST
    /// @brief The SPI host to use for communication.
    #define ANALYZER_READER_SPI_HOST     SPI2_HOST
#endif /// ANALYZER_READER_SPI_HOST
#ifndef ANALYZER_READER_SPI_SCLK
    /// @brief GPIO for SPI Clock.
    #define ANALYZER_READER_SPI_SCLK     14
#endif /// ANALYZER_READER_SPI_SCLK
#ifndef ANALYZER_READER_SPI_MOSI
    /// @brief GPIO for SPI Master Out Slave In (Slave's input).
    #define ANALYZER_READER_SPI_MOSI     13
#endif /// ANALYZER_READER_SPI_MOSI
#ifndef ANALYZER_READER_SPI_MISO
    /// @brief GPIO for SPI Master In Slave Out (Slave's output).
    #define ANALYZER_READER_SPI_MISO     12
#endif /// ANALYZER_READER_SPI_MISO
#ifndef ANALYZER_READER_SPI_CS
    /// @brief GPIO for SPI Chip Select.
    #define ANALYZER_READER_SPI_CS       15
#endif /// ANALYZER_READER_SPI_CS
#ifndef ANALYZER_READER_PIN_READY
    /// @brief GPIO to signal the master that data is ready.
    #define ANALYZER_READER_PIN_READY    2
#endif /// ANALYZER_READER_PIN_READY
#ifndef ANALYZER_READER_RX_SIZE
    /// @brief RX buffer size (unit: number of HalfWord_t elements).
    /// @note  Used for receiving data from Master (e.g. for config).
    #define ANALYZER_READER_RX_SIZE     (64)
#endif /// ANALYZER_READER_RX_SIZE
#ifndef ANALYZER_READER_TX_SIZE
    /// @brief TX buffer size (unit: number of HalfWord_t elements).
    /// @note  Used for sending data to Master. Must be >= master's RX size.
    #define ANALYZER_READER_TX_SIZE     (64)
#endif /// ANALYZER_READER_TX_SIZE

#if (EN_DRIVER_SPI_TOUCH == ENABLE)

    #ifndef ANALYZER_READER_LCD_T_CS
        /// @brief GPIO for Touch Screen Chip Select.
        #define ANALYZER_READER_LCD_T_CS    4
    #endif

    #ifndef ANALYZER_READER_LCD_CLK
        /// @brief GPIO for Touch Screen Clock.
        #define ANALYZER_READER_LCD_CLK     0
    #endif

    #ifndef ANALYZER_READER_LCD_PEN
        /// @brief GPIO for Touch Screen Pen Interrupt.
        #define ANALYZER_READER_LCD_PEN     16
    #endif

    #ifndef ANALYZER_READER_LCD_F_CS
        /// @brief GPIO for Touch Screen Flash CS (Disable it if unused).
        #define ANALYZER_READER_LCD_F_CS    -1
    #endif

    #ifndef ANALYZER_READER_LCD_MISO
        /// @brief GPIO for Touch Screen MISO.
        #define ANALYZER_READER_LCD_MISO    5
    #endif

    #ifndef ANALYZER_READER_LCD_MOSI
        /// @brief GPIO for Touch Screen MOSI.
        #define ANALYZER_READER_LCD_MOSI    17
    #endif

    #ifndef TOUCH_SPI_HOST
        /// @brief SPI Host for Touch (Must be different from Slave SPI).
        /// @note  Slave usually uses SPI2_HOST, so we default to SPI3_HOST.
        #define TOUCH_SPI_HOST              SPI3_HOST
    #endif

#endif /// (EN_DRIVER_SPI_TOUCH == ENABLE)

/// @brief Global variable storing the latest X coordinate of touch.
extern volatile uint16_t ScreenTouchX;

/// @brief Global variable storing the latest Y coordinate of touch.
extern volatile uint16_t ScreenTouchY;

/// @brief Task to handle Touch Screen SPI communication (Master mode).
void TaskScreenTouchSPICom(void * pv);

/// @brief Interrupt Service Routine for Pen Touch.
// void IRAM_ATTR ScreenTouchSPIHandler(void* arg);

/// @brief Task to handle communication with the Analyzer Master device over SPI (acting as Slave).
void TaskAnalyzerMasterCom(void * pv);

/// @brief Task to monitor and print status information for the reader.
void TaskMonitor(void * pv);


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
