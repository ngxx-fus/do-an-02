#ifndef __LCD32_H__
#define __LCD32_H__

#include "lcd32Cmd.h"
#include "lcd32Def.h"
#include "lcd32Helper.h"

/// @brief [Internal function] Delete existed canvas, free buffer, set pointer to NULL
def __lcd32DeleteCanvasBuffer(lcd32Canvas_t *canvas);

/// @brief [Internal function] Create new canvas, allocate buffer
def __lcd32ConfigCanvas(lcd32Canvas_t *canvas, dim_t maxRow, dim_t maxCol);

/// @brief [Internal function] Set up all pin in lcd32's dev
def __lcd32SetupPin(lcd32Dev_t * dev);

/// @brief [Public function] Create new object without initialize
def lcd32CreateDevice(lcd32Dev_t ** devPtr);

/// @brief [Public function] Config new object
def lcd32ConfigDevice(lcd32Dev_t *dev, lcd32DataPin_t *dataPin, lcd32ControlPin_t *controlPin, dim_t maxRow, dim_t maxCol);

/// @brief [Public function] Start-up (set-up pin, set reset-state)
def lcd32StartUpDevice(lcd32Dev_t *dev);

/// @brief [Public function] Fill cached canvas with specified color
def lcd32FillCanvas(lcd32Dev_t *dev, color_t color);

/// @brief [Public function] Flush cached canvas to real screen
def lcd32FlushCanvas(lcd32Dev_t *dev);

/// @brief [Public function] Directly draw a pixel to real screen
def lcd32DirectlyWritePixel(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color);


#endif