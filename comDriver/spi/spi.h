#ifndef __SPI_H__
#define __SPI_H__

#include "../../helper/general.h"

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
    #define __spiErr(...)       __err( __VA_ARGS__ )
#else 
    #define __spiErr(...)
#endif
#if SPI_LOG_LEVEL >= 2
    #define __spiEntry(...)     __entry( __VA_ARGS__ )
    #define __spiExit(...)      __exit( __VA_ARGS__ )
#else 
    #define __spiEntry(...)
    #define __spiExit(...)
#endif 
#if SPI_LOG_LEVEL >= 3
    #define __spiLog(...)      __log( __VA_ARGS__ )
#else 
    #define __spiLog(...)
#endif 
#if SPI_LOG_LEVEL >= 4
    #define __spiLog1(...)      __log( __VA_ARGS__ )
#else 
    #define __spiLog1(...)
#endif 

/// THE DEFINITIONSRELATED TO SPI /////////////////////////////////////////////////////////////////

/// @brief SPI device descriptor
typedef struct spiDev_t {
    pin_t       clk;        ///< Clock pin
    pin_t       mosi;       ///< MOSI pin
    pin_t       miso;       ///< MISO pin
    pin_t       cs;         ///< Chip select pin
    flag_t      conf;       ///< Configuration flags (mode, CPOL, CPHA)
    uint32_t    freq;       ///< Frequency in Hz
    void*       txdPtr;     ///< Pointer to TX buffer
    void*       rxdPtr;     ///< Pointer to RX buffer
    size_t      rxdSize;    ///< RX buffer size in bytes
    size_t      rxdByteInd; ///< Current RX byte index
    size_t      txdSize;    ///< TX buffer size in bytes
    size_t      txdByteInd; ///< Current TX byte index
    uint8_t     rxdBitInd;  ///< RX bit index within current byte
    uint8_t     txdBitInd;  ///< TX bit index within current byte
    portMUX_TYPE mutex;
} spiDev_t;

/// @brief SPI device role
enum SPI_DEVICE {
    SPI_MASTER = 0, ///< Master mode
    SPI_SLAVE  = 1  ///< Slave mode
};

/// @brief SPI configuration parameters
enum SPI_CONFIG {
    SPI_MODE = 0,   ///< Master or Slave mode
    SPI_CPOL = 1,   ///< Clock polarity
    SPI_CPHA = 2    ///< Clock phase
};

/// HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////////

/// @brief Shift the entire byte array left by 1 bit, insert newLSB at the very end.
/// @param arr   Pointer to the byte array
/// @param size  Number of bytes in the array
/// @param newLSB The bit (0 or 1) to put in the least significant bit of the last byte
/// @return Default return status
 def shiftArrayLeft(uint8_t * arr, size_t size, uint8_t newLSB);

/// @brief Shift the entire byte array right by 1 bit, insert newMSB at the very front.
/// @param arr    Pointer to the byte array
/// @param size   Number of bytes in the array
/// @param newMSB The bit (0 or 1) to put in the most significant bit of the first byte
/// @return Default return status
def shiftArrayRight(uint8_t * arr, size_t size, uint8_t newMSB);

/// @brief Shift the entire byte array left by 1 byte, insert newByte at the last position.
/// @param arr     Pointer to the byte array
/// @param size    Number of bytes in the array
/// @param newByte The new byte to insert at the end
/// @return Default return status
def shifByteLeft(uint8_t *arr, size_t size, uint8_t newByte);

/// @brief Shift the entire byte array right by 1 byte, insert newByte at the front.
/// @param arr     Pointer to the byte array
/// @param size    Number of bytes in the array
/// @param newByte The new byte to insert at the beginning
/// @return Default return status
def shiftByteRight(uint8_t *arr, size_t size, uint8_t newByte);

/// MAIN FUNCTIONS ////////////////////////////////////////////////////////////////////////////////

/// SLAVE INTERRUPT ///////////////////////////////////////////////////////////////////////////////

