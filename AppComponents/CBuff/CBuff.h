#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../../AppConfig/All.h"
#include "../../AppUtils/All.h" 

/* --- Configuration Macros --- */

// 1. Thread Safety
#ifndef CBUF_THREAD_SAFE_EN
    #define CBUF_THREAD_SAFE_EN         1
#endif

// 2. Memory Allocation (PSRAM)
#ifndef CBUF_USE_PSRAM_EN
    #define CBUF_USE_PSRAM_EN           1
#endif

// 3. Validation Checks
#ifndef CBUF_ARGS_CHECK_EN
    /// @brief Enable valid arguments check (NULL ptr, size 0)
    #define CBUF_ARGS_CHECK_EN          1 
#endif

#ifndef CBUF_INITIALIZED_CHECK_EN
    /// @brief Enable initialization check (Is buffer assigned? Is Init flag set?)
    #define CBUF_INITIALIZED_CHECK_EN   1
#endif

// 4. Logging Configuration
#ifndef CBUF_LOG_EN
    #define CBUF_LOG_EN                 1
#endif
#ifndef CBUF_ERR_EN
    #define CBUF_ERR_EN                 1
#endif
#ifndef CBUF_LOG_ENTRY_EN
    #define CBUF_LOG_ENTRY_EN           0
#endif

/* --- Dependency Includes --- */
#if (CBUF_THREAD_SAFE_EN == 1) || (CBUF_USE_PSRAM_EN == 1) || (CBUF_LOG_EN == 1)
    #include "../../AppESPWrap/All.h"
#endif 

/* --- Logging Macros Wrapper --- */
#if (CBUF_LOG_EN == 1)
    #define CBuffLog(fmt, ...)          SysTagLog("CBuff", fmt, ##__VA_ARGS__)
#else
    #define CBuffLog(fmt, ...)
#endif

#if (CBUF_ERR_EN == 1)
    #define CBuffErr(fmt, ...)          SysTagLog("CBuff", "[ERR] " fmt, ##__VA_ARGS__)
#else
    #define CBuffErr(fmt, ...)
#endif

#if (CBUF_LOG_ENTRY_EN == 1)
    #define CBuffEntry()                SysEntryVer("CBuff")
    #define CBuffExit()                 SysExitVer("CBuff")
#else
    #define CBuffEntry()
    #define CBuffExit()
#endif

/* --- Type Definitions --- */

typedef int32_t     CBuffSize_t; 
typedef uint32_t    CBuffStat_t; 
typedef uint8_t     CBuffByte_t; 

/// @brief Status flags for the circular buffer state
enum CBuffStatusPositiveFlag_e {
    CBuffHasInit    = 0x00000001,   ///< Buffer initialized and ready
    CBuffEmptyMask  = 0x00000002,   ///< Buffer is empty
    CBuffFullMask   = 0x00000004,   ///< Buffer is full
    CBuffIsExtBuf   = 0x00000008,   ///< Buffer memory is external (user provided), do not free automatically
};

/// @brief Circular buffer handle structure.
typedef struct {
    CBuffByte_t * buffer;       ///< Pointer to the data buffer.
    CBuffSize_t   head;         ///< Index of the next write position.
    CBuffSize_t   tail;         ///< Index of the next read position.
    CBuffSize_t   size;         ///< Total size of the buffer in bytes.
    CBuffStat_t   stat;         ///< Status flags (Full/Empty/Init).
    #if (CBUF_THREAD_SAFE_EN == 1)
    void * mutex;               ///< Mutex handle for thread safety
    #endif 
} CBuff_t;

/* --- Function Prototypes --- */

/// @brief Creates and allocates a new circular buffer (Heap/PSRAM managed).
/// @param size The total size of the buffer in bytes.
/// @return A pointer to the created circular buffer handle, or NULL on failure.
CBuff_t* CBuffCreate(CBuffSize_t size);

/// @brief Initialize a CBuff handle with an EXTERNAL buffer (Static array or User Pointer).
/// @note  This allows using a static array or pre-allocated memory.
/// @param cb Pointer to the CBuff handle structure.
/// @param buffer Pointer to the external memory array.
/// @param size Size of the external memory.
/// @return STAT_OKE or Error Code.
DefaultRet_t CBuffInit(CBuff_t *cb, void *buffer, CBuffSize_t size);

/// @brief Destroys and frees a circular buffer.
/// @note  If created via CBuffInit (External Buffer), this will NOT free the buffer memory, only the mutex/handle.
/// @param cb Pointer to the circular buffer handle.
void CBuffDestroy(CBuff_t *cb);

/// @brief Writes an array of data to the circular buffer.
/// @param cb Pointer to the circular buffer handle.
/// @param data Pointer to the data source.
/// @param bytes The number of bytes to write.
/// @return The number of bytes actually written.
CBuffSize_t CBuffWrite(CBuff_t *cb, const void *data, CBuffSize_t bytes);

/// @brief Reads an array of data from the circular buffer.
/// @param cb Pointer to the circular buffer handle.
/// @param data Pointer to the buffer to store read data.
/// @param bytes The max number of bytes to read.
/// @return The number of bytes actually read.
CBuffSize_t CBuffRead(CBuff_t *cb, void *data, CBuffSize_t bytes);

/// @brief Pushes a single element (or block) to the buffer (Atomic/All-or-Nothing).
/// @param cb Pointer to the circular buffer handle.
/// @param data Pointer to the element.
/// @param size The size of the element in bytes.
/// @return STAT_OKE, STAT_ERR_INVALID_ARG, STAT_ERR_INVALID_STATE, or STAT_ERR_OVERFLOW.
DefaultRet_t CBuffPush(CBuff_t *cb, const void *data, CBuffSize_t size);

/// @brief Pops a single element (or block) from the buffer (Atomic/All-or-Nothing).
/// @param cb Pointer to the circular buffer handle.
/// @param data Pointer to the buffer to store the element.
/// @param size The size of the element to pop in bytes.
/// @return STAT_OKE, STAT_ERR_INVALID_ARG, STAT_ERR_INVALID_STATE, or STAT_ERR_UNDERFLOW.
DefaultRet_t CBuffPop(CBuff_t *cb, void *data, CBuffSize_t size);

/// @brief Gets the number of bytes currently stored in the buffer.
/// @param cb Pointer to the circular buffer handle.
/// @return The number of bytes of data available to be read.
CBuffSize_t CBuffGetDataCount(const CBuff_t *cb);

/// @brief Gets the available free space in the buffer.
/// @param cb Pointer to the circular buffer handle.
/// @return The number of bytes available for writing.
CBuffSize_t CBuffGetFreeCount(const CBuff_t *cb);

/// @brief Resets the circular buffer to an empty state.
/// @param cb Pointer to the circular buffer handle.
void CBuffReset(CBuff_t *cb);

#ifdef __cplusplus
}
#endif

#endif // __CIRCULAR_BUFFER_H__