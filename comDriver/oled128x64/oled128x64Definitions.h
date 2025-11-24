<<<<<<< HEAD
=======
/// @file   oled128x64Definitions.h
/// @brief  OLED 128x64 display definitions; Provides constants, types, and device structure for I2C communication and framebuffer handling.

>>>>>>> master
#ifndef __OLED128x64_DEFINITIONS_H__
#define __OLED128x64_DEFINITIONS_H__

#include "../i2c/i2c.h"

/// @brief Maximum X coordinate (0–63) for OLED 128x64 display
#define __max_x                 63

/// @brief Maximum Y coordinate (0–127) for OLED 128x64 display
#define __max_y                 127

/// @brief Minimum X coordinate (0)
#define __min_x                 0

/// @brief Minimum Y coordinate (0)
#define __min_y                 0

/// @brief OLED display height in pixels
#define __oled_h                64

/// @brief OLED display width in pixels
#define __oled_w                128

#ifndef OLED_ADDR
    /// @brief I2C slave address of the OLED display (default 0x3C)
    #define OLED_ADDR 0x3C
#endif 

/// @typedef byte_t
/// @brief Alias for 8-bit unsigned integer (used for generic byte data)
typedef uint8_t byte_t;

/// @typedef cell_t
/// @brief Alias for a single display cell unit (1 byte)
typedef uint8_t cell_t;

/// @typedef color_t
/// @brief Alias for pixel color type (e.g., 0 = black, 1 = white)
typedef uint8_t color_t;

/// @typedef xy_t
/// @brief Alias for signed 16-bit coordinate type
typedef int16_t xy_t;

/// @struct oled128x64Dev_t
/// @brief Structure representing an OLED 128x64 display device
typedef struct oled128x64Dev_t {
    
    /// @brief Pointer to associated I2C device
    i2cDev_t *  i2cDev;

    /// @brief Pointer to framebuffer or canvas buffer (array of display cells)
    cell_t *    canvas;

    /// @brief Display height (usually 64 pixels)
    xy_t        height;

    /// @brief Display width (usually 128 pixels)
    xy_t        width;
} oled128x64Dev_t;






#endif