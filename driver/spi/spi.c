#include "spi.h"

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

    memset(*pDev, 0, sizeof(spiDev_t));

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

    dev->txdBitInd = 0;
    dev->txdByteInd = 0;
    dev->rxdBitInd = 0;
    dev->rxdByteInd = 0;

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

    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
    dev->mutex = mutex;

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
            gpio_set_intr_type(dev->clk, GPIO_INTR_ANYEDGE);
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
    dev->txdBitInd = 0;
    dev->txdByteInd = 0;


    __spiExit("setTransmitBuffer() : %s", OKE);
    return OKE;

    setTransmitBuffer_ReturnERR_NULL:

    /// Reset txdPtr and txdSize
    dev->txdPtr = NULL;
    dev->txdSize = 0;
    dev->txdBitInd = 0;
    dev->txdByteInd = 0;

    __spiExit("setTransmitBuffer() : %s", ERR_NULL);
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
    dev->rxdBitInd = 0;
    dev->rxdByteInd = 0;

    __spiExit("setReceiveBuffer() : %s", STR(OKE));
    return OKE;

    setReceiveBuffer_ReturnERR_NULL:

    /// Reset rxdPtr and rxdSize
    dev->rxdPtr = NULL;
    dev->rxdSize = 0;
    dev->rxdBitInd = 0;
    dev->rxdByteInd = 0;

    __spiExit("setReceiveBuffer() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

/// @brief Start the spi transaction (Master only)
/// @param dev Pointer to spiDev_t
/// @return <=0 nếu lỗi; >0 là số byte đã nhận
def startTransaction(spiDev_t * dev){
    __spiEntry("startTransaction(%p)", dev);

    /// Null check
    if (__is_null(dev)) {
        __spiErr("dev is null!");
        __spiExit("startTransaction() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    /// Safety checks
    if (__isnot_positive(dev->txdSize) || __is_null(dev->txdPtr)) {
        __spiErr("tx buffer invalid (ptr=%p size=%d)", dev->txdPtr, dev->txdSize);
        __spiExit("startTransaction() : %s", STR(ERR_INVALID_ARG));
        return ERR_INVALID_ARG;
    }
    if (__isnot_positive(dev->freq)) {
        __spiErr("freq = %d invalid!", dev->freq);
        __spiExit("startTransaction() : %s", STR(ERR_INVALID_ARG));
        return ERR_INVALID_ARG;
    }

    /// Check mode: only master supported
    if (__hasFlagBitSet(dev->conf, SPI_MODE) != SPI_MASTER) {
        __spiErr("Wrong mode or Function!");
        __spiExit("startTransaction() : %s", STR(ERR_INVALID_ARG));
        return ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&(dev->mutex));

    /// Clear RX buffer nếu có
    if (__isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize)) {
        memset(dev->rxdPtr, 0, dev->rxdSize);
    }

    uint8_t *txBuf = (uint8_t*)dev->txdPtr;
    uint8_t *rxBuf = (uint8_t*)dev->rxdPtr;

    /// Reset index counters
    dev->txdByteInd = 0;
    dev->txdBitInd  = 0;
    dev->rxdByteInd = 0;
    dev->rxdBitInd  = 0;

    /// Reset CLK to idle state depending on CPOL
    if (__hasFlagBitSet(dev->conf, SPI_CPOL) == LOW) {
        GPIO.out_w1tc = __mask32(dev->clk); /// idle LOW
    } else {
        GPIO.out_w1ts = __mask32(dev->clk); /// idle HIGH
    }

    /// Pull CS down to start transmission
    if (__isnot_negative(dev->cs)) GPIO.out_w1tc = __mask32(dev->cs);

    /// Main transmit loop
    while (dev->txdByteInd < dev->txdSize) {
        uint8_t *currByte = &txBuf[dev->txdByteInd];

        while (dev->txdBitInd < 8) {
            if (__hasFlagBitSet(dev->conf, SPI_CPHA) == LOW) {
                /// --- CPHA = 0: chuẩn bị data trước cạnh đầu, sample tại cạnh đầu ---

                /// Xuất MOSI
                if (*currByte & (0x80 >> dev->txdBitInd))
                    GPIO.out_w1ts = __mask32(dev->mosi);
                else
                    GPIO.out_w1tc = __mask32(dev->mosi);

                /// Cạnh đầu (sample MISO)
                if (__hasFlagBitSet(dev->conf, SPI_CPOL) == LOW)
                    GPIO.out_w1ts = __mask32(dev->clk);
                else
                    GPIO.out_w1tc = __mask32(dev->clk);

                /// Lấy mẫu từ MISO
                if (rxBuf && dev->rxdByteInd < dev->rxdSize) {
                    rxBuf[dev->rxdByteInd] <<= 1;
                    rxBuf[dev->rxdByteInd] |= boolCast(GPIO.in & __mask32(dev->miso));
                    dev->rxdBitInd++;
                    if (dev->rxdBitInd == 8) {
                        dev->rxdBitInd = 0;
                        dev->rxdByteInd++;
                    }
                }

                esp_rom_delay_us(500000 / dev->freq);

                /// Cạnh thứ hai (trả về idle)
                if (__hasFlagBitSet(dev->conf, SPI_CPOL) == LOW)
                    GPIO.out_w1tc = __mask32(dev->clk);
                else
                    GPIO.out_w1ts = __mask32(dev->clk);

                esp_rom_delay_us(500000 / dev->freq);

            } else {
                /// --- CPHA = 1: sample tại cạnh thứ 2 ---

                /// Cạnh đầu
                if (__hasFlagBitSet(dev->conf, SPI_CPOL) == LOW)
                    GPIO.out_w1ts = __mask32(dev->clk);
                else
                    GPIO.out_w1tc = __mask32(dev->clk);

                /// Xuất MOSI
                if (*currByte & (0x80 >> dev->txdBitInd))
                    GPIO.out_w1ts = __mask32(dev->mosi);
                else
                    GPIO.out_w1tc = __mask32(dev->mosi);

                esp_rom_delay_us(500000 / dev->freq);

                /// Cạnh thứ hai (sample)
                if (__hasFlagBitSet(dev->conf, SPI_CPOL) == LOW)
                    GPIO.out_w1tc = __mask32(dev->clk);
                else
                    GPIO.out_w1ts = __mask32(dev->clk);

                if (rxBuf && dev->rxdByteInd < dev->rxdSize) {
                    rxBuf[dev->rxdByteInd] <<= 1;
                    rxBuf[dev->rxdByteInd] |= boolCast(GPIO.in & __mask32(dev->miso));
                    dev->rxdBitInd++;
                    if (dev->rxdBitInd == 8) {
                        dev->rxdBitInd = 0;
                        dev->rxdByteInd++;
                    }
                }

                esp_rom_delay_us(500000 / dev->freq);
            }

            dev->txdBitInd++;
        }

        /// Chuyển sang byte tiếp theo
        dev->txdBitInd = 0;
        dev->txdByteInd++;
    }

    /// Reset CLK về idle
    if (__hasFlagBitSet(dev->conf, SPI_CPOL) == LOW)
        GPIO.out_w1tc = __mask32(dev->clk);
    else
        GPIO.out_w1ts = __mask32(dev->clk);

    /// CS lên để kết thúc truyền
    if (__isnot_negative(dev->cs)) GPIO.out_w1ts = __mask32(dev->cs);

    portEXIT_CRITICAL(&(dev->mutex));

    __spiExit("startTransaction() : %d", (int)dev->rxdByteInd);
    return (def) dev->rxdByteInd;
}
