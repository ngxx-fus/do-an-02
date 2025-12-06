/**
 * @file LCD32Colors.h
 * @brief RGB565 Color Definitions based on ANSI Terminal Colors
 * @author Nguyen Thanh Phu
 */

#ifndef __LCD32_COLORS_H__
#define __LCD32_COLORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Convert standard RGB (8-bit each) to RGB565 (16-bit)
/// @param r Red (0-255)
/// @param g Green (0-255)
/// @param b Blue (0-255)
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

#define COLOR_BLACK         0x0000      ///< Black (0, 0, 0)
#define COLOR_RED           0xA800      ///< Red (170, 0, 0)
#define COLOR_GREEN         0x0540      ///< Green (0, 170, 0)
#define COLOR_YELLOW        0xAD40      ///< Yellow (170, 170, 0) - Brownish
#define COLOR_BLUE          0x0015      ///< Blue (0, 0, 170)
#define COLOR_MAGENTA       0xA815      ///< Magenta (170, 0, 170)
#define COLOR_CYAN          0x0555      ///< Cyan (0, 170, 170)
#define COLOR_LGRAY         0xAD55      ///< Light Gray (170, 170, 170) - ANSI "White"

#define COLOR_GRAY          0x52AA      ///< Dark Gray (85, 85, 85) - ANSI "Bright Black"
#define COLOR_LRED          0xF800      ///< Light Red (255, 0, 0)
#define COLOR_LGREEN        0x07E0      ///< Light Green (0, 255, 0)
#define COLOR_LYELLOW       0xFFE0      ///< Light Yellow (255, 255, 0)
#define COLOR_LBLUE         0x001F      ///< Light Blue (0, 0, 255)
#define COLOR_LMAGENTA      0xF81F      ///< Light Magenta (255, 0, 255)
#define COLOR_LCYAN         0x07FF      ///< Light Cyan (0, 255, 255)
#define COLOR_WHITE         0xFFFF      ///< Pure White (255, 255, 255)

#define COLOR_ERROR         COLOR_LRED  ///< Color for error messages (Mapped to Light Red)
#define COLOR_GRID          0xEF9D      ///< Color for grid lines (Light Greyish)
#define COLOR_AXIS          0x2104      ///< Color for axis lines (Dark Greyish)
#define COLOR_AXIS_TEXT     COLOR_WHITE ///< Color for axis text labels.
#define COLOR_LEGEND_BG     0xEF7D      ///< Background color for legends.

#define COLOR_CHART_01      0xD6A0      ///< Pastel Red/Orange
#define COLOR_CHART_02      0xF96E      ///< Pastel Orange
#define COLOR_CHART_03      0x356C      ///< Dark Blue/Grey
#define COLOR_CHART_04      0x449E      ///< Slate Blue

#ifdef __cplusplus
}
#endif

#endif /// __LCD32_COLORS_H__