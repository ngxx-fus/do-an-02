#include "i2c.h"

def createI2CDevice(i2cDev_t ** pDev){
    __i2cEntry("createI2CDevice(i2cDev_t ** pDev)");

    if(__is_null(pDev)) {
        __i2cErr("pDev is null!");
        __i2cExit("createI2CDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    (*pDev) = NULL;
    (*pDev) = (i2cDev_t*) malloc(sizeof(i2cDev_t));
    
    if(__is_null(*pDev)){
        __i2cErr("Cannot allocate new i2cDev_t!");
        __i2cExit("createI2CDevice() : %s", STR(ERR_MALLOC_FAILED));
        return ERR_MALLOC_FAILED;
    }

    memset(*pDev, 0, sizeof(i2cDev_t));

    __i2cExit("createI2CDevice() : %s", STR(OKE));
    return OKE;
}

def configI2CDevice(i2cDev_t * dev, uint8_t addr, pin_t scl, pin_t sda, flag_t config){
    __i2cEntry("configI2CDevice(dev: %p, SCL: %d, SDA: %d, config: 0x%04x)", dev, scl, sda, config);

    if(__is_null(dev)) {
        __i2cErr("dev is null!");
        __i2cExit("configI2CDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    /// Set new proterties

    dev->addr   = addr;
    dev->scl    = scl;
    dev->sda    = sda;
    dev->conf   = config;
    dev->freq   =   ((config & 0x3) == 0) ? (100000) : 
                    ((config & 0x3) == 1) ? (400000) : 
                    ((config & 0x3) == 2) ? (800000) : (1000000);

    /// Clear all ptr buffer!

    dev->txPtr = NULL;
    dev->txSize= 0;
    dev->txIndByte = 0;
    dev->txIndBit = 0;
    
    dev->rxPtr = NULL;
    dev->rxSize= 0;
    dev->rxIndBit = 0;
    dev->rxIndByte = 0;

    __i2cExit("configI2CDevice() : %s", STR(OKE));
    return OKE;
}

int startupI2CDevice(i2cDev_t * dev){
    __i2cEntry("startupI2CDevice(%p)", dev);

    if(__is_null(dev)) {
        __i2cErr("dev is null!");
        __i2cExit("startupI2CDevice() : %s", STR(ERR_NULL));
        return ERR_NULL;
    }

    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
    dev->mutex = mutex;

    if(__hasFlagBitClr(dev->conf, I2C_MODE) == I2C_MASTER){
        /// MASTER MODE ///

        gpio_config_t sdaPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT_OUTPUT_OD,
            .pin_bit_mask = dev->sda,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_ENABLE,
        };
        gpio_config(&sdaPin);

        gpio_config_t sclPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = dev->scl,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&sclPin);
    }else{
        /// SLAVE MODE ///

        gpio_config_t sdaPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT_OUTPUT_OD,
            .pin_bit_mask = dev->sda,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLDOWN_DISABLE,
        };
        gpio_config(&sdaPin);

        gpio_config_t sclPin = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT,
            .pin_bit_mask = dev->scl,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&sclPin);

        // Install ISR service if not installed
        esp_err_t r = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
        if (r != ESP_OK && r != ESP_ERR_INVALID_STATE) {
            __i2cErr("gpio_install_isr_service() : %d", r);
            __i2cExit("startupI2CDevice() : %s", STR(ERR));
            return ERR;
        }
        
        if(__isnot_negative(dev->sda)){
            gpio_set_intr_type(dev->sda, GPIO_INTR_ANYEDGE);
            gpio_isr_handler_add(dev->sda, i2cHandleSDAIsr, dev);
        }
        if(__isnot_negative(dev->scl)){
            gpio_set_intr_type(dev->scl, GPIO_INTR_ANYEDGE);
            gpio_isr_handler_add(dev->scl, i2cHandleSCLIsr, dev);
        }
    }

    __i2cExit("startupI2CDevice() : %s", STR(OKE));
    return OKE;
}

