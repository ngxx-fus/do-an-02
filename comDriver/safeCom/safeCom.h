/// @file safeCom.h
/// @brief Safe Communication Library Header
/// @details Implements framing, stuffing, CRC check, circular buffering and multitasking support.

#include "../../config/projectConfig.h"

#if (SAFE_COM_EN == 1)

#ifndef SAFE_COM_H
#define SAFE_COM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/// Maximum number of known frames to track in the list
#define SC_KNOWN_LIST_SIZE              16

/// Size of the internal circular buffer
#define SC_BUFFER_SIZE                  4096

/// Enable Hardware CRC calculation (1: Enable, 0: Disable)
#define SC_HW_CRC_EN                    0

/// Header byte definition
#define SC_HEADER_BYTE                  0xFF

/// Trailer byte definition
#define SC_TRAILER_BYTE                 0x00

/// Enable Multi-tasking/Polling mode (1: Polling/ISR safe, 0: Direct process)
#define SC_MULTI_TASK_EN                1

/// Default timeout in milliseconds for blocking operations
#define SC_DEF_WAIT_TIME                3000

/* Stuffing Configuration */

/// Escape Byte for byte stuffing
#define SC_ESC_BYTE                     0xFE

/// XOR Mask to toggle bit for stuffed bytes
#define SC_XOR_MASK                     0x20

#if (SC_MULTI_TASK_EN == 1)
    /* User must define these in projectConfig or linked headers for their OS/Platform */
    #ifndef scTaskYield
        /// User defined yield function (e.g., osDelay(1) or yield())
        #define scTaskYield()           
    #endif
    #ifndef scTaskDelay
        /// User defined delay function (e.g., HAL_Delay(__ms))
        #define scTaskDelay(__ms)       
    #endif
#else
    /// No-op for single task mode
    #define scTaskYield()               /* Do nothing */
    /// Blocking delay implementation required if not OS
    #define scTaskDelay(__ms)           /* Blocking delay implementation required if not OS */
#endif

#if (SC_HW_CRC_EN == 1)
    /// CRC Polynomial for HW (0x1021)
    #define SC_CRC_POLYNOMIAL           0x1021
#else 
    /// CRC Polynomial for SW (0x2243)
    #define SC_CRC_POLYNOMIAL           0x2243  
#endif

/* Logging Macros */
#ifdef __sys_log
    /// System log wrapper
    /// @param ...: Variadic arguments for log message
    #define __sc_log(...)   __sys_log(__VA_ARGS__)
#else
    /// System log wrapper (Disabled)
    /// @param ...: Variadic arguments
    #define __sc_log(...)   /* Do nothing */
#endif

#ifdef __sys_log1
    /// System log level 1 wrapper
    /// @param ...: Variadic arguments for log message
    #define __sc_log1(...)  __sys_log1(__VA_ARGS__)
#else
    /// System log level 1 wrapper (Disabled)
    /// @param ...: Variadic arguments
    #define __sc_log1(...)  /* Do nothing */
#endif

#ifdef __sys_err
    /// System error log wrapper
    /// @param ...: Variadic arguments for error message
    #define __sc_err(...)   __sys_err(__VA_ARGS__)
#else
    /// System error log wrapper (Disabled)
    /// @param ...: Variadic arguments
    #define __sc_err(...)   /* Do nothing */
#endif

#ifdef __entry
    /// Function entry trace wrapper
    /// @param ...: Variadic arguments for trace
    #define __sc_entry(...)   __entry(__VA_ARGS__)
#else
    /// Function entry trace wrapper (Disabled)
    /// @param ...: Variadic arguments
    #define __sc_entry(...)   /* Do nothing */
#endif

#ifdef __exit
    /// Function exit trace wrapper
    /// @param ...: Variadic arguments for trace
    #define __sc_exit(...)   __exit(__VA_ARGS__)
#else
    /// Function exit trace wrapper (Disabled)
    /// @param ...: Variadic arguments
    #define __sc_exit(...)   /* Do nothing */
#endif

/* ========================================================================== */
/* TYPEDEFS                                                                   */
/* ========================================================================== */

