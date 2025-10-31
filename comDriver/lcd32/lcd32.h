#ifndef __LCD32_H__
#define __LCD32_H__

#include "lcd32Cmd.h"
#include "lcd32Def.h"
#include "lcd32Helper.h"

/// @brief [Internal] Delete existed canvas, free buffer, set pointer to NULL
def __lcd32DeleteCanvasBuffer(lcd32Canvas_t *canvas);

/// @brief [Internal] Create new canvas, allocate buffer
def __lcd32ConfigCanvas(lcd32Canvas_t *canvas, dim_t maxRow, dim_t maxCol);

/// @brief [Internal] Set up all pin in lcd32's dev
def __lcd32SetupPin(lcd32Dev_t * dev);

/// @brief [Internal] Address window setting
def __lcd32SetAddressWindow(lcd32Dev_t *dev, dim_t row, dim_t col, dim_t height, dim_t width);

/// @brief [Public] Create new object without initialize
def lcd32CreateDevice(lcd32Dev_t ** devPtr);

/// @brief [Public] Config new object
def lcd32ConfigDevice(lcd32Dev_t *dev, lcd32DataPin_t *dataPin, lcd32ControlPin_t *controlPin, dim_t maxRow, dim_t maxCol);

/// @brief [Public] Start-up (set-up pin, set reset-state)
def lcd32StartUpDevice(lcd32Dev_t *dev);

/// @brief [Public] Put to sleep
def lcd32PutToSleep(lcd32Dev_t *dev);

/// @brief [Public] Wake from sleep
def lcd32WakeFromSleep(lcd32Dev_t *dev);

/// @brief [Public] Fill cached canvas with specified color
def lcd32FillCanvas(lcd32Dev_t *dev, color_t color);

/// @brief [Public] Flush cached canvas to real screen
def lcd32FlushCanvas(lcd32Dev_t *dev);

/// @brief [Public] Directly draw a pixel to real screen
def lcd32DirectlyWritePixel(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color);

/// @brief [Public] Draw a single character on LCD canvas
def lcd32DrawChar(lcd32Dev_t *dev, dim_t r, dim_t c, char ch, GFXfont *f, color_t color);

/// @brief [Public] Draw a string on LCD canvas
def lcd32DrawText(lcd32Dev_t *dev, dim_t r, dim_t c, const char *str, GFXfont *f, color_t color);

/// @brief [Public] Draws a 1px line using Bresenham's algorithm.
def lcd32DrawLine(lcd32Dev_t *dev, dim_t r0, dim_t c0, dim_t r1, dim_t c1, color_t color);

def lcd32DrawThickLine(lcd32Dev_t *dev, dim_t r0, dim_t c0, dim_t r1, dim_t c1, color_t color, dim_t thickness);

def lcd32DrawPolygon(lcd32Dev_t *dev, const lcdPoint_t *points, size_t n, color_t color);

def lcd32DrawLineChart(lcd32Dev_t *dev, const lcdPoint_t *points, size_t n, color_t color);

def lcd32DrawFilledPolygon(lcd32Dev_t *dev, const lcdPoint_t *points, size_t n, color_t color);

#endif