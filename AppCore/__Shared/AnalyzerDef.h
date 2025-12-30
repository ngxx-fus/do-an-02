/**
 * @file AnalyzerDef.h
 * @brief  Data structures and type definitions for Analyzer protocol.
 * @note   Aggregates Config and Commands to define the memory layout.
 */

#ifndef __ANALYZER_DEF_H__
#define __ANALYZER_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "AnalyzerConfig.h"   // Import Sizes and Constants
#include "AnalyzerCommands.h" // Import Command Enums

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER) || (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

typedef uint16_t Word_t;    ///< 16-bit word for SPI transfer
typedef uint8_t  Byte_t;    ///< 8-bit byte for raw access

/// Convert Byte size to Word_t count (Divide by 2)
#define BYTE2WORD(ByteSize)             ((ByteSize) >> 1)

// Layout Metrics (Unit: Word count)
#define HDR_WORDS           3  // Command(1) + Seq(1) + Size(1)
#define CRC_WORDS           2  // CRC32 (32-bit = 2 words)
#define TOTAL_WORDS         BYTE2WORD(ANALYZER_PKT_SIZE_BYTE)

// Payload Capacity
// Payload = Total - Header - Footer
#define MAX_PAYLOAD_WORDS   (TOTAL_WORDS - HDR_WORDS - CRC_WORDS)
#define TOUCH_PAYLOAD_WORDS BYTE2WORD(ANALYZER_TOUCH_SIZE_BYTE)

/// @brief Union for memory mapping of SPI Packet.
/// @note  Maps exactly to ANALYZER_PKT_SIZE_BYTE.
typedef union {
    
    /// 1. Transmission Map (Tx)
    struct {
        Word_t Command;             ///< Header: Command ID
        Word_t SequenceNumber;      ///< Header: Packet Sequence
        Word_t ArgSize;             ///< Header: Argument Size
        
        /// Payload area
        Word_t Argument[MAX_PAYLOAD_WORDS]; 
        
        Word_t Crc32[CRC_WORDS];    ///< Footer: CRC32
    } Tx;

    /// 2. Touch Response Map (RxTouch)
    struct {
        Word_t Response;            ///< Header: Response ID
        Word_t SequenceNumber;      ///< Header: Packet Sequence
        Word_t TouchSize;           ///< Header: Touch Data Size
        
        /// Padding: Pushes Touch data to the end of payload
        Word_t __Padding[MAX_PAYLOAD_WORDS - TOUCH_PAYLOAD_WORDS];
        
        Word_t Touch[TOUCH_PAYLOAD_WORDS];
        
        Word_t Crc32[CRC_WORDS];    ///< Footer: CRC32
    } RxTouch;

    /// 3. Data Response Map (RxData)
    struct {
        Word_t Response;            ///< Header: Response ID
        Word_t SequenceNumber;      ///< Header: Packet Sequence
        Word_t DataSize;            ///< Header: Data Size
        
        Word_t Data[TOUCH_PAYLOAD_WORDS];   ///< Data starts immediately
        
        /// Padding: Fills remaining payload space
        Word_t __Padding[MAX_PAYLOAD_WORDS - TOUCH_PAYLOAD_WORDS];
        
        Word_t Crc32[CRC_WORDS];    ///< Footer: CRC32
    } RxData;

    /// 4. Raw Access
    Byte_t RawBytes[ANALYZER_PKT_SIZE_BYTE];

} AnalyzerDE_t;

// Compile-time check to ensure Struct size matches Config size
_Static_assert(sizeof(AnalyzerDE_t) == ANALYZER_PKT_SIZE_BYTE, "[ERR] Struct size mismatch with Config!");

#endif /// FIRMWARE_TYPE Check

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_DEF_H__