/// User defined Byte type (uint8_t)
typedef uint8_t             scByte_t;
/// User defined Word type (uint16_t)
typedef uint16_t            scWord_t;
/// User defined Size type (int)
typedef int                 scSize_t;
/// User defined Return Value type (int)
typedef int                 scReturnVal_t;

/// Enumeration for library return values and errors
enum scReturnVal_e  {
    /// Operation successful
    SC_OKE              = 0,
    /// General error
    SC_ERR              = -1,
    /// Null pointer argument error
    SC_ERR_NULL         = -2,
    /// Invalid argument error
    SC_ERR_ARG          = -3,
    /// CRC validation failed
    SC_ERR_CRC          = -4,
    /// Buffer or List empty
    SC_ERR_EMPTY        = -5,
    /// Buffer or List full
    SC_ERR_FULL         = -6,
    /// Operation timed out
    SC_ERR_TIMEOUT      = -7
};

/// Enumeration for 4-bit ID
enum scKnownID_e {
    SC_ID_CMD_MODE  = 0x10, /// [M->S] Config/Write operations
    SC_ID_REQ_DATA  = 0x11, /// [M->S] Read Request operations
    SC_ID_CHK_STAT  = 0x12, /// [M->S] Check Status
    SC_ID_RET_DATA  = 0x20, /// [S->M] Response with Data
    SC_ID_RET_STAT  = 0x21  /// [S->M] Response with Status (ACK/NACK)
};

enum scCmdMode_e {
    CMD_GPIO_SET_DIR = 0x01, // Set Input/Output (Payload: Mask 4 bit)
    CMD_GPIO_WRITE   = 0x02, // Write High/Low (Payload: Mask 4 bit)
    CMD_SPI_CONFIG   = 0x03, // Config SPI (Speed, Mode)
    CMD_I2C_CONFIG   = 0x04  // Config I2C (Address, Speed)
};

enum scReqData_e {
    REQ_READ_GPIO    = 0x01, // Read 4 GPIO status
    REQ_READ_ANALOG  = 0x02, // Read ADC Value (12-bit)
    REQ_READ_SPI_BLK = 0x03, // Read block from SPI Device
    REQ_READ_I2C_BLK = 0x04  // Read block from I2C Device
};

enum scStatus_e {
    STAT_ACK_OK      = 0x00,
    STAT_ERR_PARAM   = 0x01,
    STAT_ERR_BUSY    = 0x02,
    STAT_ERR_HW      = 0x03
};


/* ========================================================================== */
/* GLOBALS (EXTERN)                                                           */
/* ========================================================================== */

/// Circular buffer storage array
extern scByte_t __scBuffer[SC_BUFFER_SIZE];

/// List of detected Frame IDs
extern scByte_t __scIDKnownList[SC_KNOWN_LIST_SIZE];

/// List of frame positions in buffer (4-bit High: Type, 12-bit Low: Index)
extern scWord_t __scIDKnownPosList[SC_KNOWN_LIST_SIZE];

/* ========================================================================== */
/* PROTOTYPES                                                                 */
/* ========================================================================== */

/// @brief Compute CRC16 (Accumulative)
/// @param iPrev Pointer to previous CRC value
/// @param iWord Data word/byte to add to CRC
/// @param oWord Pointer to store output CRC result
/// @return Computed CRC value
scWord_t __scCRC16Cumpute(scWord_t* iPrev, scWord_t iWord, scWord_t* oWord);

/// @brief Encode data into a safe frame: [HEADER] [STUFFED_DATA] [STUFFED_CRC] [TRAILER]
/// @param iArr Pointer to input data array
/// @param iSize Size of input data
/// @param oArr Pointer to output frame buffer
/// @param oArrSize Size of output frame buffer
/// @return SC_OKE if successful, otherwise error code
scReturnVal_t scEncode(void* iArr, scSize_t iSize, void* oArr, scSize_t oArrSize);