def i2cSetTransmitBuffer(i2cDev_t * dev, void * txPtr, size_t size){
    __i2cEntry("setTransmitBuffer(%p, %p, %d)", dev,txPtr,size);

    if(__is_null(dev)){
        __i2cErr("dev = %p is invalid!", dev);
        goto setTransmitBuffer_ReturnERR_NULL;
    }

    if(__is_null(txPtr)){
        __i2cErr("txPtr = %p is invalid!", txPtr);
        goto setTransmitBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __i2cErr("size = %d is invalid!", size);
        goto setTransmitBuffer_ReturnERR_NULL;
    }

    dev->txPtr = txPtr;
    dev->txSize = size;
    dev->txIndBit = 0;
    dev->txIndByte = 0;


    __i2cExit("setTransmitBuffer() : %s", OKE);
    return OKE;

    setTransmitBuffer_ReturnERR_NULL:

    /// Reset txPtr and txSize
    dev->txPtr = NULL;
    dev->txSize = 0;
    dev->txIndBit = 0;
    dev->txIndByte = 0;

    __i2cExit("setTransmitBuffer() : %s", ERR_NULL);
    return ERR_NULL;
}

def i2cSetReceiveBuffer(i2cDev_t * dev, void * rxPtr, size_t size){
    __i2cEntry("setReceiveBuffer(%p, %p, %d)", dev, rxPtr, size);

    if(__is_null(dev)){
        __i2cErr("dev = %p is invalid!", dev);
        goto setReceiveBuffer_ReturnERR_NULL;
    }

    if(__is_null(rxPtr)){
        __i2cErr("rxPtr = %p is invalid!", rxPtr);
        goto setReceiveBuffer_ReturnERR_NULL;
    }

    if(__isnot_positive(size)){
        __i2cErr("size = %d is invalid!", size);
        goto setReceiveBuffer_ReturnERR_NULL;
    }

    dev->rxPtr = rxPtr;
    dev->rxSize = size;
    dev->rxIndBit = 0;
    dev->rxIndByte = 0;

    __i2cExit("setReceiveBuffer() : %s", STR(OKE));
    return OKE;

    setReceiveBuffer_ReturnERR_NULL:

    /// Reset rxPtr and rxSize
    dev->rxPtr = NULL;
    dev->rxSize = 0;
    dev->rxIndBit = 0;
    dev->rxIndByte = 0;

    __i2cExit("setReceiveBuffer() : %s", STR(ERR_NULL));
    return ERR_NULL;
}

def i2cSendByte(i2cDev_t * dev, uint8_t addr7, uint8_t rw, uint8_t byte, flag_t extConf){
    __I2C_ASSERT_DEV_OK1(STR(i2cSendByte), dev, STR(dev));

    if(extConf & __mask32(I2C_EN_ENTRY_CRITICAL_SEC)) vPortEnterCritical(&(dev->mutex)); 

    /// Make start condition 
    if(extConf & __mask32(I2C_EN_START_CONDI)) i2cStartCondition(dev);
    /// Send address frame
    i2cSendAddressFrame(dev, addr7, rw);
    /// Get and process returned bit
    if(i2cGetReturnBit(dev) == I2C_NACK) {
        __i2cErr("[i2cSendByte] Got NACK after send address frame!");
        goto i2cSendByte_ReturnERR;
    }
    /// Send byte high
    i2cSendDataFrame(dev, byte);
    /// Get and process returned bit
    if(i2cGetReturnBit(dev) == I2C_NACK) {
        __i2cErr("[i2cSendByte] Got NACK after send byte high!");
        goto i2cSendByte_ReturnERR;
    }

    if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
    if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
    return OKE;

    i2cSendByte_ReturnERR:
        if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
        if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
        return ERR;
}

def i2cSendDoubleByte(i2cDev_t * dev, uint8_t addr7, uint8_t rw, uint8_t byteHigh, uint8_t byteLow, flag_t extConf){
    __I2C_ASSERT_DEV_OK1(STR(i2cSendDoubleByte), dev, STR(dev));

    if(extConf & __mask32(I2C_EN_ENTRY_CRITICAL_SEC)) vPortEnterCritical(&(dev->mutex)); 

    /// Make start condition 
    if(extConf & __mask32(I2C_EN_START_CONDI)) i2cStartCondition(dev);
    /// Send address frame
    i2cSendAddressFrame(dev, addr7, rw);
    /// Get and process returned bit
    if(i2cGetReturnBit(dev) == I2C_NACK) {
        __i2cErr("[i2cSendDoubleByte] Got NACK after send address frame!");
        goto i2cSendDoubleByte_ReturnERR;
    }
    /// Send byte high
    i2cSendDataFrame(dev, byteHigh);
    /// Get and process returned bit
    if(i2cGetReturnBit(dev) == I2C_NACK) {
        __i2cErr("[i2cSendDoubleByte] Got NACK after send byte high!");
        goto i2cSendDoubleByte_ReturnERR;
    }
    /// Send byte low
    i2cSendDataFrame(dev, byteLow);
    /// Get and process returned bit
    if(i2cGetReturnBit(dev) == I2C_NACK) {
        __i2cErr("[i2cSendDoubleByte] Got NACK after send byte low!");
        goto i2cSendDoubleByte_ReturnERR;
    }

    if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
    if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
    return OKE;

    i2cSendDoubleByte_ReturnERR:
        if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
        if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
        return ERR;
}

