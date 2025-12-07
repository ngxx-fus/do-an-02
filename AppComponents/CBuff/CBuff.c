/**
 * @file CBuff.c
 * @brief Implementation of the circular buffer with Thread-Safe, PSRAM support and Robust Checks.
 * @author Gemini Code Assist
 */

#include "CBuff.h"
#include <string.h>
#include <stdlib.h>

/* --- Internal Helpers --- */

#if (CBUF_THREAD_SAFE_EN == 1)
    #define CBUF_LOCK(cb)       if((cb)->mutex) xSemaphoreTake((SemaphoreHandle_t)(cb)->mutex, portMAX_DELAY)
    #define CBUF_UNLOCK(cb)     if((cb)->mutex) xSemaphoreGive((SemaphoreHandle_t)(cb)->mutex)
#else
    #define CBUF_LOCK(cb)
    #define CBUF_UNLOCK(cb)
#endif

static CBuffSize_t _CBuffGetFreeSpace(const CBuff_t *cb) {
    if (!cb) return 0;
    if (cb->stat & CBuffFullMask) return 0;
    if (cb->head >= cb->tail) return cb->size - (cb->head - cb->tail);
    return cb->tail - cb->head;
}

static CBuffSize_t _CBuffGetDataCount(const CBuff_t *cb) {
    if (!cb) return 0;
    if (cb->stat & CBuffFullMask) return cb->size;
    if (cb->head >= cb->tail) return cb->head - cb->tail;
    return cb->size - (cb->tail - cb->head);
}

/* --- Validation Macros --- */

#if (CBUF_ARGS_CHECK_EN == 1)
    #define VALIDATE_ARGS(cb, ptr, sz) do { \
        if (!(cb) || !(ptr) || (sz) == 0) { \
            CBuffErr("Invalid Args: cb=%p, ptr=%p, sz=%d", cb, ptr, (int)sz); \
            return STAT_ERR_INVALID_ARG; \
        } \
    } while(0)
    
    // Version for functions returning Size (0 on error)
    #define VALIDATE_ARGS_RET_ZERO(cb, ptr, sz) do { \
        if (!(cb) || !(ptr) || (sz) == 0) { \
            CBuffErr("Invalid Args: cb=%p, ptr=%p, sz=%d", cb, ptr, (int)sz); \
            return 0; \
        } \
    } while(0)
#else 
    #define VALIDATE_ARGS(cb, ptr, sz)
    #define VALIDATE_ARGS_RET_ZERO(cb, ptr, sz)
#endif 

#if (CBUF_INITIALIZED_CHECK_EN == 1)
    #define VALIDATE_INIT(cb) do { \
        if (((cb)->buffer == NULL) || !((cb)->stat & CBuffHasInit)) { \
            CBuffErr("Not Initialized: cb=%p, buf=%p, stat=0x%X", cb, (cb)->buffer, (cb)->stat); \
            return STAT_ERR_INVALID_STATE; \
        } \
    } while(0)

    #define VALIDATE_INIT_RET_ZERO(cb) do { \
        if (((cb)->buffer == NULL) || !((cb)->stat & CBuffHasInit)) { \
            CBuffErr("Not Initialized: cb=%p, buf=%p, stat=0x%X", cb, (cb)->buffer, (cb)->stat); \
            return 0; \
        } \
    } while(0)

    #define VALIDATE_INIT_RET_VOID(cb) do { \
        if (((cb)->buffer == NULL) || !((cb)->stat & CBuffHasInit)) { \
            CBuffErr("Not Initialized: cb=%p, buf=%p, stat=0x%X", cb, (cb)->buffer, (cb)->stat); \
            return; \
        } \
    } while(0)
#else 
    #define VALIDATE_INIT(cb)
    #define VALIDATE_INIT_RET_ZERO(cb)
    #define VALIDATE_INIT_RET_VOID(cb)
#endif

/* --- API Implementations --- */

