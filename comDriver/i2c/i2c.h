<<<<<<< HEAD
=======
/// @file   i2c.h
/// @brief  I2C driver interface; Provides functions for creating, configuring, starting, transmitting/receiving, and destroying I2C devices.

>>>>>>> master
#ifndef __I2C_H__
#define __I2C_H__

#include "../../helper/general.h"

<<<<<<< HEAD
/// LOG CONTROL ///////////////////////////////////////////////////////////////////////////////////

#include "i2cLog.h"

/// THE DEFINITIONS RELATED TO I2C ////////////////////////////////////////////////////////////////

#include "i2cDefinition.h"

/// HELPER/INLINE FUNC ////////////////////////////////////////////////////////////////////////////

#include "i2cHelper.h"

/// MAIN FUNC /////////////////////////////////////////////////////////////////////////////////////
=======
/// @cond
/// LOG CONTROL ///////////////////////////////////////////////////////////////////////////////////
/// @endcond

#include "i2cLog.h"

/// @cond
/// THE DEFINITIONS RELATED TO I2C ////////////////////////////////////////////////////////////////
/// @endcond

#include "i2cDefinition.h"

/// @cond
/// HELPER/INLINE FUNC ////////////////////////////////////////////////////////////////////////////
/// @endcond

#include "i2cHelper.h"

/// @cond
/// MAIN FUNC /////////////////////////////////////////////////////////////////////////////////////
/// @endcond
>>>>>>> master

/// @brief Handle SCL interrupt in slave mode
/// @param pv Pointer to i2cDev_t
static void IRAM_ATTR i2cHandleSCLIsr(void* pvI2CDev){
    __i2cEntry("i2cHandleSCLIsr(%p)", pvI2CDev);
    

    __i2cExit("i2cHandleSCLIsr()");
}

/// @brief Handle SDA interrupt in slave mode
/// @param pv Pointer to i2cDev_t
static void IRAM_ATTR i2cHandleSDAIsr(void* pvI2CDev){
    __i2cEntry("i2cHandleSDAIsr(%p)", pvI2CDev);


    __i2cExit("i2cHandleSDAIsr()");
}

/// @brief Create a new I2C device instance
/// @param pDev Pointer to pointer of i2cDev_t (will store created device)
/// @return Default return status
def createI2CDevice(i2cDev_t ** pDev);

/// @brief Configure I2C device pins and options
/// @param dev Pointer to i2cDev_t
/// @param addr Address of this device (for slave mode)
/// @param scl Pin number for SCL
/// @param sda Pin number for SDA
/// @param config Configuration flags. See I2C_CONFIG_ORDER enum.
/// @return Default return status
def configI2CDevice(i2cDev_t * dev, uint8_t addr, pin_t scl, pin_t sda, flag_t config);

/// @brief Startup the I2C device (initialize GPIO and configs)
/// @param dev Pointer to i2cDev_t
/// @return Default return status
def startupI2CDevice(i2cDev_t * dev);

/// @brief Set transmit buffer for I2C
/// @param dev Pointer to i2cDev_t
/// @param tBuff Pointer to transmit buffer
/// @param size Size of transmit buffer (in bytes)
/// @return Default return status
def i2cSetTransmitBuffer(i2cDev_t * dev, void * tBuff, size_t size);

/// @brief Set receive buffer for I2C
/// @param dev Pointer to i2cDev_t
/// @param rBuff Pointer to receive buffer
/// @param size Size of transmit buffer (in bytes)
/// @return Default return status
def i2cSetReceiveBuffer(i2cDev_t * dev, void * rBuff, size_t size);

/// @brief Send a specified byte 
/// @param dev Pointer to i2cDev_t
/// @param addr7 7 bits slave address 
/// @param rw Read/Write bit (I2C_WRITE_MODE, I2C_READ_MODE)
/// @param byte Specified byte to be sent
/// @param extConf Extent config, check I2C_EXTENT_CONFIG_BIT_ORDER enum for more details.
/// @return Default return status
def i2cSendByte(i2cDev_t * dev, uint8_t addr7, uint8_t rw, uint8_t byte, flag_t extConf);

/// @brief Send a specified double byte 
/// @param dev Pointer to i2cDev_t
/// @param addr7 7 bits slave address 
/// @param rw Read/Write bit (I2C_WRITE_MODE, I2C_READ_MODE)
/// @param byteHigh Specified byte-High to be sent
/// @param byteLow Specified byte-Low to be sent
/// @param extConf Extent config, check I2C_EXTENT_CONFIG_BIT_ORDER enum for more details.
/// @return Default return status
def i2cSendDoubleByte(i2cDev_t * dev, uint8_t addr7, uint8_t rw, uint8_t byteHigh, uint8_t byteLow, flag_t extConf);

/// @brief Send all bytes in the transmit buffer
/// @param dev Pointer to i2cDev_t
/// @param addr7 7 bits slave address 
/// @param rw Read/Write bit (I2C_WRITE_MODE, I2C_READ_MODE)
/// @param extConf Extent config, check I2C_EXTENT_CONFIG_BIT_ORDER enum for more details.
/// @return Default return status
def i2cSendBuffer(i2cDev_t * dev, uint8_t addr7, uint8_t rw, flag_t extConf);

/// @brief Receive all bytes in the transmit buffer
/// @param dev Pointer to i2cDev_t
/// @param addr7 7 bits slave address 
/// @param rw Read/Write bit (I2C_WRITE_MODE, I2C_READ_MODE)
/// @param extConf Extent config, check I2C_EXTENT_CONFIG_BIT_ORDER enum for more details.
/// @return Default return status
def i2cReceiveBuffer(i2cDev_t * dev, uint8_t addr7, uint8_t rw, flag_t extConf);

/// @brief Deinitialize and free an I2C device.
/// @param dev Pointer to the I2C device structure.
/// @return OKE on success, ERR_NULL if dev is NULL.
def destroyI2CDevice(i2cDev_t * dev);

#endif 

