#include "i2c.h"

void i2c_config_headers(I2CCommunication* i2c){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0)
        i2ccom_log("[>>>] i2c_config_headers(%p)", i2c);
    #endif

    gpio_config_t sda_header_config = {
        .intr_type      = GPIO_INTR_DISABLE,
        .mode           = GPIO_MODE_OUTPUT_OD,
        .pin_bit_mask   = (1ULL << __SDA(i2c)),
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_ENABLE
    };
    gpio_config(&sda_header_config);

    gpio_config_t scl_header_config = {
        .intr_type      = GPIO_INTR_DISABLE,
        .mode           = GPIO_MODE_OUTPUT_OD,
        .pin_bit_mask   = (1ULL << __SCL(i2c)),
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_ENABLE
    };
    gpio_config(&scl_header_config);
    i2cSetSCL(i2c, 1);
    i2cSetSDA(i2c, 1);
    i2c_com_delay(__t2);
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0)
        i2ccom_log("[<<<] i2c_config_headers()");
    #endif
}

I2CCommunication* createNewI2CCommunication(uint8_t SDA, uint8_t SCL) {
    I2CCommunication* i2c = malloc(sizeof(I2CCommunication));
    if (!i2c) return NULL;
    
    i2c->sda = SDA;
    i2c->scl = SCL;
    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
    i2c->mutex = mutex;
    i2c_config_headers(i2c);
    return i2c;
}

void deleteI2CCommunication(I2CCommunication* i2cComAddr){
    if(i2cComAddr) free(i2cComAddr);
}

uint8_t i2c_send_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
        i2ccom_log("[>>>] i2c_send_byte()");
    #endif
    taskENTER_CRITICAL(&__MUTEX(i2c));
    uint8_t res;
    i2c_start_condition(i2c);
    i2c_send_address_frame(i2c, address_7bit, 0);
    if( i2c_get_response(i2c) ){
        i2c_stop_condition(i2c);
        taskEXIT_CRITICAL(&__MUTEX(i2c));
        return -1;
    }
    i2c_send_data_frame(i2c, data);
    res = i2c_get_response(i2c);
    i2c_stop_condition(i2c);
    taskEXIT_CRITICAL(&__MUTEX(i2c));
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
        i2ccom_log("[<<<] i2c_send_byte()");
    #endif
    return res;
}

uint8_t i2c_send_word(I2CCommunication* i2c, uint8_t address_7bit, uint8_t byte_h, uint8_t byte_l){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
        i2ccom_log("[>>>] i2c_send_word()");
    #endif
    taskENTER_CRITICAL(&__MUTEX(i2c));
    uint8_t res;
    i2c_start_condition(i2c);
    i2c_send_address_frame(i2c, address_7bit, 0);
    res = i2c_get_response(i2c);
    if( res ){
        i2c_stop_condition(i2c);
        taskEXIT_CRITICAL(&__MUTEX(i2c));
        return -1;
    }
    i2c_com_delay(__t1);
    i2c_send_data_frame(i2c, byte_h);
    res = i2c_get_response(i2c);
    if( res ){
        i2c_stop_condition(i2c);
        taskEXIT_CRITICAL(&__MUTEX(i2c));
        return 0;
    }
    i2c_com_delay(__t1);
    i2c_send_data_frame(i2c, byte_l);
    res = i2c_get_response(i2c);
    if( res ){
        i2c_stop_condition(i2c);
        taskEXIT_CRITICAL(&__MUTEX(i2c));
        return 1;
    }
    i2c_stop_condition(i2c);
    taskEXIT_CRITICAL(&__MUTEX(i2c));
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x1)
        i2ccom_log("[<<<] i2c_send_word()");
    #endif
    return res;
}

uint8_t i2c_send_byte_array(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data[], uint32_t array_size){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
        i2ccom_log("[>>>] i2c_send_byte_array()");
    #endif
    taskENTER_CRITICAL(&__MUTEX(i2c));
    uint8_t res;
    i2c_start_condition(i2c);
    i2c_send_address_frame(i2c, address_7bit, 0);
    res = i2c_get_response(i2c);
    if( res ){
        i2c_stop_condition(i2c);
        taskEXIT_CRITICAL(&__MUTEX(i2c));
        return -1;
    }
    for(uint8_t i = 0; i < array_size; ++i){
        i2c_com_delay(__t1);
        i2c_send_data_frame(i2c, data[i]);
        res = i2c_get_response(i2c);
        if( res ){
            i2c_stop_condition(i2c);
            taskEXIT_CRITICAL(&__MUTEX(i2c));
            return i;
        }
    }
    i2c_stop_condition(i2c);
    taskEXIT_CRITICAL(&__MUTEX(i2c));
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
        i2ccom_log("[<<<] i2c_send_byte_array()");
    #endif
    return res;
}

uint8_t i2c_send_byte_array_w_ctl_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t ctl_byte, uint8_t data[], uint32_t array_size){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
        i2ccom_log("[>>>] i2c_send_byte_array_w_ctl_byte()");
    #endif
    
    taskENTER_CRITICAL(&__MUTEX(i2c));
    uint8_t res;
    i2c_start_condition(i2c);
    i2c_send_address_frame(i2c, address_7bit, 0);
    res = i2c_get_response(i2c);
    if( res ){
        i2c_stop_condition(i2c);
        taskEXIT_CRITICAL(&__MUTEX(i2c));
        return -1;
    }
    i2c_com_delay(__t1);
    i2c_send_data_frame(i2c, ctl_byte);
    res = i2c_get_response(i2c);
    if( res ){
        i2c_stop_condition(i2c);
        taskEXIT_CRITICAL(&__MUTEX(i2c));
        return -1;
    }
    for(uint32_t i = 0; i < array_size; ++i){
        i2c_com_delay(__t1);
        i2c_send_data_frame(i2c, data[i]);
        res = i2c_get_response(i2c);
        if( res ){
            i2c_stop_condition(i2c);
            taskEXIT_CRITICAL(&__MUTEX(i2c));
            return i;
        }
    }
    i2c_stop_condition(i2c);
    taskEXIT_CRITICAL(&__MUTEX(i2c));

    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x0)
        i2ccom_log("[<<<] i2c_send_byte_array_w_ctl_byte()");
    #endif
    return res;
}