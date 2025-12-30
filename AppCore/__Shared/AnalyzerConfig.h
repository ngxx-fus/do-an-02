/**
 * @file AnalyzerConfig.h
 * @brief  Configuration parameters for Analyzer Master-Reader protocol.
 * @note   Contains tunable constants (Sizes, IDs, Frequency).
 */

#ifndef __ANALYZER_CONFIG_H__
#define __ANALYZER_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER) || (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#define ANALYZER_MASTER_ID          0xE2        ///< Master Device ID
#define ANALYZER_READER_ID          0xF4        ///< Reader Device ID
#define ANALYZER_READER_SPI_FREQ    5000000     ///< SPI Frequency (5MHz)

#define ANALYZER_READER_ACK         0xFACF      ///< Acknowledge signature
#define ANALYZER_READER_NACK        0x0630      ///< Negative Acknowledge (~ACK)

/// @brief Total Packet Size for both TX and RX.
/// @note  Must be divisible by 2 (for 16-bit alignment).
#ifndef ANALYZER_PKT_SIZE_BYTE
    #define ANALYZER_PKT_SIZE_BYTE      4096
#endif

/// @brief Dedicated size for Touch Data partition within the packet.
#ifndef ANALYZER_TOUCH_SIZE_BYTE
    #define ANALYZER_TOUCH_SIZE_BYTE    1024
#endif

#endif /// FIRMWARE_TYPE Check

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_CONFIG_H__