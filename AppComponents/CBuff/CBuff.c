/**
 * @file CBuff.c
 * @brief Implementation of the circular buffer.
 * @author Gemini Code Assist
 */

#include "CBuff.h"
#include "../../AppESPWrap/All.h"
#include <string.h>

/// @brief Resets the circular buffer to an empty state.
static void CBuffReset(CBuff_t *cb) {
    if (cb) {
        cb->head = 0;
        cb->tail = 0;
        cb->is_full = false;
    }
}

/// @brief Gets the amount of free space in the buffer.
static size_t CBuffGetFreeSpace(const CBuff_t *cb) {
    if (!cb) return 0;
    if (cb->is_full) return 0;
    if (cb->head >= cb->tail) {
        return cb->size - (cb->head - cb->tail);
    }
    return cb->tail - cb->head;
}

CBuff_t* CBuffCreate(size_t size, bool use_psram) {
    if (size == 0) return NULL;

    CBuff_t *cb = (CBuff_t *)malloc(sizeof(CBuff_t));
    if (cb == NULL) return NULL;

    if (use_psram) {
        #if (LCD32_CANVAS_IN_PSRAM_EN == 1) // Reuse existing PSRAM enable flag
        cb->buffer = (uint8_t *)heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        #else
        // Fallback to internal RAM if PSRAM is not enabled in config
        cb->buffer = (uint8_t *)malloc(size);
        #endif
    } else {
        cb->buffer = (uint8_t *)malloc(size);
    }

    if (cb->buffer == NULL) {
        free(cb);
        return NULL;
    }

    cb->size = size;
    CBuffReset(cb);

    return cb;
}

void CBuffDestroy(CBuff_t *cb) {
    if (cb == NULL) return;
    if (cb->buffer != NULL) {
        // heap_caps_free works for both internal and PSRAM allocated memory
        heap_caps_free(cb->buffer);
    }
    free(cb);
}

size_t CBuffGetDataCount(const CBuff_t *cb) {
    if (!cb) return 0;
    return cb->size - CBuffGetFreeSpace(cb);
}

size_t CBuffWrite(CBuff_t *cb, const void *data, size_t bytes) {
    if (!cb || !data || bytes == 0) return 0;

    const uint8_t *data_ptr = (const uint8_t *)data;
    size_t free_space = CBuffGetFreeSpace(cb);
    size_t bytes_to_write = (bytes > free_space) ? free_space : bytes;
    if (bytes_to_write == 0) return 0;

    if (cb->head + bytes_to_write > cb->size) {
        // Data wraps around the end of the buffer
        size_t first_part = cb->size - cb->head;
        memcpy(cb->buffer + cb->head, data_ptr, first_part);
        size_t second_part = bytes_to_write - first_part;
        memcpy(cb->buffer, data_ptr + first_part, second_part);
        cb->head = second_part;
    } else {
        // Data fits in a contiguous block
        memcpy(cb->buffer + cb->head, data_ptr, bytes_to_write);
        cb->head += bytes_to_write;
    }

    if (cb->head == cb->size) {
        cb->head = 0;
    }

    if (cb->head == cb->tail) {
        cb->is_full = true;
    }

    return bytes_to_write;
}