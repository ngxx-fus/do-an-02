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
    #define __spiErr(...)       __err( ##__VA_ARGS__ )
#else 
    #define __spiErr(...)
#endif
#if SPI_LOG_LEVEL >= 2
    #define __spiEntry(...)     __entry( ##__VA_ARGS__ )
    #define __spiExit(...)      __exit( ##__VA_ARGS__ )
#else 
    #define __spiEntry(...)
    #define __spiExit(...)
#endif 
#if SPI_LOG_LEVEL >= 3
    #define __spiLog(...)      __log( ##__VA_ARGS__ )
#else 
    #define __spiLog(...)
#endif 
#if SPI_LOG_LEVEL >= 4
    #define __spiLog1(...)      __log( ##__VA_ARGS__ )
#else 
    #define __spiLog1(...)
#endif 

/// THE DEFINITIONSRELATED TO SPI /////////////////////////////////////////////////////////////////

typedef struct spiDev_t{
    pin_t       clk, mosi, miso, cs;
    flag_t      conf;
    uint32_t    freq;
    void*       txdPtr, *rxdPtr;
    size_t      rxdSize, txdSize;
} spiDev_t;

enum SPI_DEVICE{
    SPI_MASTER = 0,
    SPI_SLAVE = 1
};

enum SPI_CONFIG{
    SPI_MODE = 0,       /// MASTER or SLAVE
    SPI_CPOL = 1,       /// Clock pol
    SPI_CPHA = 2,       /// Clock phase
};

/// HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////////

/// @brief Shift the entire byte array left by 1 bit, insert newLSB at the very end.
/// @param arr   Pointer to the byte array
/// @param size  Number of bytes in the array
/// @param newLSB The bit (0 or 1) to put in the least significant bit of the last byte
/// @return Default return status
static inline def shiftArrayLeft(uint8_t * arr, size_t size, uint8_t newLSB) {
    if(__is_null(arr)){
        __spiErr("[shiftLeft] arr = %p is invalid!", arr);
        return ERR;
    }
    if(__isnot_positive(size)){
        __spiErr("[shiftLeft] size = %d is invalid!", size);
        return ERR;
    }
    uint8_t carry = 0;
    for (size_t i = 0; i < size; i++) {
        uint8_t next_carry = (arr[i] & 0x80) ? 1 : 0;
        arr[i] = (uint8_t)((arr[i] << 1) | carry);
        carry = next_carry;
    }
    arr[size - 1] = (uint8_t)((arr[size - 1] & 0xFE) | (newLSB & 0x01));
}

/// @brief Shift the entire byte array right by 1 bit, insert newMSB at the very front.
/// @param arr    Pointer to the byte array
/// @param size   Number of bytes in the array
/// @param newMSB The bit (0 or 1) to put in the most significant bit of the first byte
/// @return Default return status
static inline def shiftArrayRight(uint8_t * arr, size_t size, uint8_t newMSB) {
    if (__is_null(arr)) {
        __spiErr("[shiftRight] arr = %p is invalid!", arr);
        return ERR;
    }
    if (__isnot_positive(size)) {
        __spiErr("[shiftRight] size = %d is invalid!", size);
        return ERR;
    }

    uint8_t carry = 0;
    for (size_t i = size; i-- > 0;) {  // iterate backwards
        uint8_t next_carry = (arr[i] & 0x01) ? 1 : 0;
        arr[i] = (uint8_t)((arr[i] >> 1) | (carry << 7));
        carry = next_carry;
    }

    // set MSB of first byte
    arr[0] = (uint8_t)((arr[0] & 0x7F) | ((newMSB & 0x01) << 7));

    return OKE;
}

/// @brief Shift the entire byte array left by 1 byte, insert newByte at the last position.
/// @param arr     Pointer to the byte array
/// @param size    Number of bytes in the array
/// @param newByte The new byte to insert at the end
/// @return Default return status
static inline def shifByteLeft(uint8_t *arr, size_t size, uint8_t newByte) {
    if (__is_null(arr)) {
        __spiErr("[shifByteLeft] arr = %p is invalid!", arr);
        return ERR;
    }
    if (__isnot_positive(size)) {
        __spiErr("[shifByteLeft] size = %d is invalid!", size);
        return ERR;
    }

    for (size_t i = 0; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
    arr[size - 1] = newByte;

    return OKE;
}

/// @brief Shift the entire byte array right by 1 byte, insert newByte at the front.
/// @param arr     Pointer to the byte array
/// @param size    Number of bytes in the array
/// @param newByte The new byte to insert at the beginning
/// @return Default return status
static inline def shiftByteRight(uint8_t *arr, size_t size, uint8_t newByte) {
    if (__is_null(arr)) {
        __spiErr("[shiftByteRight] arr = %p is invalid!", arr);
        return ERR;
    }
    if (__isnot_positive(size)) {
        __spiErr("[shiftByteRight] size = %d is invalid!", size);
        return ERR;
    }

    for (size_t i = size; i-- > 1;) {
        arr[i] = arr[i - 1];
    }
    arr[0] = newByte;

    return OKE;
}


/// MAIN FUNCTIONS ////////////////////////////////////////////////////////////////////////////////

/// @brief Create new spiDev_t
/// @param pDev the pointer point to the pointer that point the place which will store spiDev_t
/// @return Default return status
def createSPIDevice(spiDev_t ** pDev);

/// @brief Config a spi device
/// @param Dev A pointer to the place which is storing the spiDev_t
/// @param CLK CLK pin, set -1 if unused!
/// @param MOSI MOSI pin, set -1 if unused!
/// @param MISO MISO pin, set -1 if unused!
/// @param CS CS pin, set -1 if unused!
/// @param freq Set the frequency of CLK
/// @param config Set MODE, CPOL, CPHA bit.
/// @return Default return status
/// @note Config flag: MSB<[31]-[30]-...-[2:CPHA]-[1:CPOL]-[0:MODE]>LSB
def configSPIDevice(spiDev_t * Dev,pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS,uint32_t freq, flag_t config);



#endif