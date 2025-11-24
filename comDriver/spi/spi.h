<<<<<<< HEAD
=======
/// @file   spi.h
/// @brief  SPI Driver Definitions; Defines structs, enums, macros, and interfaces for software SPI (Master/Slave).
/// @details 
/// <b>Transmit buffer layout:</b>
/// @code
///             ____________________ 0x80 (start transmitting)
///            /             _______ 0x01 (stop transmitting)
///           /             /
///         7 6 5 4 3 2 1 0 
/// 0: [MSB - - - - - - - - LSB]
/// 1: [MSB - - - - - - - - LSB]
/// ...
/// Data transmit start at buffer[0], from MSB to LSB.
/// @endcode
///
/// <b>Receiver buffer layout:</b>
/// @code
///             ____________________ 0x80 (stop receiving)
///            /             _______ 0x01 (start receiving)
///           /             /
///         7 6 5 4 3 2 1 0 
/// 0: [MSB - - - - - - - - LSB]
/// ...
/// Data is receiving from MSB to LSB in a byte, and fill from 0 in a buffer.
/// @endcode

>>>>>>> master
#ifndef __SPI_H__
#define __SPI_H__

#include "../../helper/general.h"

<<<<<<< HEAD
/*
----------------------------------

Transmit buffer
           ____________________ 0x80 (start transmitting)
          /             _______ 0x01 (stop transmitting)
         /             /
        7 6 5 4 3 2 1 0 
0: [MSB - - - - - - - - LSB]
1: [MSB - - - - - - - - LSB]
1: [MSB - - - - - - - - LSB]
2: [MSB - - - - - - - - LSB]
3: [MSB - - - - - - - - LSB]
4: [MSB - - - - - - - - LSB]
index

Data transmit start at buffer[0], from MSB to LSB.

----------------------------------
Receiver buffer
           ____________________ 0x80 (stop receiving)
          /             _______ 0x01 (start receiving)
         /             /
        7 6 5 4 3 2 1 0 
0: [MSB - - - - - - - - LSB]
1: [MSB - - - - - - - - LSB]
1: [MSB - - - - - - - - LSB]
2: [MSB - - - - - - - - LSB]
3: [MSB - - - - - - - - LSB]
4: [MSB - - - - - - - - LSB]

Data is receiving from MSB to LSB in a byte, and fill from 0 in a buffer.

*/

/// LOG CONTROL ///////////////////////////////////////////////////////////////////////////////////

/*
 *  Set level of log, there are the description of the leves:
 *  Noted that: the greater value of log includes all lower log types!:
 *      1 : Only log error
 *      2 : Log entry (+ params) and exit (+ return status)
 *      3 : Log state inside the function, ...
 *      4 : The lowest level of logs, it log all things, includes every byte will be sent!
*/
#define SPI_LOG_LEVEL 3

#if SPI_LOG_LEVEL >= 1
=======
/// @cond
/// LOG CONTROL ///////////////////////////////////////////////////////////////////////////////////
/// @endcond

/**
 * @brief Set level of log.
 * 1: Only log error
 * 2: Log entry (+ params) and exit (+ return status)
 * 3: Log state inside the function
 * 4: Verbose log (includes every byte sent)
 */
#define SPI_LOG_LEVEL 3

#if SPI_LOG_LEVEL >= 1
    /// @brief Log error message
>>>>>>> master
    #define __spiErr(...)       __sys_err( __VA_ARGS__ )
#else 
    #define __spiErr(...)
#endif
<<<<<<< HEAD
#if SPI_LOG_LEVEL >= 2
    #define __spiEntry(...)     __entry( __VA_ARGS__ )
=======

#if SPI_LOG_LEVEL >= 2
    /// @brief Log function entry
    #define __spiEntry(...)     __entry( __VA_ARGS__ )
    /// @brief Log function exit
>>>>>>> master
    #define __spiExit(...)      __exit( __VA_ARGS__ )
#else 
    #define __spiEntry(...)
    #define __spiExit(...)
#endif 
<<<<<<< HEAD
#if SPI_LOG_LEVEL >= 3
    #define __spiLog(...)      __sys_log( __VA_ARGS__ )
