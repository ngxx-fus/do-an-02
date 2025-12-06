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

/// @brief Define a log section for this component
#define ANALYZER_READER_LOG_SECTION

/// @brief Enable/Disable the Analyzer Reader (Slave) task.
#define ANALYZER_READER_EN           1

/* --- SPI SLAVE CONFIGURATION (Must match Master's config) --- */

/// @brief The SPI host to use for communication.
#define ANALYZER_READER_SPI_HOST     SPI2_HOST
/// @brief GPIO for SPI Clock.
#define ANALYZER_READER_SPI_SCLK     41
/// @brief GPIO for SPI Master Out Slave In (Slave's input).
#define ANALYZER_READER_SPI_MOSI     39
/// @brief GPIO for SPI Master In Slave Out (Slave's output).
#define ANALYZER_READER_SPI_MISO     40
/// @brief GPIO for SPI Chip Select.
#define ANALYZER_READER_SPI_CS       42
/// @brief GPIO to signal the master that data is ready.
#define ANALYZER_READER_PIN_READY    38

/// @brief Task to handle communication with the Analyzer Master device over SPI (acting as Slave).
void TaskAnalyzerReader(void * pv);

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

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_READER_H__