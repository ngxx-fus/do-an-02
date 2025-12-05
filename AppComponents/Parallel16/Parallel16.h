#ifndef __PARALLEL16_H__
#define __PARALLEL16_H__

#include <string.h>

#include "../../AppConfig/All.h"
#include "../../AppUtils/All.h"

#include "Parallel16Def.h"
#include "Parallel16Utils.h"

/// @brief Create and initialize a new parallel16 device instance.
def P16CreateDevice(P16Dev_t ** dev);

/// @brief Configure data and control pins for the device.
def P16ConfigDevice(P16Dev_t * dev, P16DataPin_t* dataPin, P16ControlPin_t * controlPin);

/// @brief Initialize and start up the parallel16 device.
def P16StartUpDevice(P16Dev_t * dev, P16DataPin_t* dataPin, P16ControlPin_t * controlPin);

/// @brief Set device mode to READ.
def P16SetModeRead(P16Dev_t * dev);

/// @brief Set device mode to WRITE.
def P16SetModeWrite(P16Dev_t * dev);

/// @brief Write a 16-bit data value to the device.
def P16Write(P16Dev_t * dev, P16Data_t data16);

/// @brief Read a 16-bit data value from the device.
def P16Read(P16Dev_t * dev, P16Data_t * data16);

/// @brief Write multiple 16-bit values to the device.
def P16WriteArray(P16Dev_t * dev, const P16Data_t *data16, P16Size_t dataSize);

/// @brief Read multiple 16-bit values from the device.
def P16ReadArray(P16Dev_t * dev, P16Data_t * data16, P16Size_t dataSize);

#endif