void IRAM_ATTR spiHandleCLKIsr(void* pv) {
    spiDev_t *dev = (spiDev_t*) pv;

    uint8_t *txBuf = (uint8_t*)dev->txdPtr;
    uint8_t *rxBuf = (uint8_t*)dev->rxdPtr;

    if (__hasFlagBitClr(dev->conf, SPI_CPHA)) {
        /// CPHA = 0
        if (__is_positive(GPIO.in & __mask32(dev->clk))) {
            /// Rising edge: sample MOSI
            if (rxBuf && dev->rxdByteInd < dev->rxdSize) {
                uint8_t bit = (__is_positive(GPIO.in & __mask32(dev->mosi))) ? 1 : 0;
                rxBuf[dev->rxdByteInd] <<= 1;
                rxBuf[dev->rxdByteInd] |= bit;
                dev->rxdBitInd++;

                if (dev->rxdBitInd >= 8) {
                    dev->rxdBitInd = 0;
                    dev->rxdByteInd++;
                }
            }
        } else {
            /// Falling edge: export MISO
            if (txBuf && dev->txdByteInd < dev->txdSize) {
                uint8_t outBit = (txBuf[dev->txdByteInd] & (0x80 >> dev->txdBitInd)) ? 1 : 0;
                if (outBit)
                    GPIO.out_w1ts = __mask32(dev->miso);
                else
                    GPIO.out_w1tc = __mask32(dev->miso);

                dev->txdBitInd++;
                if (dev->txdBitInd >= 8) {
                    dev->txdBitInd = 0;
                    dev->txdByteInd++;
                }
            }
        }
    } else {
        /// CPHA = 1
        if (__is_positive(GPIO.in & __mask32(dev->clk))) {
            /// Rising edge: export MISO
            if (txBuf && dev->txdByteInd < dev->txdSize) {
                uint8_t outBit = (txBuf[dev->txdByteInd] & (0x80 >> dev->txdBitInd)) ? 1 : 0;
                if (outBit)
                    GPIO.out_w1ts = __mask32(dev->miso);
                else
                    GPIO.out_w1tc = __mask32(dev->miso);

                dev->txdBitInd++;
                if (dev->txdBitInd >= 8) {
                    dev->txdBitInd = 0;
                    dev->txdByteInd++;
                }
            }
        } else {
            /// Falling edge: sample MOSI
            if (rxBuf && dev->rxdByteInd < dev->rxdSize) {
                uint8_t bit = (__is_positive(GPIO.in & __mask32(dev->mosi))) ? 1 : 0;
                rxBuf[dev->rxdByteInd] <<= 1;
                rxBuf[dev->rxdByteInd] |= bit;
                dev->rxdBitInd++;

                if (dev->rxdBitInd >= 8) {
                    dev->rxdBitInd = 0;
                    dev->rxdByteInd++;
                }
            }
        }
    }
}

void IRAM_ATTR spiHandleCSIsr(void* pv) {
    spiDev_t *dev = (spiDev_t*) pv;

    if (__is_positive(GPIO.in & __mask32(dev->cs))) {
        /// CS rising edge (ngắt giao dịch)

        gpio_intr_disable(dev->clk);

        /// Reset TX index
        dev->txdByteInd = 0;
        dev->txdBitInd  = 0;

    } else {
        /// CS falling edge (bắt đầu giao dịch)

        gpio_intr_enable(dev->clk);

        /// Reset RX index
        dev->rxdByteInd = 0;
        dev->rxdBitInd  = 0;
    }
}


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
/// @note - In slave mode, txdPtr, txdSize, rxdPtr, rxdSize have to be set before any transaction!
def configSPIDevice(spiDev_t * dev,pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS,uint32_t freq, flag_t config);

/// @brief Startup (init gpio, ...) base on the config of spiDev
/// @param dev A pointer to the place which is storing the spiDev_t
/// @return Default return status
int startupSPIDevice(spiDev_t * dev);

/// @brief Set SPI transmit buffer
/// @param dev A pointer to the place which is storing the spiDev_t
/// @param txdPtr The pointer to transmit buffer
/// @param size Size of the transmit buffer (in byte)
/// @return Default return status
def spiSetTransmitBuffer(spiDev_t * dev, void * txdPtr, size_t size);

/// @brief Set SPI receive buffer
/// @param dev A pointer to the place which is storing the spiDev_t
/// @param rxdPtr The pointer to receive buffer
/// @param size Size of the receive buffer (in byte)
/// @return Default return status
def spiSetReceiveBuffer(spiDev_t * dev, void * rxdPtr, size_t size);

/// @brief Start the spi transaction (Master only)
/// @param dev Pointer to spiDev_t
/// @return <=0 if error occured; >0 # of received bytes
def startTransaction(spiDev_t * dev);


#endif