/// @file   spi.c
/// @brief  SPI Driver Implementation; Manages software SPI (bit-banging) for Master mode and ISR-based Slave mode.

#include "spi.h"

/// @cond
/// HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////////
/// @endcond

#define UINT8_ARRAY(pv, index) (((uint8_t*)(pv))[index])

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

/// @brief Reset the transmit buffer indices and flags.
/// @param dev Pointer to the SPI device structure.
static inline __attribute__((always_inline))
void spiResetTBuff(spiDev_t *dev){
    dev->tBuffIndex = 0;
    dev->tBuffMask  = 0x80;
    __setFlagBit(dev->stat, SPISTAT_TBUFF_FULL);
    __clearFlagBit(dev->stat, SPISTAT_TBUFF_EMPTY);
}

/// @brief Reset the receive buffer indices and flags.
/// @param dev Pointer to the SPI device structure.
static inline __attribute__((always_inline))
void spiResetRBuff(spiDev_t *dev){
    dev->rBuffIndex = 0;
    dev->rBuffMask  = 0x80;
    // memset(dev->rBuff, 0, dev->rBuffSize);
    __setFlagBit(dev->stat, SPISTAT_RBUFF_EMPTY);
    __clearFlagBit(dev->stat, SPISTAT_RBUFF_FULL);
}

/// @brief Get the next bit to transmit from the buffer.
/// @param dev    Pointer to the SPI device structure.
/// @param outBit Pointer to store the retrieved bit.
/// @return Status OKE on success, ERR_UNDERFLOW if buffer is empty.
static inline __attribute__((always_inline))
def spiTransmitBuffGetNextBit(spiDev_t * dev, uint8_t * outBit){
    def ret = OKE;    
    /// NULL check
    if(__is_null(dev->tBuff)){
        ret = ERR_NULL; goto returnERR;
    }
    /// Size check
    if(__isnot_positive(dev->tBuffSize)){
        ret = ERR_INVALID_ARG; goto returnERR;
    }
    /// Check empty
    if(__hasFlagBitSet(dev->stat, SPISTAT_TBUFF_EMPTY)) {
        (*outBit) = 0;
        ret = ERR_UNDERFLOW; goto returnERR;
    }else{
        __clearFlagBit(dev->stat, SPISTAT_TBUFF_FULL);
    }
    /// Get transmit bit, then shift right tBuffMask
    (*outBit) = boolCast(UINT8_ARRAY(dev->tBuff, dev->tBuffIndex) & (dev->tBuffMask));
    /// Move to next bit
    if ( (dev->tBuffMask) == 0x01 ){
        if( dev->tBuffIndex+1 < dev->tBuffSize){
            ++ (dev->tBuffIndex) ;
        }else{
            dev->tBuffIndex = 0;
            __setFlagBit(dev->stat, SPISTAT_TBUFF_EMPTY);
        }
        (dev->tBuffMask) = 0x80;
    }else{
        (dev->tBuffMask) >>= 1;
    }
    return OKE;
    returnERR:
        // __spiErr("[spiTransmitBuffGetNextBit] %s", getDefRetStat_Str(ret));
        return ret;
}

/// @brief Push a received bit into the receive buffer.
/// @param dev   Pointer to the SPI device structure.
/// @param inBit The bit value to push (0 or 1).
/// @return Status OKE on success, ERR_UNDERFLOW (used as Overflow here) if buffer is full.
static inline __attribute__((always_inline))
def spiReceiveBuffPushBit(spiDev_t * dev, uint8_t inBit){
    def ret = OKE;    
    /// NULL check
    if(__is_null(dev->rBuff)){
        ret = ERR_NULL; goto returnERR;
    }
    /// Size check
    if(__isnot_positive(dev->rBuffSize)){
        ret = ERR_INVALID_ARG; goto returnERR;
    }
    /// Check full
    if(__hasFlagBitSet(dev->stat, SPISTAT_RBUFF_FULL)) {
        ret = ERR_UNDERFLOW; goto returnERR;
    }else{
        __clearFlagBit(dev->stat, SPISTAT_RBUFF_EMPTY);
    }
    /// Push received bit
    if(inBit){
        UINT8_ARRAY(dev->rBuff, dev->rBuffIndex) |= dev->rBuffMask;
    }else{
        UINT8_ARRAY(dev->rBuff, dev->rBuffIndex) &= (~(dev->rBuffMask));
    }
    if( (dev->rBuffMask) == 0x1){
        if( dev->rBuffIndex+1 < dev->rBuffSize){
            ++ (dev->rBuffIndex) ;
        }else{
            dev->rBuffIndex = 0;
            __setFlagBit(dev->stat, SPISTAT_RBUFF_FULL);
        }
        (dev->rBuffMask) = 0x80;
    }else{
        (dev->rBuffMask) >>= 1;
    }
    return OKE;
    returnERR:
        // __spiErr("[spiReceiveBuffPushBit] %s", getDefRetStat_Str(ret));
        return ret;
}

