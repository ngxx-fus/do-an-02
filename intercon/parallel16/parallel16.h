#ifndef __PARALLEL16_H__
#define __PARALLEL16_H__

#include "../../helper/general.h"

#include "parallel16Def.h"
#include "prallel16Helper.h"

/// @brief Create and initialize a new parallel16 device instance.
def p16CreateDevice(parallel16Dev_t ** dev);

/// @brief Configure data and control pins for the device.
def p16ConfigDevice(parallel16Dev_t * dev, p16DataPin_t* dataPin, p16ControlPin_t * controlPin);

/// @brief Initialize and start up the parallel16 device.
def p16StartUpDevice(parallel16Dev_t * dev, p16DataPin_t* dataPin, p16ControlPin_t * controlPin);

/// @brief Set device mode to READ.
def p16SetModeRead(parallel16Dev_t * dev);

/// @brief Set device mode to WRITE.
def p16SetModeWrite(parallel16Dev_t * dev);

/// @brief Write a 16-bit data value to the device.
def p16Write(parallel16Dev_t * dev, p16Data_t data16);

/// @brief Read a 16-bit data value from the device.
def p16Read(parallel16Dev_t * dev, p16Data_t * data16);

/// @brief Write multiple 16-bit values to the device.
def p16WriteArray(parallel16Dev_t * dev, const p16Data_t *data16, p16Size_t dataSize);

/// @brief Read multiple 16-bit values from the device.
def p16ReadArray(parallel16Dev_t * dev, p16Data_t * data16, p16Size_t dataSize);

#endif