DefaultRet_t CBuffInit(CBuff_t *cb, void *buffer, CBuffSize_t size) {
    if (!cb || !buffer || size <= 0) return STAT_ERR_INVALID_ARG;

    CBuffEntry();
    
    // Clear structure first
    memset(cb, 0, sizeof(CBuff_t));
    
    cb->buffer = (CBuffByte_t *)buffer;
    cb->size = size;
    cb->head = 0;
    cb->tail = 0;
    
    // Init Mutex
    #if (CBUF_THREAD_SAFE_EN == 1)
    cb->mutex = xSemaphoreCreateMutex();
    if (cb->mutex == NULL) {
        CBuffErr("Mutex Init Failed");
        return STAT_ERR_INIT_FAILED;
    }
    #endif

    // Mark as Initialized, Empty and External Buffer
    cb->stat = CBuffHasInit | CBuffEmptyMask | CBuffIsExtBuf;

    CBuffLog("Init External Buffer: %p, Size: %d", buffer, size);
    return STAT_OKE;
}

CBuff_t* CBuffCreate(CBuffSize_t size) {
    if (size <= 0) return NULL;
    CBuffEntry();

    CBuff_t *cb = (CBuff_t *)malloc(sizeof(CBuff_t));
    if (!cb) {
        CBuffErr("Malloc Struct Failed");
        return NULL;
    }

    void * bufMem = NULL;
    // 1. Try Allocate Buffer (Priority: PSRAM -> iRAM)
    #if (CBUF_USE_PSRAM_EN == 1)
    bufMem = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    #endif

    if (!bufMem) {
        bufMem = malloc(size);
    }

    if (!bufMem) {
        CBuffErr("Malloc Buffer Failed");
        free(cb);
        return NULL;
    }

    // Reuse CBuffInit logic
    if (CBuffInit(cb, bufMem, size) != STAT_OKE) {
        if (bufMem) heap_caps_free(bufMem);
        free(cb);
        return NULL;
    }

    // Clear External Buffer flag because we allocated it internally
    cb->stat &= ~CBuffIsExtBuf;

    CBuffLog("Created CBuff: %p, Size: %d", cb, size);
    return cb;
}

void CBuffDestroy(CBuff_t *cb) {
    if (cb == NULL) return;
    CBuffEntry();

    #if (CBUF_THREAD_SAFE_EN == 1)
    if (cb->mutex) {
        vSemaphoreDelete((SemaphoreHandle_t)cb->mutex);
        cb->mutex = NULL;
    }
    #endif

    // Only free buffer if it was allocated by CBuffCreate (Not External)
    if (cb->buffer != NULL && !(cb->stat & CBuffIsExtBuf)) {
        heap_caps_free(cb->buffer); 
        cb->buffer = NULL;
    }
    
    cb->stat = 0; // Clear all flags
    free(cb);
    CBuffExit();
}

void CBuffReset(CBuff_t *cb) {
    if (cb) {
        VALIDATE_INIT_RET_VOID(cb); // Ensure it's valid before lock
        CBUF_LOCK(cb);
        cb->head = 0;
        cb->tail = 0;
        // Keep Init and ExtBuf flags, reset Empty/Full
        CBuffStat_t persistentFlags = cb->stat & (CBuffHasInit | CBuffIsExtBuf);
        cb->stat = persistentFlags | CBuffEmptyMask;
        CBUF_UNLOCK(cb);
    }
}

CBuffSize_t CBuffWrite(CBuff_t *cb, const void *data, CBuffSize_t bytes) {
    VALIDATE_ARGS_RET_ZERO(cb, data, bytes);
    VALIDATE_INIT_RET_ZERO(cb);

    CBUF_LOCK(cb);

    const CBuffByte_t *data_ptr = (const CBuffByte_t *)data;
    CBuffSize_t free_space = _CBuffGetFreeSpace(cb);
    CBuffSize_t bytes_to_write = (bytes > free_space) ? free_space : bytes;

    if (bytes_to_write > 0) {
        cb->stat &= ~CBuffEmptyMask; 

        if (cb->head + bytes_to_write > cb->size) {
            CBuffSize_t first_part = cb->size - cb->head;
            memcpy(cb->buffer + cb->head, data_ptr, first_part);
            memcpy(cb->buffer, data_ptr + first_part, bytes_to_write - first_part);
            cb->head = bytes_to_write - first_part;
        } else {
            memcpy(cb->buffer + cb->head, data_ptr, bytes_to_write);
            cb->head += bytes_to_write;
        }

        if (cb->head == cb->size) cb->head = 0;
        if (cb->head == cb->tail) cb->stat |= CBuffFullMask;
    }

    CBUF_UNLOCK(cb);
    return bytes_to_write;
}

