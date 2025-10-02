#include "spi.h"

/// SLAVE INTERRUPT ///////////////////////////////////////////////////////////////////////////////

void IRAM_ATTR spiHandleCLKIsr(void* pv) {
    spiDev_t *dev = (spiDev_t*) pv;

    if (__hasFlagBitClr(dev->conf, SPI_CPHA)) {
        /// CPHA = 0
        if (__is_positive(GPIO.in & __mask32(dev->clk))) {
            /// Rising edge: sample MOSI
            if(__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)){
                if (__is_positive(GPIO.in & __mask32(dev->mosi))) {
                    shiftArrayLeft(dev->rxdPtr, dev->rxdSize, 1);
                } else {
                    shiftArrayLeft(dev->rxdPtr, dev->rxdSize, 0);
                }
            }
        } else {
            /// Falling edge: export MISO
            if(__isnot_null(dev->txdPtr) && __is_positive(dev->txdSize)){
                if (__is_positive(((uint8_t*)(dev->txdPtr))[0] & 0x80)) {
                    GPIO.out_w1ts = __mask32(dev->miso);   // or out1 if pin>=32
                } else {
                    GPIO.out_w1tc = __mask32(dev->miso);
                }
                shiftArrayLeft(dev->txdPtr, dev->txdSize, 0);
            }
        }
    } else {
        /// CPHA = 1
        if (__is_positive(GPIO.in & __mask32(dev->clk))) {
            /// Rising edge: export MISO
            if(__isnot_null(dev->txdPtr) && __is_positive(dev->txdSize)){
                if (__is_positive(((uint8_t*)(dev->txdPtr))[0] & 0x80)) {
                    GPIO.out_w1ts = __mask32(dev->miso);
                } else {
                    GPIO.out_w1tc = __mask32(dev->miso);
                }
                shiftArrayLeft(dev->txdPtr, dev->txdSize, 0);
            }
        } else {
            /// Falling edge: sample MOSI
            if(__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)){
                if (__is_positive(GPIO.in & __mask32(dev->mosi))) {
                    shiftArrayLeft(dev->rxdPtr, dev->rxdSize, 1);
                } else {
                    shiftArrayLeft(dev->rxdPtr, dev->rxdSize, 0);
                }
            }
        }
    }
}

void IRAM_ATTR spiHandleCSIsr(void* pv){
    spiDev_t *dev = (spiDev_t*) pv;

    if (__is_positive(GPIO.in & __mask32(dev->cs))) {
        /// Is raising edge
        
        /// Disable CLK isr!
        gpio_intr_disable(dev->clk);
    
        /// Clear buffer
        if(__isnot_null(dev->txdPtr) && __is_positive(dev->txdSize)){
            memset(dev->txdPtr, 0, dev->txdSize);
        }
    
    }else{
        /// Is falling edge

        /// Enable CLK isr!
        gpio_intr_enable(dev->clk);

        /// Clear buffer
        if(__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)){
            memset(dev->rxdPtr, 0, dev->rxdSize);
        }
    }
}

/// SPI ///////////////////////////////////////////////////////////////////////////////////////////

