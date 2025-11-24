#ifndef _GFFFONT_H_
#define _GFFFONT_H_
#include <stdint.h>

#define PROGMEM

<<<<<<< HEAD
typedef struct GFXglyph{ // Data stored PER GLYPH
=======
typedef struct { // Data stored PER GLYPH
>>>>>>> master
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL (upper-left) corner 
} GFXglyph;

<<<<<<< HEAD
typedef struct GFXfont{ // Data stored for FONT AS A WHOLE:
=======
typedef struct { // Data stored for FONT AS A WHOLE:
>>>>>>> master
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;

#endif