#else 
    #define __spiLog(...)
#endif 
#if SPI_LOG_LEVEL >= 4
=======

#if SPI_LOG_LEVEL >= 3
    /// @brief Log general information
    #define __spiLog(...)       __sys_log( __VA_ARGS__ )
#else 
    #define __spiLog(...)
#endif 

#if SPI_LOG_LEVEL >= 4
    /// @brief Log verbose information
>>>>>>> master
    #define __spiLog1(...)      __sys_log( __VA_ARGS__ )
#else 
    #define __spiLog1(...)
#endif 

<<<<<<< HEAD
/// THE DEFINITIONSRELATED TO SPI /////////////////////////////////////////////////////////////////

#define __spiDelay(us)  esp_rom_delay_us(us);

/// @brief SPI device descriptor
typedef struct spiDev_t {
    pin_t       clk;        /// Clock pin
    pin_t       mosi;       /// MOSI pin
    pin_t       miso;       /// MISO pin
    pin_t       cs;         /// Chip select pin
    flag_t      conf;       /// Configuration flags (mode, CPOL, CPHA)
    uint32_t    freq;       /// Frequency in Hz
    
    flag_t      stat;     /// Status flags ()

    void*       tBuff;
    size_t      tBuffSize;
    size_t      tBuffIndex;
    size_t      tBuffMask;

    void*       rBuff;
    size_t      rBuffSize;
    size_t      rBuffIndex;
    size_t      rBuffMask;

    /// Private:

    uint8_t     __cpha;             /// NOTE: Only can be set via config function!
    uint8_t     __cpol;             /// NOTE: Only can be set via config function!
    uint8_t     __master_slave;     /// 0: Master | 1: Slave. NOTE: Only can be set via config function!

    portMUX_TYPE mutex;
} spiDev_t;

/// @brief SPI device role
=======
/// @cond
/// THE DEFINITIONSRELATED TO SPI /////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Delay microseconds wrapper
#define __spiDelay(us)  esp_rom_delay_us(us);

/// @brief SPI device descriptor structure
typedef struct spiDev_t {
    pin_t       clk;        ///< Clock pin
    pin_t       mosi;       ///< MOSI pin
    pin_t       miso;       ///< MISO pin
    pin_t       cs;         ///< Chip select pin
    flag_t      conf;       ///< Configuration flags (mode, CPOL, CPHA)
    uint32_t    freq;       ///< Frequency in Hz
    
    flag_t      stat;       ///< Status flags (buffer states)

    void* tBuff;      ///< Pointer to transmit buffer
    size_t      tBuffSize;  ///< Size of transmit buffer
    size_t      tBuffIndex; ///< Current index in transmit buffer
    size_t      tBuffMask;  ///< Current bit mask for transmission

    void* rBuff;      ///< Pointer to receive buffer
    size_t      rBuffSize;  ///< Size of receive buffer
    size_t      rBuffIndex; ///< Current index in receive buffer
    size_t      rBuffMask;  ///< Current bit mask for reception

    /// Private members:
    uint8_t     __cpha;           ///< Clock phase (0 or 1). Set via config function.
    uint8_t     __cpol;           ///< Clock polarity (0 or 1). Set via config function.
    uint8_t     __master_slave;   ///< Mode (0: Master | 1: Slave). Set via config function.

    portMUX_TYPE mutex;           ///< Mutex for thread safety
} spiDev_t;

/// @brief SPI device role enum
>>>>>>> master
enum SPI_DEVICE {
    SPI_MASTER = 0, /// Master mode
    SPI_SLAVE  = 1  /// Slave mode
};

<<<<<<< HEAD
/// @brief  SPI clk state (idle/active)
enum SPI_CLK_STATE {
    SPICLK_IDLE = 0,
    SPICLK_ACTIVE = 1,
};

/// @brief SPI configuration parameters
enum SPI_BIT_ORDER_CONFIG {
    SPI_MODE = 0,   /// Master or Slave mode
    SPI_CPOL = 1,   /// Clock polarity
    SPI_CPHA = 2,    /// Clock phase
    SPI_BIT_ORDER_CONFIG_COUNT,
};

