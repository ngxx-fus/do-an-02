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

def setTransmitBuffer(i2cDev_t * dev, void * txPtr, size_t size){
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

def setReceiveBuffer(i2cDev_t * dev, void * rxPtr, size_t size){
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



// void i2c_config_headers(I2CCommunication* i2c){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0)
//         i2ccom_log("[>>>] i2c_config_headers(%p)", i2c);
//     #endif

//     gpio_config_t sda_header_config = {
//         .intr_type      = GPIO_INTR_DISABLE,
//         .mode           = GPIO_MODE_OUTPUT_OD,
//         .pin_bit_mask   = (1ULL << __SDA(i2c)),
//         .pull_down_en   = GPIO_PULLDOWN_DISABLE,
//         .pull_up_en     = GPIO_PULLUP_ENABLE
//     };
//     gpio_config(&sda_header_config);

//     gpio_config_t scl_header_config = {
//         .intr_type      = GPIO_INTR_DISABLE,
//         .mode           = GPIO_MODE_OUTPUT_OD,
//         .pin_bit_mask   = (1ULL << __SCL(i2c)),
//         .pull_down_en   = GPIO_PULLDOWN_DISABLE,
//         .pull_up_en     = GPIO_PULLUP_ENABLE
//     };
//     gpio_config(&scl_header_config);
//     i2cSetSCL(i2c, 1);
//     i2cSetSDA(i2c, 1);
//     i2c_com_delay(__t2);
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0)
//         i2ccom_log("[<<<] i2c_config_headers()");
//     #endif
// }

// I2CCommunication* createNewI2CCommunication(uint8_t SDA, uint8_t SCL) {
//     I2CCommunication* i2c = malloc(sizeof(I2CCommunication));
//     if (!i2c) return NULL;
    
//     i2c->sda = SDA;
//     i2c->scl = SCL;
//     portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
//     i2c->mutex = mutex;
//     i2c_config_headers(i2c);
//     return i2c;
// }

// void deleteI2CCommunication(I2CCommunication* i2cComAddr){
//     if(i2cComAddr) free(i2cComAddr);
// }

// uint8_t i2c_send_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
//         i2ccom_log("[>>>] i2c_send_byte()");
//     #endif
//     taskENTER_CRITICAL(&__MUTEX(i2c));
//     uint8_t res;
//     i2c_start_condition(i2c);
//     i2c_send_address_frame(i2c, address_7bit, 0);
//     if( i2c_get_response(i2c) ){
//         i2c_stop_condition(i2c);
//         taskEXIT_CRITICAL(&__MUTEX(i2c));
//         return -1;
//     }
//     i2c_send_data_frame(i2c, data);
//     res = i2c_get_response(i2c);
//     i2c_stop_condition(i2c);
//     taskEXIT_CRITICAL(&__MUTEX(i2c));
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
//         i2ccom_log("[<<<] i2c_send_byte()");
//     #endif
//     return res;
// }

// uint8_t i2c_send_word(I2CCommunication* i2c, uint8_t address_7bit, uint8_t byte_h, uint8_t byte_l){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
//         i2ccom_log("[>>>] i2c_send_word()");
//     #endif
//     taskENTER_CRITICAL(&__MUTEX(i2c));
//     uint8_t res;
//     i2c_start_condition(i2c);
//     i2c_send_address_frame(i2c, address_7bit, 0);
//     res = i2c_get_response(i2c);
//     if( res ){
//         i2c_stop_condition(i2c);
//         taskEXIT_CRITICAL(&__MUTEX(i2c));
//         return -1;
//     }
//     i2c_com_delay(__t1);
//     i2c_send_data_frame(i2c, byte_h);
//     res = i2c_get_response(i2c);
//     if( res ){
//         i2c_stop_condition(i2c);
//         taskEXIT_CRITICAL(&__MUTEX(i2c));
//         return 0;
//     }
//     i2c_com_delay(__t1);
//     i2c_send_data_frame(i2c, byte_l);
//     res = i2c_get_response(i2c);
//     if( res ){
//         i2c_stop_condition(i2c);
//         taskEXIT_CRITICAL(&__MUTEX(i2c));
//         return 1;
//     }
//     i2c_stop_condition(i2c);
//     taskEXIT_CRITICAL(&__MUTEX(i2c));
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
//         i2ccom_log("[<<<] i2c_send_word()");
//     #endif
//     return res;
// }

// uint8_t i2c_send_byte_array(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data[], uint32_t array_size){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
//         i2ccom_log("[>>>] i2c_send_byte_array()");
//     #endif
//     taskENTER_CRITICAL(&__MUTEX(i2c));
//     uint8_t res;
//     i2c_start_condition(i2c);
//     i2c_send_address_frame(i2c, address_7bit, 0);
//     res = i2c_get_response(i2c);
//     if( res ){
//         i2c_stop_condition(i2c);
//         taskEXIT_CRITICAL(&__MUTEX(i2c));
//         return -1;
//     }
//     for(uint8_t i = 0; i < array_size; ++i){
//         i2c_com_delay(__t1);
//         i2c_send_data_frame(i2c, data[i]);
//         res = i2c_get_response(i2c);
//         if( res ){
//             i2c_stop_condition(i2c);
//             taskEXIT_CRITICAL(&__MUTEX(i2c));
//             return i;
//         }
//     }
//     i2c_stop_condition(i2c);
//     taskEXIT_CRITICAL(&__MUTEX(i2c));
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
//         i2ccom_log("[<<<] i2c_send_byte_array()");
//     #endif
//     return res;
// }

// uint8_t i2c_send_byte_array_w_ctl_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t ctl_byte, uint8_t data[], uint32_t array_size){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
//         i2ccom_log("[>>>] i2c_send_byte_array_w_ctl_byte()");
//     #endif
    
//     taskENTER_CRITICAL(&__MUTEX(i2c));
//     uint8_t res;
//     i2c_start_condition(i2c);
//     i2c_send_address_frame(i2c, address_7bit, 0);
//     res = i2c_get_response(i2c);
//     if( res ){
//         i2c_stop_condition(i2c);
//         taskEXIT_CRITICAL(&__MUTEX(i2c));
//         return -1;
//     }
//     i2c_com_delay(__t1);
//     i2c_send_data_frame(i2c, ctl_byte);
//     res = i2c_get_response(i2c);
//     if( res ){
//         i2c_stop_condition(i2c);
//         taskEXIT_CRITICAL(&__MUTEX(i2c));
//         return -1;
//     }
//     for(uint32_t i = 0; i < array_size; ++i){
//         i2c_com_delay(__t1);
//         i2c_send_data_frame(i2c, data[i]);
//         res = i2c_get_response(i2c);
//         if( res ){
//             i2c_stop_condition(i2c);
//             taskEXIT_CRITICAL(&__MUTEX(i2c));
//             return i;
//         }
//     }
//     i2c_stop_condition(i2c);
//     taskEXIT_CRITICAL(&__MUTEX(i2c));

//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
//         i2ccom_log("[<<<] i2c_send_byte_array_w_ctl_byte()");
//     #endif
//     return res;
// }