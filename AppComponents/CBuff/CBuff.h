/**
 * @file CBuff.h
 * @brief A simple circular buffer implementation.
 * @author Gemini Code Assist
 */

#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/// @brief Circular buffer handle structure.
typedef struct {
    uint8_t *buffer;    ///< Pointer to the data buffer.
    size_t head;        ///< Index of the next write position.
    size_t tail;        ///< Index of the next read position.
    size_t size;        ///< Total size of the buffer in bytes.
    bool is_full;       ///< Flag to indicate if the buffer is full.
} CBuff_t;

/// @brief Creates and allocates a new circular buffer.
/// @param size The total size of the buffer in bytes.
/// @param use_psram If true, attempts to allocate the buffer in PSRAM.
/// @return A pointer to the created circular buffer handle, or NULL on failure.
CBuff_t* CBuffCreate(size_t size, bool use_psram);

/// @brief Destroys and frees a circular buffer.
/// @param cb Pointer to the circular buffer handle.
void CBuffDestroy(CBuff_t *cb);

/// @brief Writes data to the circular buffer.
/// @param cb Pointer to the circular buffer handle.
/// @param data Pointer to the data to be written.
/// @param bytes The number of bytes to write.
/// @return The number of bytes actually written. May be less than requested if the buffer is full.
size_t CBuffWrite(CBuff_t *cb, const void *data, size_t bytes);

/// @brief Gets the number of bytes currently stored in the buffer.
/// @param cb Pointer to the circular buffer handle.
/// @return The number of bytes of data available to be read.
size_t CBuffGetDataCount(const CBuff_t *cb);

#ifdef __cplusplus
}
#endif

#endif // __CIRCULAR_BUFFER_H__