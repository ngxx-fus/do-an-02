#ifndef __FIRMWARE_TYPE_H__
#define __FIRMWARE_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

/// @brief [Test device] Sender
#define TYPE_SENDER             0
/// @brief [Test device] Receiver
#define TYPE_RECEIVER           1
/// @brief [Analyzer device] Master
#define TYPE_ANALYZER_MASTER    2
/// @brief [Analyzer device] Reader
#define TYPE_ANALYZER_READER    3

/// @brief Configuration selection
#define FIRMWARE_TYPE   TYPE_RECEIVER

/// @brief Firmware type enumeration
typedef enum {
    FWT_Sender          = TYPE_SENDER,
    FWT_Receiver        = TYPE_RECEIVER,
    FWT_Analyzer_Master = TYPE_ANALYZER_MASTER,
    FWT_Analyzer_Reader = TYPE_ANALYZER_READER
} eFirmwareType;

#if (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)
    #pragma message ("Config: Analyzer Master (UX/UI included)")
#elif (FIRMWARE_TYPE == TYPE_ANALYZER_READER)
    #pragma message ("Config: Analyzer Reader (Signal logic included)")
#endif

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_CONFIG_H */