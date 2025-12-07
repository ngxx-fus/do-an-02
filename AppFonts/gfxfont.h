#ifndef _GFFFONT_H_
#define _GFFFONT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppFonts/gfxfont.h")
#endif

/// @brief Macro to support PROGMEM on specific platforms (e.g., AVR), empty for ESP32/STM32.
#ifndef PROGMEM
#define PROGMEM
#endif

/// @brief Data stored PER GLYPH (Character).
typedef struct GFXglyph {
    uint16_t bitmapOffset;      ///< Pointer into GFXfont->bitmap
    uint8_t  width;             ///< Bitmap width in pixels
    uint8_t  height;            ///< Bitmap height in pixels
    uint8_t  xAdvance;          ///< Distance to advance cursor (x axis)
    int8_t   xOffset;           ///< Dist from cursor pos to UL (upper-left) corner X
    int8_t   yOffset;           ///< Dist from cursor pos to UL (upper-left) corner Y
} GFXglyph;

/// @brief Data stored for FONT AS A WHOLE.
typedef struct GFXfont {
    uint8_t  *bitmap;           ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;            ///< Glyph array
    uint8_t   first;            ///< ASCII extents (First character)
    uint8_t   last;             ///< ASCII extents (Last character)
    uint8_t   yAdvance;         ///< Newline distance (y axis)
} GFXfont;

#ifdef __cplusplus
}
#endif

#endif /// _GFFFONT_H_