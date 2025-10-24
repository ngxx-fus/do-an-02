#ifndef __LCD32_H__
#define __LCD32_H__

#include "lcd32Cmd.h"
#include "lcd32Def.h"
#include "lcd32Helper.h"

def __lcd32CreateCanvas(lcd32Canvas_t *canvas, dim_t maxRow, dim_t maxCol);
def __lcd32SetupPin(lcd32Dev_t * dev);

def lcd32CreateDevice(lcd32Dev_t ** devPtr);
def lcd32ConfigDevice(lcd32Dev_t *dev, lcd32DataPin_t *dataPin, lcd32ControlPin_t *controlPin, dim_t maxRow, dim_t maxCol);
def lcd32StartUpDevice(lcd32Dev_t *dev);

def lcd32FillCanvas(lcd32Dev_t *dev, color_t color);
def lcd32FlushCanvas(lcd32Dev_t *dev);
void lcd32DirectWritePixel(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color);


#endif