/// @brief SPI configuration parameters
enum SPI_PRESET_CONFIG {
    SPI_00_MASTER = 0,   /// CPHA = 0 | CPOL = 0 | MATER MODE
    SPI_01_MASTER = 2,   /// CPHA = 0 | CPOL = 1 | MATER MODE
    SPI_10_MASTER = 4,   /// CPHA = 1 | CPOL = 0 | MATER MODE
    SPI_11_MASTER = 6,   /// CPHA = 1 | CPOL = 1 | MATER MODE
    SPI_00_SLAVE = 1,    /// CPHA = 0 | CPOL = 0 | SLAVE MODE
    SPI_01_SLAVE = 3,    /// CPHA = 0 | CPOL = 1 | SLAVE MODE
    SPI_10_SLAVE = 5,    /// CPHA = 1 | CPOL = 0 | SLAVE MODE
    SPI_11_SLAVE = 7,    /// CPHA = 1 | CPOL = 1 | SLAVE MODE
    SPI_PRESET_CONFIG_COUNT
};

enum SPI_STATUS_FLAG {
    SPISTAT_TBUFF_FULL = 0, /// After assign buffer (0: Don't care | 1: Full)
    SPISTAT_RBUFF_FULL,     /// Full, can not receive anymore! (0: Don't care | 1: Full)
    SPISTAT_TBUFF_EMPTY,    /// Nothing to be send! (0: Don't care | 1: Empty)
    SPISTAT_RBUFF_EMPTY,    /// After assign buffer! (0: Don't care | 1: Empty)
    SPI_STATUS_FLAG_NUM
};

/// HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////////

/// @brief Set CLK base on CPOL with IDLE/ACTIVE state
/// @param dev Spi descriptor
/// @param state LOW : idle state | HIGH : active state 
=======
/// @brief SPI clock state (idle/active)
enum SPI_CLK_STATE {
    SPICLK_IDLE = 0,    ///< Clock line is in idle state
    SPICLK_ACTIVE = 1,  ///< Clock line is in active state
};

/// @brief SPI configuration bit positions
enum SPI_BIT_ORDER_CONFIG {
    SPI_MODE = 0,   ///< Master or Slave mode bit index
    SPI_CPOL = 1,   ///< Clock polarity bit index
    SPI_CPHA = 2,   ///< Clock phase bit index
    SPI_BIT_ORDER_CONFIG_COUNT,
};

/// @brief SPI preset configuration combinations
enum SPI_PRESET_CONFIG {
    SPI_00_MASTER = 0,   ///< CPHA=0 | CPOL=0 | MASTER MODE
    SPI_01_MASTER = 2,   ///< CPHA=0 | CPOL=1 | MASTER MODE
    SPI_10_MASTER = 4,   ///< CPHA=1 | CPOL=0 | MASTER MODE
    SPI_11_MASTER = 6,   ///< CPHA=1 | CPOL=1 | MASTER MODE
    SPI_00_SLAVE = 1,    ///< CPHA=0 | CPOL=0 | SLAVE MODE
    SPI_01_SLAVE = 3,    ///< CPHA=0 | CPOL=1 | SLAVE MODE
    SPI_10_SLAVE = 5,    ///< CPHA=1 | CPOL=0 | SLAVE MODE
    SPI_11_SLAVE = 7,    ///< CPHA=1 | CPOL=1 | SLAVE MODE
    SPI_PRESET_CONFIG_COUNT
};

/// @brief SPI internal status flags
enum SPI_STATUS_FLAG {
    SPISTAT_TBUFF_FULL = 0, /// Tx Buffer Full flag
    SPISTAT_RBUFF_FULL,     /// Rx Buffer Full flag
    SPISTAT_TBUFF_EMPTY,    /// Tx Buffer Empty flag
    SPISTAT_RBUFF_EMPTY,    /// Rx Buffer Empty flag
    SPI_STATUS_FLAG_NUM
};