CBuffSize_t CBuffRead(CBuff_t *cb, void *data, CBuffSize_t bytes) {
    VALIDATE_ARGS_RET_ZERO(cb, data, bytes);
    VALIDATE_INIT_RET_ZERO(cb);

    CBUF_LOCK(cb);

    CBuffSize_t data_count = _CBuffGetDataCount(cb);
    CBuffSize_t bytes_to_read = (bytes > data_count) ? data_count : bytes;
    uint8_t *out_ptr = (uint8_t *)data;

    if (bytes_to_read > 0) {
        cb->stat &= ~CBuffFullMask;

        if (cb->tail + bytes_to_read > cb->size) {
            CBuffSize_t first_part = cb->size - cb->tail;
            memcpy(out_ptr, cb->buffer + cb->tail, first_part);
            memcpy(out_ptr + first_part, cb->buffer, bytes_to_read - first_part);
            cb->tail = bytes_to_read - first_part;
        } else {
            memcpy(out_ptr, cb->buffer + cb->tail, bytes_to_read);
            cb->tail += bytes_to_read;
        }

        if (cb->tail == cb->size) cb->tail = 0;
        if (cb->tail == cb->head) cb->stat |= CBuffEmptyMask;
    }

    CBUF_UNLOCK(cb);
    return bytes_to_read;
}

DefaultRet_t CBuffPush(CBuff_t *cb, const void *data, CBuffSize_t size) {
    VALIDATE_ARGS(cb, data, size);
    VALIDATE_INIT(cb);

    CBUF_LOCK(cb);
    
    if (_CBuffGetFreeSpace(cb) < size) {
        CBUF_UNLOCK(cb);
        // CBuffErr("Push Overflow: Req %d, Free %d", size, _CBuffGetFreeSpace(cb)); // Optional Log
        return STAT_ERR_OVERFLOW; 
    }
    
    cb->stat &= ~CBuffEmptyMask;

    const CBuffByte_t *data_ptr = (const CBuffByte_t *)data;
    if (cb->head + size > cb->size) {
        CBuffSize_t first_part = cb->size - cb->head;
        memcpy(cb->buffer + cb->head, data_ptr, first_part);
        memcpy(cb->buffer, data_ptr + first_part, size - first_part);
        cb->head = size - first_part;
    } else {
        memcpy(cb->buffer + cb->head, data_ptr, size);
        cb->head += size;
    }
    
    if (cb->head == cb->size) cb->head = 0;
    if (cb->head == cb->tail) cb->stat |= CBuffFullMask;

    CBUF_UNLOCK(cb);
    return STAT_OKE;
}

DefaultRet_t CBuffPop(CBuff_t *cb, void *data, CBuffSize_t size) {
    VALIDATE_ARGS(cb, data, size);
    VALIDATE_INIT(cb);

    CBUF_LOCK(cb);

    if (_CBuffGetDataCount(cb) < size) {
        CBUF_UNLOCK(cb);
        return STAT_ERR_UNDERFLOW; 
    }

    cb->stat &= ~CBuffFullMask;

    uint8_t *out_ptr = (uint8_t *)data;
    if (cb->tail + size > cb->size) {
        CBuffSize_t first_part = cb->size - cb->tail;
        memcpy(out_ptr, cb->buffer + cb->tail, first_part);
        memcpy(out_ptr + first_part, cb->buffer, size - first_part);
        cb->tail = size - first_part;
    } else {
        memcpy(out_ptr, cb->buffer + cb->tail, size);
        cb->tail += size;
    }

    if (cb->tail == cb->size) cb->tail = 0;
    if (cb->tail == cb->head) cb->stat |= CBuffEmptyMask;

    CBUF_UNLOCK(cb);
    return STAT_OKE;
}

CBuffSize_t CBuffGetDataCount(const CBuff_t *cb) {
    if (!cb) return 0;
    // Note: No INIT Check strict here to allow checking 0 on invalid
    
    CBuff_t * non_const_cb = (CBuff_t *)cb;
    CBUF_LOCK(non_const_cb);
    CBuffSize_t count = _CBuffGetDataCount(cb);
    CBUF_UNLOCK(non_const_cb);
    return count;
}

CBuffSize_t CBuffGetFreeCount(const CBuff_t *cb) {
    if (!cb) return 0;
    
    CBuff_t * non_const_cb = (CBuff_t *)cb;
    CBUF_LOCK(non_const_cb);
    CBuffSize_t count = _CBuffGetFreeSpace(cb);
    CBUF_UNLOCK(non_const_cb);
    return count;
}