#ifndef __FONT_UTILS_H__
#define __FONT_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppFonts/AppFont.h")
#endif

#include "gfxfont.h"
#include "localFonts/FreeSerifBoldItalic24pt7b.h"
#include "localFonts/FreeSerifBoldItalic18pt7b.h"
#include "localFonts/FreeSerifBoldItalic12pt7b.h"
#include "localFonts/FreeSerifBoldItalic9pt7b.h"
#include "localFonts/FreeSerif9pt7b.h"
#include "localFonts/Picopixel.h"

/// @brief Helper macro to get the height of a font instance.
#ifndef DefaultTextH
    #define DefaultTextH(__GFXfont) (__GFXfont.yAdvance)
#endif

/// @brief Union containing the entire system font configuration.
/// @note  Allows access via named members (e.g., .Title) or array index (e.g., .arr[0]).
typedef union {
    struct {
        const GFXfont *Title;       ///< Font for large titles.
        const GFXfont *Body;        ///< Font for body text.
        const GFXfont *Heading01;   ///< Font for Heading Level 1.
        const GFXfont *Heading02;   ///< Font for Heading Level 2.
        const GFXfont *Heading03;   ///< Font for Heading Level 3.
        const GFXfont *Note;        ///< Font for small notes/annotations.
    };
    const GFXfont *arr[6];          ///< Array access to the fonts above.
} SystemFont_t;

/// @brief Global instance to access system fonts.
extern SystemFont_t SystemFont;

#ifdef __cplusplus
}
#endif

#endif /// __FONT_UTILS_H__