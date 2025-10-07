#include "spi.h"

/// HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////////

/// @brief Shift the entire byte array left by 1 bit, insert newLSB at the very end.
/// @param arr   Pointer to the byte array
/// @param size  Number of bytes in the array
/// @param newLSB The bit (0 or 1) to put in the least significant bit of the last byte
/// @return Default return status
def shiftArrayLeft(uint8_t * arr, size_t size, uint8_t newLSB) {
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
    return OKE;
}

/// @brief Shift the entire byte array right by 1 bit, insert newMSB at the very front.
/// @param arr    Pointer to the byte array
/// @param size   Number of bytes in the array
/// @param newMSB The bit (0 or 1) to put in the most significant bit of the first byte
/// @return Default return status
def shiftArrayRight(uint8_t * arr, size_t size, uint8_t newMSB) {
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
def shifByteLeft(uint8_t *arr, size_t size, uint8_t newByte) {
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
 def shiftByteRight(uint8_t *arr, size_t size, uint8_t newByte) {
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

/// SPI ///////////////////////////////////////////////////////////////////////////////////////////

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

def configSPIDevice(spiDev_t * dev, pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS,uint32_t freq, flag_t config){
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

def startupSPIDevice(spiDev_t * dev){
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
        __spiLog1("[startupSPIDevice] Config for MASTER mode");
        
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
        __spiLog1("[startupSPIDevice] Config for SLAVE mode");
        
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

        // Try to install ISR service (safe even if already installed)
        esp_err_t r = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);

        if (r == ESP_ERR_INVALID_STATE) {
            // Already installed, just log info and continue
            __spiLog1("gpio_install_isr_service(): already installed, continue...");
        } else if (r != ESP_OK) {
            // Real error -> log and abort
            __spiErr("gpio_install_isr_service() failed: %d", r);
            __spiExit("startupSPIDevice() : %s", STR(ERR));
            return ERR;
        }

        // Attach ISR handlers safely
        if (__isnot_negative(dev->clk)) {
            gpio_set_intr_type(dev->clk, GPIO_INTR_ANYEDGE);
            gpio_isr_handler_add(dev->clk, spiHandleCLKIsr, dev);
            __spiLog1("CLK ISR attached to GPIO %d", dev->clk);
        }

        if (__isnot_negative(dev->cs)) {
            gpio_set_intr_type(dev->cs, GPIO_INTR_ANYEDGE);
            gpio_isr_handler_add(dev->cs, spiHandleCSIsr, dev);
            __spiLog1("CS ISR attached to GPIO %d", dev->cs);
        }
    }

    __spiExit("startupSPIDevice() : %s", STR(OKE));
    return OKE;
}

def spiGetTransmitSize(spiDev_t *dev) {
    __spiEntry("spiGetTransmitSize(%p)", dev);

    if (__is_null(dev)) {
        __spiErr("dev = %p is invalid!", dev);
        __spiExit("spiGetTransmitSize() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    if (__is_null(dev->txdPtr)) {
        __spiErr("txdPtr is NULL, no transmit buffer assigned!");
        __spiExit("spiGetTransmitSize() : %s", STR(ERR_NULL));
        return 0;
    }

    __spiExit("spiGetTransmitSize() : %d", dev->txdSize);
    return (def)dev->txdSize;
}

def spiGetReceiveSize(spiDev_t *dev) {
    __spiEntry("spiGetReceiveSize(%p)", dev);

    if (__is_null(dev)) {
        __spiErr("dev = %p is invalid!", dev);
        __spiExit("spiGetReceiveSize() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    if (__is_null(dev->rxdPtr)) {
        __spiErr("rxdPtr is NULL, no receive buffer assigned!");
        __spiExit("spiGetReceiveSize() : %s", STR(ERR_NULL));
        return 0;
    }

    __spiExit("spiGetReceiveSize() : %d", dev->rxdSize);
    return (def)dev->rxdSize;
}

def spiResetTransmitIndex(spiDev_t * dev){
    __spiEntry("spiResetTransmitIndex(%p)", dev);
    if(__is_null(dev)){
        __spiErr("dev = %p is invalid!", dev);
        goto spiResetTransmitIndex_ReturnERR_NULL;
    }

    dev->txdBitInd = 0;
    dev->txdByteInd = 0;

    __spiExit("spiResetTransmitIndex() : %s", STR(OKE));
    return OKE;

    spiResetTransmitIndex_ReturnERR_NULL:
    __spiExit("spiResetTransmitIndex() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

def spiResetReceiveIndex(spiDev_t * dev){
    __spiEntry("spiResetReceiveIndex(%p)", dev);
    if(__is_null(dev)){
        __spiErr("dev = %p is invalid!", dev);
        goto spiResetReceiveIndex_ReturnERR_NULL;
    }

    dev->rxdBitInd = 0;
    dev->rxdByteInd = 0;

    __spiExit("spiResetReceiveIndex() : %s", STR(OKE));
    return OKE;

    spiResetReceiveIndex_ReturnERR_NULL:
    __spiExit("spiResetReceiveIndex() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

def spiSetTransmitBuffer(spiDev_t * dev, void * txdPtr, size_t size){
    __spiEntry("spiSetTransmitBuffer(%p, %p, %d)", dev,txdPtr,size);

    if(__is_null(dev)){
        __spiErr("dev = %p is invalid!", dev);
        goto spiSetTransmitBuffer_ReturnERR_NULL;
    }

    if(__is_null(txdPtr)){
        __spiErr("txdPtr = %p is invalid!", txdPtr);
        goto spiSetTransmitBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __spiErr("size = %d is invalid!", size);
        goto spiSetTransmitBuffer_ReturnERR_NULL;
    }

    dev->txdPtr = txdPtr;
    dev->txdSize = size;
    dev->txdBitInd = 0;
    dev->txdByteInd = 0;


    __spiExit("spiSetTransmitBuffer() : %s", OKE);
    return OKE;

    spiSetTransmitBuffer_ReturnERR_NULL:

    /// Reset txdPtr and txdSize
    dev->txdPtr = NULL;
    dev->txdSize = 0;
    dev->txdBitInd = 0;
    dev->txdByteInd = 0;

    __spiExit("spiSetTransmitBuffer() : %s", ERR_NULL);
    return ERR_NULL;
}

def spiSetReceiveBuffer(spiDev_t * dev, void * rxdPtr, size_t size){
    __spiEntry("spiSetReceiveBuffer(%p, %p, %d)", dev, rxdPtr, size);

    if(__is_null(dev)){
        __spiErr("dev = %p is invalid!", dev);
        goto spiSetReceiveBuffer_ReturnERR_NULL;
    }

    if(__is_null(rxdPtr)){
        __spiErr("rxdPtr = %p is invalid!", rxdPtr);
        goto spiSetReceiveBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __spiErr("size = %d is invalid!", size);
        goto spiSetReceiveBuffer_ReturnERR_NULL;
    }

    dev->rxdPtr = rxdPtr;
    dev->rxdSize = size;
    dev->rxdBitInd = 0;
    dev->rxdByteInd = 0;

    __spiExit("spiSetReceiveBuffer() : %s", STR(OKE));
    return OKE;

    spiSetReceiveBuffer_ReturnERR_NULL:

    /// Reset rxdPtr and rxdSize
    dev->rxdPtr = NULL;
    dev->rxdSize = 0;
    dev->rxdBitInd = 0;
    dev->rxdByteInd = 0;

    __spiExit("spiSetReceiveBuffer() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

/// @brief Start an SPI transaction (master mode only)
/// @param dev Pointer to SPI device configuration (spiDev_t)
/// @return Number of received bytes (>0) or error code (<=0)
def spiStartTransaction(spiDev_t * dev) {
    __spiEntry("spiStartTransaction(%p)", dev);
    
    def ret = OKE;

    /// NULL check
    if (__is_null(dev)) {
        __spiErr("dev is null!");
        __spiExit("spiStartTransaction() : %s", getDefRetStat_Str(ERR_NULL));
        ret = ERR_NULL; goto spiStartTransaction_ReturnERR1; 
    }
    /// Safety checks
    if (__isnot_positive(dev->txdSize) || __is_null(dev->txdPtr)) {
        __spiErr("tx buffer invalid (ptr=%p size=%d)", dev->txdPtr, dev->txdSize);
        ret = ERR_INVALID_ARG; goto spiStartTransaction_ReturnERR1;
    }
    /// Check mode: only master supported
    if (__hasFlagBitSet(dev->conf, SPI_MODE) != SPI_MASTER) {
        __spiErr("Wrong mode or Function!");
        ret = ERR_INVALID_ARG; goto spiStartTransaction_ReturnERR1;
    }

    uint8_t *txBuf = (uint8_t*)dev->txdPtr;
    uint8_t *rxBuf = (uint8_t*)dev->rxdPtr;
    uint8_t cpha = __hasFlagBitSet(dev->conf, SPI_CPHA);
    uint8_t cpol = __hasFlagBitSet(dev->conf, SPI_CPOL);
    uint8_t ableToReceive = __isnot_null(dev->rxdPtr) && __is_positive(dev->rxdSize);

    vPortEnterCritical(&(dev->mutex));

    if (ableToReceive) {
        /// Clear RX buffer if provided
        memset(dev->rxdPtr, 0, dev->rxdSize);
        /// Reset index counters
        dev->rxdByteInd = 0;
        dev->rxdBitInd  = 0;
    }

    /// Reset CLK back to IDLE state
    spiSetCLKState(dev, SPICLK_IDLE);
    /// Pull CS down to start transmission
    spiSetCS(dev, LOW);

    if(cpha == LOW){
        /// Main transmit loop
        while (dev->txdByteInd < dev->txdSize){
            uint8_t *currByte = &txBuf[dev->txdByteInd];
            /// Export MSB bit data 
            spiSetMOSI(dev, txBuf[dev->txdByteInd] & 0x80);
            /// Byte transmit loop
            for(uint8_t mask = 0x80; mask ; mask >>= 1){
                    /// Make 1st edge CLK
                    spiSetCLKState(dev, SPICLK_ACTIVE);
                    /// Sample at 1st edge on MISO
                    if(ableToReceive) {
                        rxBuf[dev->txdByteInd] = (rxBuf[dev->txdByteInd] << 1) | boolCast(GPIO.in & __mask32(dev->miso));
                        dev->txdBitInd ++;
    
                        if(dev->txdBitInd >= 8){
                            dev->txdBitInd = 0;
                            dev->txdByteInd ++;
                        }
                    }
                    /// Delay for a half of CLK period
                    __spiDelay(500000 / dev->freq);
                    /// Make 2nd edge CLK
                    spiSetCLKState(dev, SPICLK_IDLE);
                    /// Export new bit into MOSI
                    if(mask > 0x1 ){
                        spiSetMOSI(dev, txBuf[dev->txdByteInd] & (mask>>1));
                    }else{
                        if(dev->txdByteInd + 1 < dev->txdSize)
                            spiSetMOSI(dev, txBuf[dev->txdByteInd+1] & 0x80);
                    }
                    /// Delay for a half of CLK period
                    __spiDelay(500000 / dev->freq);
            }
        }
    }else{
        /// Main transmit loop
        while (dev->txdByteInd < dev->txdSize){
            uint8_t *currByte = &txBuf[dev->txdByteInd];
            /// Byte transmit loop
            for(uint8_t mask = 0x80; mask ; mask >>= 1){
                    /// Make 1st edge CLK
                    spiSetCLKState(dev, SPICLK_ACTIVE);
                    /// Export new bit into MOSI
                    spiSetMOSI(dev, txBuf[dev->txdByteInd] & (mask));
                    /// Delay for a half of CLK period
                    __spiDelay(500000 / dev->freq);
                    /// Make 2nd edge CLK
                    spiSetCLKState(dev, SPICLK_IDLE);
                    /// Sample at 1st edge on MISO
                    if(ableToReceive) {
                        rxBuf[dev->txdByteInd] = (rxBuf[dev->txdByteInd] << 1) | boolCast(GPIO.in & __mask32(dev->miso));
                        dev->txdBitInd ++;
    
                        if(dev->txdBitInd >= 8){
                            dev->txdBitInd = 0;
                            dev->txdByteInd ++;
                        }
                    }
                    /// Delay for a half of CLK period
                    __spiDelay(500000 / dev->freq);
            }
        }
    }

    /// Reset CLK to idle
    spiSetCLKState(dev, SPICLK_IDLE);

    /// Pull CS up to stop transmission
    if (__isnot_negative(dev->cs)) GPIO.out_w1ts = __mask32(dev->cs);

    vPortExitCritical(&(dev->mutex));
    __spiExit("spiStartTransaction() : %d", (int)dev->rxdByteInd);
    return (def) dev->rxdByteInd;
    
    // spiStartTransaction_ReturnERR0:
        // vPortExitCritical(&(dev->mutex));
    spiStartTransaction_ReturnERR1:
        __spiExit("spiStartTransaction() : %d", getDefRetStat_Str(ret));
        return ret;
}

def destroySPIDevice(spiDev_t **pDev) {
    __spiEntry("destroySPIDevice(%p)", pDev);

    if (__is_null(pDev) || __is_null(*pDev)) {
        __spiErr("pDev or *pDev is null!");
        __spiExit("destroySPIDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    spiDev_t *dev = *pDev;

    vPortEnterCritical(&(dev->mutex));

    /// --- 1. Uninstall ISR (only if in slave mode) ---
    if (__hasFlagBitSet(dev->conf, SPI_MODE) == SPI_SLAVE) {
        if (__isnot_negative(dev->clk)) {
            gpio_isr_handler_remove(dev->clk);
        }
        if (__isnot_negative(dev->cs)) {
            gpio_isr_handler_remove(dev->cs);
        }
    }

    /// --- 2. Reset all GPIO pins to default (input floating) ---
    uint64_t gpioMask = 0;

    if (__isnot_negative(dev->clk))  gpioMask |= __mask64(dev->clk);
    if (__isnot_negative(dev->mosi)) gpioMask |= __mask64(dev->mosi);
    if (__isnot_negative(dev->miso)) gpioMask |= __mask64(dev->miso);
    if (__isnot_negative(dev->cs))   gpioMask |= __mask64(dev->cs);

    if (gpioMask) {
        gpio_config_t defaultPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT,
            .pin_bit_mask = gpioMask,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&defaultPin);
    }

    /// --- 3. Clear buffers and internal pointers ---
    dev->txdPtr = NULL;
    dev->rxdPtr = NULL;
    dev->txdSize = 0;
    dev->rxdSize = 0;

    dev->txdBitInd = 0;
    dev->txdByteInd = 0;
    dev->rxdBitInd = 0;
    dev->rxdByteInd = 0;

    /// --- 4. Reset other parameters ---
    dev->freq = 0;
    dev->conf = 0;

    vPortExitCritical(&(dev->mutex));

    /// --- 5. Free memory and nullify pointer ---
    free(dev);
    *pDev = NULL;

    __spiExit("destroySPIDevice() : %s", STR(OKE));
    return OKE;
}