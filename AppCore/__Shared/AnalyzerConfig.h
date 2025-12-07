#ifndef __ANALYZER_COMMANDS_H__
#define __ANALYZER_COMMANDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER) || (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#include <stdint.h>
#include <stdlib.h>

/// @brief RX buffer size (unit: number of HalfWord_t elements).
/// @note  Used for receiving data from Reader.
#define ANALYZER_MASTER_RX_SIZE     (2048)
/// @brief TX buffer size (unit: number of HalfWord_t elements).
/// @note  Used for sending commands to Reader.
#define ANALYZER_MASTER_TX_SIZE     (2048)
/// @brief SPI freqency for Analyzer-Reader communication (5MHz).
#define ANALYZER_READER_SPI_FREQ    5000000

/// @brief Identifier for the Master device.
#define ANALYZER_MASTER_ID          0x32

/// @brief Identifier for the Reader device.
#define ANALYZER_READER_ID          0x04

/// @brief Command definitions for Analyzer Master-Reader protocol.
enum AnalyzerCmds_e {
    AM_CMD_REQ_ID       = 0x10, /// Request Reader ID (+ 01 halfword Master ID).
    AM_CMD_REQ_TEST     = 0x11, /// Request random test data (+ 01 halfword size).
    AM_CMD_REQ_STAT     = 0x12, /// Request status from Reader.
    AM_CMD_REQ_MODE     = 0x13, /// Request current read mode.
    AM_CMD_REQ_DATA     = 0x14, /// Request measurement data from Reader.
    AM_CMD_SEL_REG      = 0x22, /// Select register/config (+ 01 halfword argument).
    AM_CMD_SET_DATA     = 0x23, /// Send data for config reader (+ n halfword arguments).
    AM_CMD_SET_SIZE_TX  = 0x31, /// Config Reader TX buffer size (+ 01 halfword arg, MAX=65535).
    AM_CMD_SET_SIZE_RX  = 0x32, /// Config Reader RX buffer size (+ 01 halfword arg, MAX=65535).
};


/// @brief Half-Word type for Analyzer-Reader communication.
typedef uint16_t HalfWord_t;


#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_READER) || ((FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_COMMANDS_H__