/// @brief Decode a raw stuffed array (Helper function)
/// @param iArr Pointer to input stuffed frame
/// @param iSize Size of input stuffed frame
/// @param oArr Pointer to output decoded data buffer
/// @param oArrSize Size of output buffer
/// @return SC_OKE if successful, otherwise error code
scReturnVal_t scDecode(void* iArr, scSize_t iSize, void* oArr, scSize_t oArrSize);

/// @brief Non-blocking push byte to buffer (ISR safe)
/// @param iByte Byte to push into buffer
/// @return SC_OKE if successful, otherwise error code
scReturnVal_t scPushByte(scByte_t iByte);

/// @brief Blocking push multiple bytes to buffer
/// @param iArr Pointer to data array
/// @param iSize Number of bytes to push
/// @return SC_OKE if successful, otherwise error code
scReturnVal_t scPushBytes(void* iArr, scSize_t iSize);

/// @brief Check if there are valid new frames in the queue
/// @return SC_OKE if frames available, SC_ERR otherwise
scReturnVal_t scHasNewFrame(void);

/// @brief Process the buffer to detect frames (Polling Task)
void scPooling(void);

/// @brief Create a new OS task for pooling mechanism
void scNewPoolingTask(void);

/* --- Getter Functions (Blocking with Timeout) --- */

/// @brief Get Header Byte of the oldest frame (Legacy)
/// @param oHeaderByte Pointer to store header byte
/// @return SC_OKE if successful
scReturnVal_t scGetHeaderByte(void* oHeaderByte);

/// @brief Get Trailer Byte of the oldest frame (Legacy)
/// @param oTrailerByte Pointer to store trailer byte
/// @return SC_OKE if successful
scReturnVal_t scGetTrailerByte(void* oTrailerByte);

/// @brief Get Data of the Oldest valid frame with timeout
/// @param oArr Pointer to output buffer
/// @param oSize Size of output buffer
/// @param timeout Timeout in ms (<0: Default, 0: No wait)
/// @return SC_OKE if successful
scReturnVal_t scGetDataByte(scByte_t* oArr, scSize_t oSize, int32_t timeout);

/// @brief Get the Latest received frame (LIFO) without removing it
/// @param oHeader Pointer to store header byte
/// @param oTrailer Pointer to store trailer byte
/// @param oDataArr Pointer to store data payload
/// @param oArrSize Size of data payload buffer
/// @param timeout Timeout in ms (<0: Default, 0: No wait)
/// @return SC_OKE if successful
scReturnVal_t scGetLatestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oDataArr, scSize_t oArrSize, int32_t timeout);

/// @brief Get the Oldest received frame (FIFO) without removing it
/// @param oHeader Pointer to store header byte
/// @param oTrailer Pointer to store trailer byte
/// @param oDataArr Pointer to store data payload
/// @param oArrSize Size of data payload buffer
/// @param timeout Timeout in ms (<0: Default, 0: No wait)
/// @return SC_OKE if successful
scReturnVal_t scGetOldestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oDataArr, scSize_t oArrSize, int32_t timeout);

/// @brief Get and remove the Latest frame (LIFO)
/// @param oHeader Pointer to store header byte
/// @param oTrailer Pointer to store trailer byte
/// @param oArr Pointer to store data payload
/// @param oSize Size of data payload buffer
/// @param timeout Timeout in ms (<0: Default, 0: No wait)
/// @return SC_OKE if successful
scReturnVal_t scRemoveLatestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oArr, scSize_t oSize, int32_t timeout);

/// @brief Get and remove the Oldest frame (FIFO)
/// @param oHeader Pointer to store header byte
/// @param oTrailer Pointer to store trailer byte
/// @param oArr Pointer to store data payload
/// @param oSize Size of data payload buffer
/// @param timeout Timeout in ms (<0: Default, 0: No wait)
/// @return SC_OKE if successful
scReturnVal_t scRemoveOldestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oArr, scSize_t oSize, int32_t timeout);

/// @brief Count number of valid frames currently in the queue
/// @return Number of frames
scSize_t scCountFrame(void);

#ifdef __cplusplus
}
#endif

#endif /* SAFE_COM_H */
#endif /* SAFE_COM_EN */