/// @cond
/// SLAVE INTERRUPT ///////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief ISR handler for SPI Clock edge (Slave mode).
/// @param pv Pointer to the SPI device structure (passed as void*).
static 
void IRAM_ATTR spiHandleCLKIsr(void* pv) {
    spiDev_t *dev = (spiDev_t*) pv;

    if (__is_positive(GPIO.in & __mask32(dev->cs))) return;
    
    uint8_t clkLevel = __is_positive(GPIO.in & __mask32(dev->clk));

    uint8_t isRising = (dev->__cpol) ? !clkLevel : clkLevel;
    uint8_t isSampleEdge = ((dev->__cpha) == 0) ? isRising : !isRising;
    uint8_t isShiftEdge  = !isSampleEdge;
    uint8_t outBit       = 0;

    if (isSampleEdge) {
        // __spiLog1(">> %d", clkLevel);
        // Sample MOSI
        spiReceiveBuffPushBit(dev, __is_positive(GPIO.in & __mask32(dev->mosi)));
    } else {
        // Export MISO
        spiTransmitBuffGetNextBit(dev, &outBit);
        spiSetMISO_SlaveMode(dev, outBit);
        // __spiLog1("<< %d", outBit);
    }
}

/// @brief ISR handler for SPI Chip Select edge (Slave mode).
/// @param pv Pointer to the SPI device structure (passed as void*).
static 
void IRAM_ATTR spiHandleCSIsr(void* pv) {
    spiDev_t *dev = (spiDev_t*) pv;

    if (__is_positive(GPIO.in & __mask32(dev->cs))) {
        __spiLog1("CS rising edge");
        /// CS rising edge — end of transaction
        // gpio_intr_disable(dev->clk);
        
    } else {
        __spiLog1("CS falling edge");
        /// CS falling edge — start of transaction
        // gpio_intr_enable(dev->clk);


        /// For CPHA = 0, export the first MSB bit immediately (before 1st clock edge)
        if (__hasFlagBitClr(dev->conf, SPI_CPHA)) {
            if (__isnot_null(dev->tBuff) && __is_positive(dev->tBuffSize)) {
                if (UINT8_ARRAY(dev->tBuff, dev->tBuffIndex) & (dev->tBuffMask)) {
                    GPIO.out_w1ts = __mask32(dev->miso);
                } else {
                    GPIO.out_w1tc = __mask32(dev->miso);
                }
            }
        }
    }
}

/// @cond
/// SPI ///////////////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Allocate memory for a new SPI device structure.
/// @param pDev Double pointer to store the allocated device pointer.
/// @return OKE on success, ERR_NULL or ERR_MALLOC_FAILED on failure.
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

/// @brief Configure the SPI device parameters (pins, frequency, mode).
/// @param dev    Pointer to the SPI device structure.
/// @param CLK    GPIO pin for Clock.
/// @param MOSI   GPIO pin for MOSI.
/// @param MISO   GPIO pin for MISO.
/// @param CS     GPIO pin for Chip Select.
/// @param freq   SPI frequency in Hz.
/// @param config SPI configuration flags (CPOL, CPHA, Master/Slave).
/// @return OKE on success, ERR_NULL if dev is invalid.
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

    dev->tBuff = NULL;
    dev->tBuffSize = 0;
    spiResetTBuff(dev);

    dev->rBuff = NULL;
    dev->rBuffSize = 0;
    spiResetRBuff(dev);

    dev->stat = __masks32(0, 1, 2, 3);

    dev->__cpha = __hasFlagBitSet(dev->conf, SPI_CPHA);
    dev->__cpol = __hasFlagBitSet(dev->conf, SPI_CPOL);
    dev->__master_slave = __hasFlagBitSet(dev->conf, SPI_MODE);

    __spiExit("configSPIDevice() : %s", STR(OKE));
    return OKE;
}

