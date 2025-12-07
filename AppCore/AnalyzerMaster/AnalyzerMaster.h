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

#include "../__Shared/AnalyzerConfig.h"

/// @brief Define a log section for this component
#define ANALYZER_MASTER_LOG_SECTION
#define ANALYZER_MASTER_SPI_CONFIG_SECTION

/// @brief Enable/Disable the Analyzer Reader task and its SPI initialization.
#define ANALYZER_READER_COM_EN          1

#ifdef ANALYZER_MASTER_SPI_CONFIG_SECTION

    /// @brief The SPI host to use for communication with the reader device.
    #define ANALYZER_READER_SPI_HOST    SPI2_HOST

    #ifndef ANALYZER_READER_SPI_FREQ
        /// @brief SPI freqency for Analyzer-Reader communication (5MHz).
        #define ANALYZER_READER_SPI_FREQ    5000000
    #endif /// ANALYZER_READER_SPI_FREQ

    #ifndef ANALYZER_MASTER_RX_SIZE
        /// @brief RX buffer size (unit: number of HalfWord_t elements).
        /// @note  Used for receiving data from Reader.
        #define ANALYZER_MASTER_RX_SIZE     (2048)
    #endif /// ANALYZER_MASTER_RX_SIZE
    #ifndef ANALYZER_MASTER_TX_SIZE
        /// @brief TX buffer size (unit: number of HalfWord_t elements).
        /// @note  Used for sending commands to Reader.
        #define ANALYZER_MASTER_TX_SIZE     (64)
    #endif /// ANALYZER_MASTER_TX_SIZE

    #ifndef ANALYZER_MASTER_SPI_SCLK
        /// @brief GPIO for SPI Clock (SCK).
        #define ANALYZER_MASTER_SPI_SCLK    41
    #endif /// ANALYZER_MASTER_SPI_SCLK
    #ifndef ANALYZER_MASTER_SPI_MOSI
        /// @brief GPIO for SPI Master Out Slave In (MOSI).
        #define ANALYZER_MASTER_SPI_MOSI    39
    #endif /// ANALYZER_MASTER_SPI_MOSI
    #ifndef ANALYZER_MASTER_SPI_MISO
        /// @brief GPIO for SPI Master In Slave Out (MISO).
        #define ANALYZER_MASTER_SPI_MISO    40
    #endif /// ANALYZER_MASTER_SPI_MISO
    #ifndef ANALYZER_MASTER_SPI_CS
        /// @brief GPIO for SPI Chip Select (CS).
        #define ANALYZER_MASTER_SPI_CS      42
    #endif /// ANALYZER_MASTER_SPI_CS
    #ifndef ANALYZER_READER_PIN_READY
        /// @brief GPIO to check if the reader device is ready with data (Handshake/Interrupt).
        #define ANALYZER_READER_PIN_READY   38
    #endif /// ANALYZER_READER_PIN_READY

#endif /// ANALYZER_MASTER_SPI_CONFIG_SECTION

/// @brief External reference to the LCD32 device instance.
extern LCD32Dev_t *     lcd32; 

/// @brief Current configured size of the RX buffer.
extern const uint32_t   AnalyzerMasterRxSize;
/// @brief Current configured size of the TX buffer.
extern const uint32_t   AnalyzerMasterTxSize;
/// @brief Pointer to the RX data buffer.
extern uint16_t *       AnalyzerMasterRx;
/// @brief Pointer to the TX data buffer.
extern uint16_t *       AnalyzerMasterTx;
/// @brief 
extern CBuff_t *        AnalyzerMasterCBuff;

/// @brief Perform a visual test sequence on the LCD screen.
/// @param lcd32 Pointer to the LCD device structure.
void PerformScreenTest(LCD32Dev_t * lcd32);

/// @brief FreeRTOS task for handling LCD screen updates and UI logic.
/// @param pv Parameters passed to the task (usually NULL).
void TaskScreen(void * pv);

/// @brief FreeRTOS task to handle SPI communication with the Analyzer Reader device.
/// @param pv Parameters passed to the task (usually NULL).
void TaskAnalyzerReaderCom(void * pv);

#ifdef ANALYZER_MASTER_LOG_SECTION
    #define AM_LOG_EN       1
    #define AM_ERR_EN       1
    #define AM_LOG_ENTRY    1
    #define AM_LOG_EXIT     1

    #if (AM_LOG_EN == 1)
        #define AMLog(...)      SysLog(__VA_ARGS__)
    #else 
        #define AMLog(...)
    #endif 

    #if (AM_ERR_EN == 1)
        #define AMErr(...)      SysErr(__VA_ARGS__)
    #else
        #define AMErr(...)
    #endif 

    #if (AM_LOG_EXIT == 1)
        #define AMExit(...)     SysExit(__VA_ARGS__)
    #else
        #define AMExit(...)
    #endif 

    #if (AM_LOG_ENTRY == 1)
        #define AMEntry(...)    SysEntry(__VA_ARGS__)
    #else 
        #define AMEntry(...)
    #endif 
#endif /// ANALYZER_MASTER_LOG_SECTION

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_MASTER_H__