/// @brief Create new spiDev_t
/// @param pDev the pointer point to the pointer that point the place which will store spiDev_t
/// @return Default return status
def createSPIDevice(spiDev_t ** pDev){
    __spiEntry("createSPIDevice(spiDev_t ** pDev)");

    if(__is_null(pDev)) {
        __spiErr("pDev is null!");
        __spiExit("createSPIDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    (*pDev) = NULL;
    (*pDev) = (spiDev_t*) malloc(sizeof(spiDev_t));
    if(__is_null(*pDev)){
        __spiErr("Cannot allocate new spiDev_t!");
        __spiExit("createSPIDevice() : %s", STR(ERR_MALLOC_FAILED));
        return ERR_MALLOC_FAILED;
    }
    __spiExit("createSPIDevice() : %s", STR(OKE));
    return OKE;
}

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
def configSPIDevice(spiDev_t * dev,pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS,uint32_t freq, flag_t config){
    __spiEntry("configSPIDevice(dev: %p, CLK: %d, MOSI: %d, MISO: %d, CS: %d, freq: %d, config: 0x%04x)", 
                dev, CLK, MOSI, MISO, CS, freq, config);

    if(__is_null(dev)) {
        __spiErr("dev is null!");
        __spiExit("configSPIDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }
    
    /// Set new proterties

    dev->clk    = CLK;
    dev->mosi   = MOSI;
    dev->miso   = MISO;
    dev->cs     = CS;
    dev->freq   = freq;
    dev->conf   = config;

    /// Clear all ptr buffer!

    dev->txdPtr = NULL;
    dev->txdSize= 0;
    dev->rxdPtr = NULL;
    dev->rxdSize= 0;

    __spiExit("configSPIDevice() : %s", STR(OKE));
    return OKE;
}

/// @brief Startup (init gpio, ...) base on the config of spiDev
/// @param dev A pointer to the place which is storing the spiDev_t
/// @return Default return status
int startupSPIDevice(spiDev_t * dev){
    __spiEntry("startupSPIDevice(%p)", dev);

    if(__is_null(dev)) {
        __spiErr("dev is null!");
        __spiExit("startupSPIDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    if(__hasFlagBitClr(dev->conf, SPI_MODE)){
        /// -------- Master --------
        uint64_t gpioMask = 0;

        if(__isnot_negative(dev->clk))  gpioMask |= __mask64(dev->clk);
        if(__isnot_negative(dev->mosi)) gpioMask |= __mask64(dev->mosi);
        if(__isnot_negative(dev->cs))   gpioMask |= __mask64(dev->cs);

        gpio_config_t outPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = gpioMask,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&outPin);

        // MISO as input
        gpioMask = 0;
        if(__isnot_negative(dev->miso)) gpioMask |= __mask64(dev->miso);
        gpio_config_t inPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT,
            .pin_bit_mask = gpioMask,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&inPin);
    }else{
        /// -------- Slave --------
        uint64_t gpioMask = 0;

        if(__isnot_negative(dev->clk))  gpioMask |= __mask64(dev->clk);
        if(__isnot_negative(dev->mosi)) gpioMask |= __mask64(dev->mosi);
        if(__isnot_negative(dev->cs))   gpioMask |= __mask64(dev->cs);

        // CLK, MOSI, CS as input
        gpio_config_t inPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT,
            .pin_bit_mask = gpioMask,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&inPin);

        // MISO as output
        gpioMask = 0;
        if(__isnot_negative(dev->miso)) gpioMask |= __mask64(dev->miso);
        gpio_config_t outPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = gpioMask,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&outPin);

        // Install ISR service if not installed
        esp_err_t r = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
        if (r != ESP_OK && r != ESP_ERR_INVALID_STATE) {
            __spiErr("gpio_install_isr_service() : %d", r);
            __spiExit("startupSPIDevice() : %s", STR(ERR));
            return ERR;
        }
        
        if(__isnot_negative(dev->clk)){
            gpio_set_intr_type(dev->clk, GPIO_INTR_POSEDGE);
            gpio_isr_handler_add(dev->clk, spiHandleCLKIsr, dev);
        }
        if(__isnot_negative(dev->cs)){
            gpio_set_intr_type(dev->cs, GPIO_INTR_ANYEDGE);
            gpio_isr_handler_add(dev->cs, spiHandleCSIsr, dev);
        }
    }

    __spiExit("startupSPIDevice() : %s", STR(OKE));
    return OKE;
}

/// @brief Set SPI transmit buffer
/// @param dev A pointer to the place which is storing the spiDev_t
/// @param txdPtr The pointer to transmit buffer
/// @param size Size of the transmit buffer (in byte)
/// @return Default return status
def setTransmitBuffer(spiDev_t * dev, void * txdPtr, size_t size){
    __spiEntry("setTransmitBuffer(%p, %p, %d)", dev,txdPtr,size);

    if(__is_null(dev)){
        __spiErr("dev = %p is invalid!", dev);
        goto setTransmitBuffer_ReturnERR_NULL;
    }

    if(__is_null(txdPtr)){
        __spiErr("txdPtr = %p is invalid!", txdPtr);
        goto setTransmitBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __spiErr("size = %d is invalid!", size);
        goto setTransmitBuffer_ReturnERR_NULL;
    }

    dev->txdPtr = txdPtr;
    dev->txdSize = size;

    __spiExit("setTransmitBuffer() : %s", OKE);
    return OKE;

    setTransmitBuffer_ReturnERR_NULL:

    /// Reset txdPtr and txdSize
    dev->txdPtr = NULL;
    dev->txdSize = 0;

    __spiExit("setTransmitBuffer() : %s", OKE);
    return ERR_NULL;
}

/// @brief Set SPI receive buffer
/// @param dev A pointer to the place which is storing the spiDev_t
/// @param rxdPtr The pointer to receive buffer
/// @param size Size of the receive buffer (in byte)
/// @return Default return status
int setReceiveBuffer(spiDev_t * dev, void * rxdPtr, size_t size){
    __spiEntry("setReceiveBuffer(%p, %p, %d)", dev, rxdPtr, size);

    if(__is_null(dev)){
        __spiErr("dev = %p is invalid!", dev);
        goto setReceiveBuffer_ReturnERR_NULL;
    }

    if(__is_null(rxdPtr)){
        __spiErr("rxdPtr = %p is invalid!", rxdPtr);
        goto setReceiveBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __spiErr("size = %d is invalid!", size);
        goto setReceiveBuffer_ReturnERR_NULL;
    }

    dev->rxdPtr = rxdPtr;
    dev->rxdSize = size;

    __spiExit("setReceiveBuffer() : %s", STR(OKE));
    return OKE;

    setReceiveBuffer_ReturnERR_NULL:

    /// Reset rxdPtr and rxdSize
    dev->rxdPtr = NULL;
    dev->rxdSize = 0;

    __spiExit("setReceiveBuffer() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

/// @brief Start the spi transaction
/// @param dev A pointer to the place which is storing the spiDev_t
/// @return Default return status
/// @note Only for master mode 
def startTransaction(spiDev_t * dev){
    __spiEntry("startTransaction(%p)", dev);

    if(__is_null(dev)) {
        __spiErr("dev is null!");
        __spiExit("startupSPIDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    size_t receivedByteNumber = 0;

    if(__hasFlagBitSet(dev->conf, SPI_MODE) == SPI_MASTER){

        uint8_t currByte = 0;

        /// Get the 1st transmited byte
        currByte = ((uint8_t*)(dev->txdPtr))[0];
        shifByteLeft(dev->txdPtr, dev->txdSize, 0x0);
        
        
        if(__hasFlagBitSet(dev->conf, SPI_CPOL) == LOW){
            /// CPOL = 0
            
            /// Reset idle state as LOW
            GPIO.out_w1tc = __mask32(dev->clk);

            /// Check CPHA
            if(__hasFlagBitSet(dev->conf, SPI_CPHA) == LOW){
                /// CPOL = 0 & CPHA = 0 ---> Sample at raising edge 
                /// Prepair the data before CS is LOW!
                if(currByte & 0x80){
                    GPIO.out_w1ts = __mask32(dev->mosi);
                }else{
                    GPIO.out_w1tc = __mask32(dev->mosi);
                }
                currByte <<= 1;

                /// Clear receive buffer before start the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);
                /// Set CS low to start transmittion
                GPIO.out_w1tc = __mask32(dev->cs);

                /// Send all byte in txdPtr
                REP(k, 0, dev->txdSize){
                    /// Send 8 bit of current byte
                    REP(i, 0, 8){
                        /// Make raising edge of CLK
                        GPIO.out_w1ts = __mask32(dev->clk);
                        /// Sample on MISO
                        if(__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)){
                            shiftArrayLeft(dev->rxdPtr, dev->rxdSize, boolCast(GPIO.in & __mask32(dev->miso)));
                        }
                        
                        /// Keep high for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));
                        
                        /// Make falling edge of CLK
                        GPIO.out_w1tc = __mask32(dev->clk);
                        /// Export new bit to MOSI
                        if(i == 7){break;}
                        if(currByte & 0x80){
                            GPIO.out_w1ts = __mask32(dev->mosi);
                        }else{
                            GPIO.out_w1tc = __mask32(dev->mosi);
                        }
                        currByte <<= 1;

                        /// Keep low for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));
                    }

                    /// Prepair for next byte!
                    if(k == (dev->txdPtr-1)) {break;}
                    currByte = ((uint8_t*)(dev->txdPtr))[0];
                    shiftByteLeft(dev->txdPtr, dev->txdSize, 0x0);
                }

                /// Set CS high to stop the transmittion
                GPIO.out_w1ts = __mask32(dev->cs);
                /// Clear trasmit buffer after stop the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);

            }else{
                /// CPOL = 0 & CPHA = 1 ---> Sample at falling edge 

                /// Clear receive buffer before start the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);
                /// Set CS low to start transmittion
                GPIO.out_w1tc = __mask32(dev->cs);

                /// Send all byte in txdPtr
                REP(k, 0, dev->txdSize){

                    REP(i, 0, 8){
                        /// Make raising edge of CLK
                        GPIO.out_w1ts = __mask32(dev->clk);
                        /// Export new bit to MOSI
                        if(currByte & 0x80){
                            GPIO.out_w1ts = __mask32(dev->mosi);
                        }else{
                            GPIO.out_w1tc = __mask32(dev->mosi);
                        }
                        currByte <<= 1;

                        /// Keep HIGH for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));

                        /// Make falling edge of CLK
                        GPIO.out_w1tc = __mask32(dev->clk);
                        /// Sample on MISO
                        if(__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)){
                            shiftArrayLeft(dev->rxdPtr, dev->rxdSize, boolCast(GPIO.in & __mask32(dev->miso)));
                        }

                        /// Keep LOW for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));
                    }
                    
                    /// Prepair for next byte
                    if(k == (dev->txdPtr-1)) {break;}
                    currByte = ((uint8_t*)(dev->txdPtr))[0];
                    shiftByteLeft(dev->txdPtr, dev->txdSize, 0x0);
                }

                /// Set CS high to stop the transmittion
                GPIO.out_w1ts = __mask32(dev->cs);
                /// Clear trasmit buffer after stop the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);
            }

            /// Reset idle state of CLK as LOW
            GPIO.out_w1tc = __mask32(dev->clk);
        }else{
            /// CPOL = 1
            
            /// Reset idle state of CLK as HIGH
            GPIO.out_w1ts = __mask32(dev->clk);

            /// Check CPHA
            if(__hasFlagBitSet(dev->conf, SPI_CPHA) == LOW){
                /// CPOL = 1 & CPHA = 0 ---> Sample at falling edge 

                /// Prepair the data before CS is LOW!
                if(currByte & 0x80){
                    GPIO.out_w1ts = __mask32(dev->mosi);
                }else{
                    GPIO.out_w1tc = __mask32(dev->mosi);
                }
                currByte <<= 1;

                /// Clear receive buffer before start the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);
                /// Set CS low to start transmittion
                GPIO.out_w1tc = __mask32(dev->cs);

                /// Send all byte in txdPtr
                REP(k, 0, dev->txdSize){
                    /// Send 8 bit of current byte
                    REP(i, 0, 8){
                        /// Make falling edge of CLK
                        GPIO.out_w1tc = __mask32(dev->clk);
                        /// Sample on MISO
                        if(__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)){
                            shiftArrayLeft(dev->rxdPtr, dev->rxdSize, boolCast(GPIO.in & __mask32(dev->miso)));
                        }
                        
                        /// Keep high for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));
                        
                        /// Make raising edge of CLK
                        GPIO.out_w1ts = __mask32(dev->clk);
                        /// Export new bit to MOSI
                        if(i == 7){break;}
                        if(currByte & 0x80){
                            GPIO.out_w1ts = __mask32(dev->mosi);
                        }else{
                            GPIO.out_w1tc = __mask32(dev->mosi);
                        }
                        currByte <<= 1;

                        /// Keep low for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));
                    }

                    /// Prepair for next byte!
                    if(k == (dev->txdPtr-1)) {break;}
                    currByte = ((uint8_t*)(dev->txdPtr))[0];
                    shiftByteLeft(dev->txdPtr, dev->txdSize, 0x0);
                }

                /// Set CS high to stop the transmittion
                GPIO.out_w1ts = __mask32(dev->cs);
                /// Clear trasmit buffer after stop the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);

            }else{
                /// CPOL = 1 & CPHA = 1 ---> Sample at raising edge 

                /// Clear receive buffer before start the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);
                /// Set CS low to start transmittion
                GPIO.out_w1tc = __mask32(dev->cs);

                /// Send all byte in txdPtr
                REP(k, 0, dev->txdSize){

                    REP(i, 0, 8){
                        /// Make falling edge of CLK
                        GPIO.out_w1tc = __mask32(dev->clk);
                        /// Export new bit to MOSI
                        if(currByte & 0x80){
                            GPIO.out_w1ts = __mask32(dev->mosi);
                        }else{
                            GPIO.out_w1tc = __mask32(dev->mosi);
                        }
                        currByte <<= 1;

                        /// Keep LOW for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));

                        /// Make raising edge of CLK
                        GPIO.out_w1ts = __mask32(dev->clk);
                        /// Sample on MISO
                        if(__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)){
                            shiftArrayLeft(dev->rxdPtr, dev->rxdSize, boolCast(GPIO.in & __mask32(dev->miso)));
                        }

                        /// Keep HIGH for a half of the CLK period
                        esp_rom_delay_us(500000/(dev->freq));
                    }
                    
                    /// Prepair for next byte
                    if(k == (dev->txdPtr-1)) {break;}
                    currByte = ((uint8_t*)(dev->txdPtr))[0];
                    shiftByteLeft(dev->txdPtr, dev->txdSize, 0x0);
                }

                /// Set CS high to stop the transmittion
                GPIO.out_w1ts = __mask32(dev->cs);
                /// Clear trasmit buffer after stop the transmittion
                memset(dev->rxdPtr, 0, dev->rxdSize);

            }

            /// Reset idle state of CLK as HIGH
            GPIO.out_w1ts = __mask32(dev->clk);
        }
    }else{
        /// Slave
        __spiErr("Wrong mode or Function!");
        __spiExit("startTransaction() : %s", STR(ERR));
        return ERR;
    }

    __spiExit("startTransaction() : %d", receivedByteNumber);
    return receivedByteNumber;
}
