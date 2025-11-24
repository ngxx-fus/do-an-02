/// @file   i2cHelpers.h
/// @brief  Helper functions and macros for I2C driver; Includes pin operations, start/stop conditions, data/address frame handling, buffer management, and device assertions.

#ifndef __I2C_HELPERS_H__
#define __I2C_HELPERS_H__

#include "../../helper/general.h"
#include "i2cDefinition.h"
#include "i2cLog.h"

/// @brief Assert that the I2C device pointer is valid. If not, call esp_restart().
/// @param funcName Function name where the check is invoked.
/// @param i2cDev   Pointer to the I2C device.
/// @param devName  String name of the I2C device variable.
#define __I2C_ASSERT_DEV_OK(funcName, i2cDev, devName)     \
    do {                                                   \
        if(__is_null(i2cDev)){                             \
            __i2cErr("[%s] %s = %p is invalid",            \
                            funcName, devName, i2cDev);    \
            esp_restart();                                 \
            __builtin_unreachable();                       \
        }                                                  \
    } while(0)                                  

/// @brief Assert that the I2C device pointer is valid. If not, call return ERR_NULL.
/// @param funcName Function name where the check is invoked.
/// @param i2cDev   Pointer to the I2C device.
/// @param devName  String name of the I2C device variable.
/// @return call ERR_NULL if dev is null.
#define __I2C_ASSERT_DEV_OK1(funcName, i2cDev, devName)    \
    do {                                                   \
        if(__is_null(i2cDev)){                             \
            __i2cErr("[%s] %s = %p is invalid",            \
                            funcName, devName, i2cDev);    \
            return ERR_NULL;                               \
        }                                                  \
    } while(0)   


/// @brief Assert that the I2C pin configuration is valid.
/// @param funcName Function name where the check is invoked.
/// @param pinValue Numeric pin value.
/// @param pinName  String name of the pin variable.
#define __I2C_ASSERT_PIN_OK(funcName, pinValue, pinName)    \
    do {                                                    \
        if(__isnot_positive(pinValue)){                     \
            __i2cErr("[%s] %s = %d is invalid",             \
                     funcName, pinName, pinValue);          \
            esp_restart();                                  \
            __builtin_unreachable();                        \
        }                                                   \
    } while(0)

/// @brief Delay function wrapper for I2C.
/// @param us Delay time in microseconds.
#define __i2cDelay(us) esp_rom_delay_us(us)

/// @brief Get I2C bus frequency of a given device.
/// @param dev Pointer to I2C device.
/// @return Frequency in Hz.
/// @note Will reset system if frequency is invalid.
static inline  __attribute__((always_inline)) 
uint32_t __i2cFreq(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK("__i2cFreq", dev->freq, STR(freq));
    return dev->freq;
}

/// @brief Get the SCL (clock) pin of a given I2C device.
/// @param dev Pointer to I2C device.
/// @return Pin number for SCL.
/// @note Will reset system if pin is invalid.
static inline  __attribute__((always_inline)) 
pin_t __i2cSCL(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(STR(__i2cSCL), dev->sda, STR(dev->sda));
    return dev->scl;
}

/// @brief Get the SDA (data) pin of a given I2C device.
/// @param dev Pointer to I2C device.
/// @return Pin number for SDA.
/// @note Will reset system if pin is invalid.
static inline  __attribute__((always_inline)) 
pin_t __i2cSDA(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(STR(__i2cSDA), dev->scl, STR(dev->scl));
    return dev->sda;
}

/// @brief Set logic level for SDA line of an I2C device.
/// @param dev Pointer to I2C device.
/// @param level Logic level (0 = low, 1 = high).
/// @note Will reset system if SDA pin is invalid.
static inline  __attribute__((always_inline)) 
void __i2cSetSDA(i2cDev_t * dev, def level){
    __I2C_ASSERT_PIN_OK(STR(__i2cSetSDA), dev->sda, STR(dev->sda));
    if(level)
        GPIO.out_w1ts = __mask32(dev->sda);
    else
        GPIO.out_w1tc = __mask32(dev->sda);
}

/// @brief Set logic level for SCL line of an I2C device.
/// @param dev Pointer to I2C device.
/// @param level Logic level (0 = low, 1 = high).
/// @note Will reset system if SCL pin is invalid.
static inline  __attribute__((always_inline)) 
void __i2cSetSCL(i2cDev_t * dev, def level){
    __I2C_ASSERT_PIN_OK(STR(__i2cSetSCL), dev->scl, STR(dev->scl));
    if(level)
        GPIO.out_w1ts = __mask32(dev->scl);
    else
        GPIO.out_w1tc = __mask32(dev->scl);
}

