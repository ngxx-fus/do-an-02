#ifndef __OLED128X64_H__
#define __OLED128X64_H__

#include "../i2c/i2c.h"

#include "oled128x64Commands.h"
#include "oled128x64Log.h"
#include "oled128x64Helpers.h"
#include "oled128x64Definitions.h"

/*
      0 1 2  y    127
    0 ###############
    1 ###############
    x #######p#######
    7 ###############

    For 3D buffer:
        p(x, y) <-> frame[pgID][clID][rowID]
    For 1D buffer:
        + Byte access:      __byte_at(x, y): (interget(x/8)*127 + y)
        + Read operation:   __read_at(x, y):  (__byte_at(x, y) & (1<<(x%8)))
        + Write 1-bit operation: __write_1_at(x, y):  (__byte_at(x, y) |= (1<<(x%8)))
        + Write 0-bit operation: __write_0_at(x, y):  (__byte_at(x, y) &= (~(1<<(x%8))))
        + Write op: __write_at(x, y, bit): ((bit)?__write_1_at(x, y):__write_0_at(x, y))

*/

def createOLEDDevice(oled128x64Dev_t ** oled);
def configOLEDDevice(oled128x64Dev_t * oled, pin_t scl, pin_t sda);
def startupOLEDDevice(oled128x64Dev_t * oled);

#endif