/// @brief Initialize GPIOs and interrupts for the SPI device based on configuration.
/// @param dev Pointer to the SPI device structure.
/// @return OKE on success, ERR_NULL or ERR on failure.
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

        // if (__isnot_negative(dev->cs)) {
        //     gpio_set_intr_type(dev->cs, GPIO_INTR_ANYEDGE);
        //     gpio_isr_handler_add(dev->cs, spiHandleCSIsr, dev);
        //     __spiLog1("CS ISR attached to GPIO %d", dev->cs);
        // }
    }

    __spiExit("startupSPIDevice() : %s", STR(OKE));
    return OKE;
}

/// @brief Set the data buffer for transmission.
/// @param dev   Pointer to the SPI device structure.
/// @param tBuff Pointer to the data buffer to transmit.
/// @param size  Size of the buffer in bytes.
/// @return OKE on success, ERR_NULL if arguments are invalid.
def spiSetTransmitBuffer(spiDev_t * dev, void * tBuff, size_t size){
    __spiEntry("spiSetTransmitBuffer(%p, %p, %d)", dev,tBuff,size);

    if(__is_null(dev)){
        __spiErr("dev = %p is invalid!", dev);
        goto spiSetTransmitBuffer_ReturnERR_NULL;
    }

    if(__is_null(tBuff)){
        __spiErr("tBuff = %p is invalid!", tBuff);
        goto spiSetTransmitBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __spiErr("size = %d is invalid!", size);
        goto spiSetTransmitBuffer_ReturnERR_NULL;
    }

    dev->tBuff      = tBuff;
    dev->tBuffSize  = size;

    spiResetTBuff(dev);

    if(__hasFlagBitSet(dev->conf, SPI_CPHA) == LOW){
        if(__hasFlagBitSet(dev->conf, SPI_MODE) == SPI_MASTER)
            spiSetMOSI(dev, UINT8_ARRAY(dev->tBuff, dev->tBuffIndex) & 0x80);
        else
            spiSetMISO_SlaveMode(dev, UINT8_ARRAY(dev->tBuff, dev->tBuffIndex) & 0x80);
    }

    __spiExit("spiSetTransmitBuffer() : %s", STR(OKE));
    return OKE;

    spiSetTransmitBuffer_ReturnERR_NULL:
    __spiExit("spiSetTransmitBuffer() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

/// @brief Set the buffer to store received data.
/// @param dev   Pointer to the SPI device structure.
/// @param rBuff Pointer to the reception buffer.
/// @param size  Size of the buffer in bytes.
/// @return OKE on success, ERR_NULL if arguments are invalid.
def spiSetReceiveBuffer(spiDev_t * dev, void * rBuff, size_t size){
    __spiEntry("spiSetReceiveBuffer(%p, %p, %d)", dev, rBuff, size);

    if(__is_null(dev)){
        __spiErr("[spiSetReceiveBuffer] dev = %p is invalid!", dev);
        goto spiSetReceiveBuffer_ReturnERR_NULL;
    }

    if(__is_null(rBuff)){
        __spiErr("[spiSetReceiveBuffer] rBuff = %p is invalid!", rBuff);
        goto spiSetReceiveBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __spiErr("[spiSetReceiveBuffer] size = %d is invalid!", size);
        goto spiSetReceiveBuffer_ReturnERR_NULL;
    }

    dev->rBuff      = rBuff;
    dev->rBuffSize  = size;

    spiResetRBuff(dev);

    __spiExit("spiSetReceiveBuffer() : %s", STR(OKE));
    return OKE;

    spiSetReceiveBuffer_ReturnERR_NULL:
    __spiExit("spiSetReceiveBuffer() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

/// @brief Perform a SPI transaction (Master mode bit-banging).
/// @param dev Pointer to the SPI device structure.
/// @return OKE on success, ERR_NULL or ERR_INVALID_ARG on failure.
def spiStartTransaction(spiDev_t * dev) {
    __spiEntry("spiStartTransaction(%p)", dev);
    
    def ret = OKE;

    /// NULL check
    if (__is_null(dev)) {
        __spiErr("[spiStartTransaction] dev is null!");
        ret = ERR_NULL; goto spiStartTransaction_ReturnERR1; 
    }
    /// Safety checks
    if (__isnot_positive(dev->tBuffSize) || __is_null(dev->tBuff)) {
        __spiErr("[spiStartTransaction] tx buffer invalid (ptr=%p size=%d)", dev->tBuff, dev->tBuffSize);
        ret = ERR_INVALID_ARG; goto spiStartTransaction_ReturnERR1;
    }
    /// Check mode: only master supported
    if (__hasFlagBitSet(dev->conf, SPI_MODE) != SPI_MASTER) {
        __spiErr("[spiStartTransaction] Wrong mode or Function!");
        ret = ERR_INVALID_ARG; goto spiStartTransaction_ReturnERR1;
    }

    uint8_t ableToReceive = __isnot_null(dev->rBuff) && 
                            __is_positive(dev->rBuffSize) &&
                            __hasFlagBitClr(dev->stat, SPISTAT_RBUFF_FULL);
    uint8_t outBit = 0;

    vPortEnterCritical(&(dev->mutex));

    if (ableToReceive) spiResetRBuff(dev);

    /// Reset CLK back to IDLE state
    spiSetCLKState(dev, SPICLK_IDLE);
    /// Pull CS down to start transmission
    spiSetCS(dev, LOW);

    if(__hasFlagBitSet(dev->conf, SPI_CPHA) == LOW){
        /// Export 1st bit before the fist edge of CLK
        ret = spiTransmitBuffGetNextBit(dev, &outBit);
        spiSetMOSI(dev, outBit);
        /// Delay for a half of CLK period
        __spiDelay(500000/(dev->freq));
        /// Send n bit in buffer
        if (ret >= 0)
            while(0x1){
                /// Get next bit
                ret = spiTransmitBuffGetNextBit(dev, &outBit);
                /// Make 1st edge
                spiSetCLKState(dev, SPICLK_ACTIVE);
                /// Sample MISO
                if (ableToReceive) 
                    spiReceiveBuffPushBit(dev, GPIO.in & __mask32(dev->miso));
                /// Delay for a half of CLK period
                __spiDelay(500000/(dev->freq));
                /// Make 2nd edge
                spiSetCLKState(dev, SPICLK_IDLE);
                /// Export new bit to MOSI
                spiSetMOSI(dev, outBit);
                /// Delay for a half of CLK period
                __spiDelay(500000/(dev->freq));
                /// Break if there is nothing to send
                if (ret < 0) break;
            }
    
    }else{
        while(0x1){
            /// Get next bit
            ret = spiTransmitBuffGetNextBit(dev, &outBit);
            /// Break if there is nothing to send
            if (ret < 0) break;
            /// Make 1st edge
            spiSetCLKState(dev, SPICLK_ACTIVE);
            /// Export new bit to MOSI
            spiSetMOSI(dev, outBit);
            /// Delay for a half of CLK period
            __spiDelay(500000/(dev->freq));
            /// Make 2nd edge
            spiSetCLKState(dev, SPICLK_IDLE);
            /// Sample MISO
            if (ableToReceive) 
                spiReceiveBuffPushBit(dev, GPIO.in & __mask32(dev->miso));
            /// Delay for a half of CLK period
            __spiDelay(500000/(dev->freq));
        }
    }

    /// Reset CLK to idle
    spiSetCLKState(dev, SPICLK_IDLE);

    /// Pull CS up to stop transmission
    spiSetCS(dev, HIGH);

    vPortExitCritical(&(dev->mutex));
    __spiExit("spiStartTransaction() : OKE");
    return OKE;
    
    // spiStartTransaction_ReturnERR0:
        // vPortExitCritical(&(dev->mutex));
    spiStartTransaction_ReturnERR1:
        __spiExit("spiStartTransaction() : %s", getDefRetStat_Str(ret));
        return ret;
}

/// @brief Clean up and free the SPI device.
/// @param pDev Double pointer to the SPI device structure to free.
/// @return OKE on success, ERR_NULL if pointers are invalid.
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
    dev->tBuff = NULL;
    dev->rBuff = NULL;
    dev->tBuffSize = 0;
    dev->rBuffSize = 0;

    dev->tBuffMask = 0;
    dev->rBuffIndex = 0;
    dev->rBuffMask = 0;
    dev->rBuffIndex = 0;

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