/// @brief Read the current logic level of SDA line.
/// @param dev Pointer to I2C device.
/// @return Current logic level of SDA (0 or 1).
/// @note Will reset system if SDA pin is invalid.
static inline def  __attribute__((always_inline)) 
__i2cGetSDA(i2cDev_t * dev){
    // __I2C_ASSERT_PIN_OK(STR(__i2cGetSDA), dev->sda, STR(dev->sda));
    return boolCast(GPIO.in & __mask32(dev->sda));
}

/// @brief Read the current logic level of SCL line.
/// @param dev Pointer to I2C device.
/// @return Current logic level of SCL (0 or 1).
/// @note Will reset system if SCL pin is invalid.
static inline  __attribute__((always_inline)) 
def __i2cGetSCL(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(STR(__i2cGetSCL), dev->scl, STR(dev->scl));
    return boolCast(GPIO.in & __mask32(dev->scl));
}

/// @brief Generate I2C START condition
/// @param dev Pointer to I2C device
static inline  __attribute__((always_inline)) 
void i2cStartCondition(i2cDev_t * dev){

    __i2cDelay(250000/(__i2cFreq(dev)));

    /// Release both SCL, SDA line
    __i2cSetSCL(dev, HIGH);
    __i2cSetSDA(dev, HIGH);

    /// Wait for both SCL and SDA are stable (a half of SCL period)
    __i2cDelay(500000/(__i2cFreq(dev)));

    /// Make start condition (SDA down to LOW when SCL is HIGH)
    __i2cSetSDA(dev, LOW);

    /// Wait for a quarter of SCL period
    __i2cDelay(250000/(__i2cFreq(dev)));
    
    /// Pull SCL down for 1st clock pulse
    __i2cSetSCL(dev, LOW);
}

/// @brief Generate I2C STOP condition
/// @param dev Pointer to I2C device
static inline  __attribute__((always_inline)) 
void i2cStopCondition(i2cDev_t * dev){

    __i2cDelay(250000/(__i2cFreq(dev)));

    /// Make sure both SCL, SDA line are LOW 
    __i2cSetSCL(dev, LOW);
    __i2cSetSDA(dev, LOW);

    /// Wait for both SCL and SDA are stable (a half of SCL period)
    __i2cDelay(500000/(__i2cFreq(dev)));

    /// Pull SCL up to HIGH for stop condition
    __i2cSetSCL(dev, HIGH);
    
    /// Wait for a quarter of SCL period
    __i2cDelay(250000/(__i2cFreq(dev)));
    
    /// Make stop condition (SDA is pulled up to HIGH when SCL is HIGH)
    __i2cSetSDA(dev, HIGH);
}

/// @brief Send I2C address frame with R/W bit
/// @param dev Pointer to I2C device
/// @param addr7 7-bit slave address
/// @param rw Read/Write mode (I2C_WRITE_MODE or I2C_READ_MODE)
static inline  __attribute__((always_inline)) 
void i2cSendAddressFrame(i2cDev_t * dev, uint8_t addr7, uint8_t rw){

    uint8_t addressFrame = 0;

    if((rw & 1U) == I2C_WRITE_MODE){
        /// WRITE MODE
        addressFrame = (addr7 << 1);
    }else{
        /// SLAVE MODE
        addressFrame = (addr7 << 1) | 1U;
    }
    // __i2cLog1("addressFrame = 0x%04x", addressFrame);
    /// Start sending 8 bit of address frame from MSB to LSB
    for(uint8_t mask = 0x80; mask; mask  >>= 1){
        
        /// Export bit data to SDA
        __i2cSetSDA(dev, boolCast(mask & addressFrame));
        /// Make mono pulse of SCL
        __i2cDelay(500000/__i2cFreq(dev));
        __i2cSetSCL(dev, HIGH);
        __i2cDelay(500000/__i2cFreq(dev)); 
        __i2cSetSCL(dev, LOW);      
    }
}

