/**
 * @file AppFont.c
 * @brief Implementation of system font definitions.
 */

#include "All.h"

/// @brief Global initialization of system fonts.
SystemFont_t SystemFont = {
    .Title      = &FreeSerifBoldItalic24pt7b,   ///< Big Title
    .Body       = &FreeSerif9pt7b,              ///< Standard Body
    .Heading01  = &FreeSerifBoldItalic18pt7b,   ///< H1
    .Heading02  = &FreeSerifBoldItalic12pt7b,   ///< H2
    .Heading03  = &FreeSerifBoldItalic9pt7b,    ///< H3
    .Note       = &Picopixel                    ///< Small Note
};