/// @cond
/// HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Set CLK pin level based on CPOL and desired logical state (IDLE/ACTIVE)
/// @param dev Pointer to SPI device descriptor
/// @param state Desired logical state (SPICLK_IDLE or SPICLK_ACTIVE)
>>>>>>> master
static inline __attribute__((always_inline))
void spiSetCLKState(spiDev_t *dev, enum SPI_CLK_STATE state) {
    uint8_t     cpol = __hasFlagBitSet(dev->conf, SPI_CPOL);
    uint32_t    mask = __mask32(dev->clk);
    if (state == SPICLK_IDLE)
        (cpol ? (GPIO.out_w1ts = mask) : (GPIO.out_w1tc = mask));
    else
        (cpol ? (GPIO.out_w1tc = mask) : (GPIO.out_w1ts = mask));
}

<<<<<<< HEAD
/// @brief Set GPIO to HIGH or LOW
=======
/// @brief Set GPIO pin to specific level
/// @param gpio GPIO pin number
/// @param level Desired level (HIGH/LOW)
>>>>>>> master
static inline __attribute__((always_inline))
void spiSetGPIO(uint32_t gpio, uint8_t level) {
    if (level == LOW)
        GPIO.out_w1tc = __mask32(gpio);
    else
        GPIO.out_w1ts = __mask32(gpio);
}

<<<<<<< HEAD
#define spiSetCS(dev, lvl)              spiSetGPIO((dev)->cs,   (lvl))
#define spiSetMOSI(dev, lvl)            spiSetGPIO((dev)->mosi, (lvl))
#define spiSetMISO_SlaveMode(dev, lvl)  spiSetGPIO((dev)->miso, (lvl))

/// MAIN FUNCTIONS ////////////////////////////////////////////////////////////////////////////////

/// @brief Create new spiDev_t
/// @param pDev the pointer point to the pointer that point the place which will store spiDev_t
/// @return Default return status
def createSPIDevice(spiDev_t ** pDev);

/// @brief Config a spi device
/// @param dev A pointer to the place which is storing the spiDev_t
/// @param CLK CLK pin, set -1 if unused!
/// @param MOSI MOSI pin, set -1 if unused!
/// @param MISO MISO pin, set -1 if unused!
/// @param CS CS pin, set -1 if unused!
/// @param freq Set the frequency of CLK
/// @param config Set MODE, CPOL, CPHA bit.
/// @return Default return status
/// @note - Config flag: MSB<[31]-[30]-...-[2:CPHA]-[1:CPOL]-[0:MODE]>LSB
/// @note - In slave mode, tBuff, tBuffSize, rBuff, rBuffSize have to be set before any transaction!
def configSPIDevice(spiDev_t * dev,pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS,uint32_t freq, flag_t config);

/// @brief Startup (init gpio, ...) base on the config of spiDev
/// @param dev A pointer to the place which is storing the spiDev_t
/// @return Default return status
def startupSPIDevice(spiDev_t * dev);

/// @brief Get transmit buffer size (in bytes)
/// @param dev Pointer to SPI device
/// @return Transmit buffer size (>0) or error code (<=0)
def spiGetTransmitSize(spiDev_t *dev);

/// @brief Get receive buffer size (in bytes)
/// @param dev Pointer to SPI device
/// @return Receive buffer size (>0) or error code (<=0)
def spiGetReceiveSize(spiDev_t *dev);

/// @brief Reset transmit buffer index 
/// @param dev A pointer to the place which is storing the spiDev_t
/// @return Default return status
def spiResetTransmitIndex(spiDev_t * dev);

/// @brief Reset receive buffer index 
/// @param dev A pointer to the place which is storing the spiDev_t
/// @return Default return status
def spiResetReceiveIndex(spiDev_t * dev);

/// @brief Set SPI transmit buffer
/// @param dev A pointer to the place which is storing the spiDev_t
/// @param tBuff The pointer to transmit buffer
/// @param size Size of the transmit buffer (in byte)
/// @return Default return status
def spiSetTransmitBuffer(spiDev_t * dev, void * tBuff, size_t size);

/// @brief Set SPI receive buffer
/// @param dev A pointer to the place which is storing the spiDev_t
/// @param rBuff The pointer to receive buffer
/// @param size Size of the receive buffer (in byte)
/// @return Default return status
def spiSetReceiveBuffer(spiDev_t * dev, void * rBuff, size_t size);