/// @brief Send one I2C data frame (8-bit)
/// @param dev Pointer to I2C device
/// @param byte Data byte to send
static inline  __attribute__((always_inline)) 
void i2cSendDataFrame(i2cDev_t * dev, uint8_t byte){

    uint32_t dataFrame = byte; /// <-- small fix: should assign from input byte
    // __i2cLog1("dataFrame = 0x%04x", dataFrame);

    /// Start sending 8 bit of data frame from MSB to LSB
    for(uint32_t mask = 0x80; mask; mask  >>= 1){
        
        /// Export bit data to SDA
        __i2cSetSDA(dev, boolCast(mask & dataFrame));

        /// Make mono pulse of SCL
        __i2cDelay(500000/__i2cFreq(dev));
        __i2cSetSCL(dev, HIGH);
        __i2cDelay(500000/__i2cFreq(dev));
        __i2cSetSCL(dev, LOW);
    }
}

/// @brief Release SDA, then get return bit
/// @param dev I2c device
/// @return 
static inline __attribute__((always_inline)) 
def i2cGetReturnBit(i2cDev_t * dev){
    /// Release SDA line
    __i2cSetSDA(dev, HIGH);

    /// Make raising edge
    __i2cSetSCL(dev, LOW);
    __i2cDelay(500000/__i2cFreq(dev));

    __i2cSetSCL(dev, HIGH);
    __i2cDelay(500000/__i2cFreq(dev));
    def ret = __i2cGetSDA(dev);

    __i2cSetSCL(dev, LOW);

    return ret;
}

/// @brief Reset I2C transmit buffer index (bit/byte) to zero.
/// @param dev Pointer to I2C device context.
/// @return OKE on success, ERR_NULL if dev is null.
static inline __attribute__((always_inline)) 
def i2cResetIndexTransmitBuffer(i2cDev_t * dev){
    if(__is_null(dev)){
        __i2cErr("[i2cResetIndexTransmitBuffer] Dev is null!");
        return ERR_NULL;
    }
    (dev->txIndBit) = 0;
    (dev->txIndByte) = 0;
    return OKE;
}

/// @brief Reset I2C receive buffer index (bit/byte) to zero.
/// @param dev Pointer to I2C device context.
/// @return OKE on success, ERR_NULL if dev is null.
static inline __attribute__((always_inline)) 
def i2cResetIndexReceiveBuffer(i2cDev_t * dev){
    if(__is_null(dev)){
        __i2cErr("[i2cResetIndexReceiveBuffer] Dev is null!");
        return ERR_NULL;
    }
    (dev->rxIndBit) = 0;
    (dev->rxIndByte) = 0;
    return OKE;
}

/// @brief Clear I2C transmit buffer and reset its indices.
/// @param dev Pointer to I2C device context.
/// @return OKE on success, ERR_NULL if dev is null.
static inline __attribute__((always_inline)) 
def i2cClearTransmitBuffer(i2cDev_t * dev){
    if(__is_null(dev)){
        __i2cErr("[i2cClearTransmitBuffer] Dev is null!");
        return ERR_NULL;
    }
    (dev->txIndBit) = 0;
    (dev->txIndByte) = 0;
    memset(dev->txPtr, 0, dev->txSize);
    return OKE;
}

/// @brief Clear I2C receive buffer and reset its indices.
/// @param dev Pointer to I2C device context.
/// @return OKE on success, ERR_NULL if dev is null.
static inline __attribute__((always_inline)) 
def i2cClearReceiveBuffer(i2cDev_t * dev){
    if(__is_null(dev)){
        __i2cErr("[i2cClearReceiveBuffer] Dev is null!");
        return ERR_NULL;
    }
    (dev->rxIndBit) = 0;
    (dev->rxIndByte) = 0;
    memset(dev->rxPtr, 0, dev->rxSize);
    return OKE;
}

/// @brief Just receive 8 bit from SDA (without start/stop condition and send anything) 
/// @param dev Pointer to I2C device context.
/// @return A non negative number (received byte) if success, negative if error.
static inline __attribute__((always_inline)) 
def i2cReceiveByte(i2cDev_t * dev){
    def rcvByte = 0;
    __i2cSetSCL(dev, LOW);

    REP(i, 0, 8){
        __i2cDelay(500000/__i2cFreq(dev));
        __i2cSetSCL(dev, HIGH);
        rcvByte = (rcvByte << 1) | __i2cGetSDA(dev);
        __i2cDelay(500000/__i2cFreq(dev));
        __i2cSetSCL(dev, LOW);
    }
    return rcvByte;
}

#endif