def i2cSendBuffer(i2cDev_t * dev, uint8_t addr7, uint8_t rw, flag_t extConf){
    __I2C_ASSERT_DEV_OK1(STR(i2cSendBuffer), dev, STR(dev));

    if(extConf & __mask32(I2C_EN_ENTRY_CRITICAL_SEC)) vPortEnterCritical(&(dev->mutex)); 

    /// Make start condition 
    if(extConf & __mask32(I2C_EN_START_CONDI)) i2cStartCondition(dev);
    /// Send address frame
    i2cSendAddressFrame(dev, addr7, rw);
    /// Get and process returned bit
    if(i2cGetReturnBit(dev) == I2C_NACK) {
        __i2cErr("[i2cSendDoubleByte] Got NACK after send address frame!");
        goto i2cSendBuffer_ReturnERR;
    }

    REP(i, dev->txIndByte, dev->txSize){
        /// Send byte[i]
        i2cSendDataFrame(dev, ((char*)(dev->txPtr))[i]);
        /// Get and process returned bit
        if(i2cGetReturnBit(dev) == I2C_NACK) {
            __i2cErr("[i2cSendDoubleByte] Got NACK after send byte[%d] in buffer!", i);
            goto i2cSendBuffer_ReturnERR;
        }
    }

    if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
    if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
    return OKE;

    i2cSendBuffer_ReturnERR:
        if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
        if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
        return ERR;
}

def i2cReceiveBuffer(i2cDev_t * dev, uint8_t addr7, uint8_t rw, flag_t extConf){
    __I2C_ASSERT_DEV_OK1(STR(i2cSendBuffer), dev, STR(dev));

    if(extConf & __mask32(I2C_EN_ENTRY_CRITICAL_SEC)) vPortEnterCritical(&(dev->mutex)); 
    /// Make start condition 
    if(extConf & __mask32(I2C_EN_START_CONDI)) i2cStartCondition(dev);
    /// Send address frame
    i2cSendAddressFrame(dev, addr7, rw);
    /// Get and process returned bit
    if(i2cGetReturnBit(dev) == I2C_NACK) {
        __i2cErr("[i2cSendDoubleByte] Got NACK after send address frame!");
        goto i2cReceiveBuffer_ReturnERR;
    }

    def receivedByte = 0;

    REP(i, dev->txIndByte, dev->txSize){
        receivedByte = i2cReceiveByte(dev);
        if(receivedByte < 0){
            __err("[i2cReceiveBuffer] The i2cReceiveByte has returned [%d]", receivedByte);
            __i2cSetSDA(dev, I2C_NACK);
            /// Mono SCL pulse (9th pulse)
            __i2cDelay(500000/__i2cFreq(dev));
            __i2cSetSCL(dev, HIGH);
            __i2cDelay(500000/__i2cFreq(dev));
            __i2cSetSCL(dev, LOW);
            goto i2cReceiveBuffer_ReturnERR;
        }else{
            ((uint8_t*)(dev->txPtr))[(dev->rxIndByte)++] = receivedByte;
            __i2cSetSDA(dev, I2C_ACK);
            /// Mono SCL pulse (9th pulse)
            __i2cDelay(500000/__i2cFreq(dev));
            __i2cSetSCL(dev, HIGH);
            __i2cDelay(500000/__i2cFreq(dev));
            __i2cSetSCL(dev, LOW);
        }
    }

    if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
    if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
    return OKE;

    i2cReceiveBuffer_ReturnERR:
        if(extConf & __mask32(I2C_EN_STOP_CONDI)) i2cStopCondition(dev);
        if(extConf & __mask32(I2C_EN_EXIT_CRITICAL_SEC)) vPortExitCritical(&(dev->mutex));
        return ERR;
}