/// @brief Start the spi transaction (Master only)
/// @param dev Pointer to spiDev_t
/// @return <=0 if error occured; >0 # of received bytes
def spiStartTransaction(spiDev_t * dev);

/// @brief Destroy and free all resources associated with an SPI device.
///        This function disables interrupts, releases GPIO pins,
///        clears all internal pointers, and frees the device structure.
/// @param pDev Double pointer to the spiDev_t object.
///             After this call, *pDev will be set to NULL.
/// @return Default return status.
=======
/// @brief Helper macro to set Chip Select pin level
#define spiSetCS(dev, lvl)              spiSetGPIO((dev)->cs,   (lvl))
/// @brief Helper macro to set MOSI pin level
#define spiSetMOSI(dev, lvl)            spiSetGPIO((dev)->mosi, (lvl))
/// @brief Helper macro to set MISO pin level (Slave mode)
#define spiSetMISO_SlaveMode(dev, lvl)  spiSetGPIO((dev)->miso, (lvl))

/// @cond
/// MAIN FUNCTIONS ////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Allocate memory for a new SPI device structure
/// @param pDev Double pointer to store the address of the new spiDev_t
/// @return Default return status (OKE/ERR)
def createSPIDevice(spiDev_t ** pDev);

/// @brief Configure a SPI device
/// @param dev Pointer to the spiDev_t structure
/// @param CLK Clock pin number (-1 if unused)
/// @param MOSI MOSI pin number (-1 if unused)
/// @param MISO MISO pin number (-1 if unused)
/// @param CS Chip Select pin number (-1 if unused)
/// @param freq Clock frequency in Hz
/// @param config Configuration flags (MODE, CPOL, CPHA)
/// @return Default return status
/// @note Config flag format: MSB<...-[2:CPHA]-[1:CPOL]-[0:MODE]>LSB
/// @note In slave mode, buffers must be set before any transaction!
def configSPIDevice(spiDev_t * dev,pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS,uint32_t freq, flag_t config);

/// @brief Initialize GPIOs and interrupts based on configuration
/// @param dev Pointer to the spiDev_t structure
/// @return Default return status
def startupSPIDevice(spiDev_t * dev);

/// @brief Get the configured transmit buffer size
/// @param dev Pointer to SPI device
/// @return Transmit buffer size in bytes (>0) or error code (<=0)
def spiGetTransmitSize(spiDev_t *dev);

/// @brief Get the configured receive buffer size
/// @param dev Pointer to SPI device
/// @return Receive buffer size in bytes (>0) or error code (<=0)
def spiGetReceiveSize(spiDev_t *dev);

/// @brief Reset transmit buffer index to 0
/// @param dev Pointer to the spiDev_t structure
/// @return Default return status
def spiResetTransmitIndex(spiDev_t * dev);

/// @brief Reset receive buffer index to 0
/// @param dev Pointer to the spiDev_t structure
/// @return Default return status
def spiResetReceiveIndex(spiDev_t * dev);

/// @brief Assign transmit buffer to the SPI device
/// @param dev Pointer to the spiDev_t structure
/// @param tBuff Pointer to the data buffer
/// @param size Size of the buffer in bytes
/// @return Default return status
def spiSetTransmitBuffer(spiDev_t * dev, void * tBuff, size_t size);

/// @brief Assign receive buffer to the SPI device
/// @param dev Pointer to the spiDev_t structure
/// @param rBuff Pointer to the data buffer
/// @param size Size of the buffer in bytes
/// @return Default return status
def spiSetReceiveBuffer(spiDev_t * dev, void * rBuff, size_t size);

/// @brief Start a SPI transaction (Master mode only)
/// @param dev Pointer to spiDev_t
/// @return <=0 if error occurred; >0 number of received bytes
def spiStartTransaction(spiDev_t * dev);

/// @brief Destroy SPI device and free resources
///        Disables interrupts, resets GPIOs, and frees memory.
/// @param pDev Double pointer to the spiDev_t object. Set to NULL after freeing.
/// @return Default return status
>>>>>>> master
def destroySPIDevice(spiDev_t **pDev);

#endif