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

#ifndef DefaultTextH
    #define DefaultTextH(__GFXfont) (__GFXfont.yAdvance)
#endif

extern GFXfont    mainFont;
extern GFXfont    fontTitle;
extern GFXfont    fontBody;
extern GFXfont    fontHeading01;
extern GFXfont    fontHeading02;
extern GFXfont    fontHeading03;
extern GFXfont    fontNote;

#ifdef __cplusplus
}
#endif

#endif 
