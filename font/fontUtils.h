#ifndef __FONT_UTILS_H__
#define __FONT_UTILS_H__

#include "gfxfont.h"
#include "localFonts/FreeSerifBoldItalic24pt7b.h"
#include "localFonts/FreeSerifBoldItalic18pt7b.h"
#include "localFonts/FreeSerifBoldItalic12pt7b.h"
#include "localFonts/FreeSerifBoldItalic9pt7b.h"
#include "localFonts/FreeSerif9pt7b.h"

#ifndef defaultTextH
    #define defaultTextH(__GFXfont) (__GFXfont.yAdvance)
#endif

extern GFXfont    mainFont;
extern GFXfont    fontTitle;
extern GFXfont    fontBody;
extern GFXfont    fontHeading01;
extern GFXfont    fontHeading02;
extern GFXfont    fontHeading03;

#endif 
