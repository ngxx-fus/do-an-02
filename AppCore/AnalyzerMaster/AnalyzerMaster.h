#ifndef __ANALYZER_MASTER_H__
#define __ANALYZER_MASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppCore/AnalyzerMaster/AnalyzerMaster.h")
#endif

#include "../../AppConfig/All.h"
#include "../../AppUtils/All.h"
#include "../../AppESPWrap/All.h"
#include "../../AppFonts/All.h"

#include "../../AppComponents/CBuff/All.h"
#include "../../AppComponents/LCD32/LCD32.h"

/// @brief Define a log section for this component
#define ANALYZER_MASTER_LOG_SECTION

/// @brief Enable/Disable the Analyzer Reader task and its SPI initialization.
#define ANALYZER_READER_EN          1

/* --- SPI READER CONFIGURATION --- */

/// @brief The SPI host to use for communication with the reader device.
#define ANALYZER_READER_SPI_HOST    SPI2_HOST
/// @brief GPIO for SPI Clock.
#define ANALYZER_MASTER_SPI_SCLK    41
/// @brief GPIO for SPI Master Out Slave In.
#define ANALYZER_MASTER_SPI_MOSI    39
/// @brief GPIO for SPI Master In Slave Out.
#define ANALYZER_MASTER_SPI_MISO    40
/// @brief GPIO for SPI Chip Select.
#define ANALYZER_MASTER_SPI_CS      42
/// @brief GPIO to check if the reader device is ready with data.
#define ANALYZER_READER_PIN_READY   38

extern LCD32Dev_t * lcd32; 

void PerformScreenTest(LCD32Dev_t * lcd32);
void TaskScreen(void * pv);

/// @brief Task to handle communication with the Analyzer Reader device over SPI.
void TaskAnalyzerReader(void * pv);

/* --- LOGGING MACROS --- */
#ifdef ANALYZER_MASTER_LOG_SECTION
    #define AM_LOG_EN           1
    #define AM_ERR_EN           1
    #define AM_LOG_ENTRY        1
    #define AM_LOG_EXIT         1

    #if (AM_LOG_EN == 1)
        #define AMLog(...)                 SysLog(__VA_ARGS__)
    #else 
        #define AMLog(...)
    #endif 

    #if (AM_ERR_EN == 1)
        #define AMErr(...)                 SysErr(__VA_ARGS__)
    #else
        #define AMErr(...)
    #endif 

    #if (AM_LOG_EXIT == 1)
        #define AMExit(...)                SysExit(__VA_ARGS__)
    #else
        #define AMExit(...)
    #endif 

    #if (AM_LOG_ENTRY == 1)
        #define AMEntry(...)               SysEntry(__VA_ARGS__)
    #else 
        #define AMEntry(...)
    #endif 
#endif /// ANALYZER_MASTER_LOG_SECTION

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_MASTER_H__
