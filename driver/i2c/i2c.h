#ifndef __I2C_COM_H__
#define __I2C_COM_H__

/*Params of delay:
Start condition:
            ______
                  |
                  |
        SDA       |____________
            ____________
                        |
                        |
        SCL             |______

              ... |t1   | ...               : Time

Stop condition:
                         ______
                        |
                        |
        SDA ____________|
                   ____________
                  |
                  |
        SCL ______|
        
              ... |t1   | ...               : Time

Normal send (write op):
                   _______________
                  |               |
                  |               |
        SDA ______|_ _ _ _ _ _ _ _|______
                       _______
                      |       |
                      |       |
        SCL __________|       |__________

            |??   |t1 |t2     |??  |??   |   : Time
----> Period: 2*(t0+t1)+t2


*/

#include "../../helper/general.h"

#ifndef logic_cast
    #define logic_cast(value) ((value)>0?1:0)
#endif


// #define I2CCOM_LOG 
// #define I2CCOM_LOG_LEVEL 0
// #define I2CCOM_LOG_DATAFRAME
#if defined(I2CCOM_LOG)

    #ifdef __log
        #define i2ccom_log __log
    #else
        #include "rom/ets_sys.h"
        #include "esp_timer.h"
        #include <stdarg.h>

        #define i2ccom_log(fmt, ...) \
                ets_printf("[%lld] [I2CCOM] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)

    #endif

    #ifndef I2CCOM_LOG_LEVEL
        // #define I2CCOM_LOG_LEVEL 
        // #define I2CCOM_LOG_LEVEL 0x01 // LOG send byte array functions, startup
        // #define I2CCOM_LOG_LEVEL 0x02 // LOG send word/byte functions
        // #define I2CCOM_LOG_LEVEL 0x03 // LOG send adress/data (a part of frame)
        // #define I2CCOM_LOG_LEVEL 0x04 // LOG start/stop  condition
        // #define I2CCOM_LOG_LEVEL 0x05 // LOG mono pulse 
    #endif

    #ifndef I2CCOM_LOG_DATAFRAME
        // #define I2CCOM_LOG_DATAFRAME
    #endif
        
#endif

typedef struct I2CCommunication{
    uint8_t sda, scl;
    portMUX_TYPE mutex;
}I2CCommunication;

#ifndef __SCL
    #define __SCL(i2c) (i2c->scl)
#endif
#ifndef __SDA
    #define __SDA(i2c) (i2c->sda)
#endif
#ifndef __MUTEX
    #define __MUTEX(i2c) (i2c->mutex)
#endif
#ifndef __t1
    #define __t1        1
#endif
#ifndef __t2
    #define __t2        2
#endif

#ifndef __i2cSetSDA
    #define __i2cSetSDA
    static inline void i2cSetSDA(I2CCommunication* i2c, bool level){
        if (level) {
            GPIO.out_w1ts = (1 << __SDA(i2c));
        } else {
            GPIO.out_w1tc = (1 << __SDA(i2c));
        }
    }
#endif 
#ifndef __i2cSetSCL
    #define __i2cSetSCL
    static inline void i2cSetSCL(I2CCommunication* i2c, bool level){
        if (level) {
            GPIO.out_w1ts = (1 << __SCL(i2c));
        } else {
            GPIO.out_w1tc = (1 << __SCL(i2c));
        }
    }
#endif 

#ifndef __i2cGetSCL
    #define __i2cGetSCL
    static inline uint8_t i2cGetSCL(I2CCommunication* i2c){
        return logic_cast((GPIO.in >> __SCL(i2c)) & 0x1);
    }
#endif 

#ifndef __i2cGetSDA
    #define __i2cGetSDA
    static inline uint8_t i2cGetSDA(I2CCommunication* i2c){
        return logic_cast((GPIO.in >> __SDA(i2c)) & 0x1);
    }
#endif 

#ifndef i2c_com_delay
    // #define i2c_com_delay(__stick)
    // #define i2c_com_delay(__stick) vTaskDelay(__stick);
    #define i2c_com_delay(__us) esp_rom_delay_us(__us)
#endif 

#ifndef wait_for_scl
    #define wait_for_scl 
    // #define wait_for_scl for(uint32_t t_wait = 0; t_wait < 1000 && get_scl() == 0; ++t_wait);
    // #define wait_for_scl for(uint32_t t_wait = 0; t_wait < 1000 && get_scl() == 0; ++t_wait) i2c_com_delay(1);
#endif 

static inline void i2c_start_condition(I2CCommunication* i2c){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
        i2ccom_log("[>>>] i2c_start_condition(%p)", i2c);
    #endif
    i2cSetSDA(i2c, 1); i2cSetSCL(i2c, 1);
    i2c_com_delay(__t1);
    i2cSetSDA(i2c, 0);
    i2c_com_delay(__t1);
    i2cSetSCL(i2c, 0);
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
        i2ccom_log("[<<<] [i2c:%p] i2c_start_condition(i2c)", i2c);
    #endif
}

static inline void i2c_stop_condition(I2CCommunication* i2c){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
        i2ccom_log("[>>>] i2c_stop_condition(%p)", i2c);
    #endif
    i2cSetSDA(i2c, 0); i2cSetSCL(i2c, 0);
    i2c_com_delay(__t1);
    i2cSetSCL(i2c, 1);
    i2c_com_delay(__t1);
    i2cSetSDA(i2c, 1);
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
        i2ccom_log("[<<<] i2c_stop_condition(%p)", i2c);
    #endif
}

static inline void scl_mono_pulse(I2CCommunication* i2c){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x4)
        i2ccom_log("[>>>] scl_mono_pulse(%p)", i2c);
    #endif
    i2cSetSCL(i2c, 0);
    i2c_com_delay(__t1);
    i2cSetSCL(i2c, 1);
    i2c_com_delay(__t2);
    wait_for_scl;
    i2cSetSCL(i2c, 0);
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x4)
        i2ccom_log("[<<<] scl_mono_pulse(%p)", i2c);
    #endif
}

static inline void i2c_send_address_frame(I2CCommunication* i2c, uint8_t address_7bit, uint8_t mode){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
        i2ccom_log("[>>>] i2c_send_address_frame(%p, %d, %d)", i2c, address_7bit, mode);
    #endif
    /// 7 pulses of scl 
    uint8_t i = 0x40;
    while(i > 0){
        i2cSetSDA(i2c, logic_cast(address_7bit & i));
        scl_mono_pulse(i2c);
        i>>=1;
    };
    /// last pulse for R/W
    i2cSetSDA(i2c, logic_cast(mode));
    scl_mono_pulse(i2c);
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
        i2ccom_log("[<<<] i2c_send_address_frame()");
    #endif
}

static inline void i2c_send_data_frame(I2CCommunication* i2c, uint8_t data){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
        i2ccom_log("[>>>] i2c_send_data_frame(%p, %d)", i2c, data);
    #endif
    #if defined(I2CCOM_LOG) && defined(I2CCOM_LOG_DATAFRAME)
        i2ccom_log("[-->] <0x%02X>", data);
    #endif
    /// 8 pulses of scl 
    uint8_t i = 0x80;
    while(i > 0){
        i2cSetSDA(i2c, logic_cast(data & i));
        scl_mono_pulse(i2c);
        i>>=1;
    };
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
        i2ccom_log("[<<<] i2c_send_data_frame()");
    #endif
}

static inline uint8_t i2c_get_response(I2CCommunication* i2c){
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x5)
        i2ccom_log("[>>>] i2c_get_response(%p)", i2c);
    #endif
    uint8_t res; // 0-ACK, 1-NCK
    i2c_com_delay(__t1);
    i2cSetSDA(i2c, 1); /// release the SDA line
    i2c_com_delay(__t1);
    i2cSetSCL(i2c, 1);
    i2c_com_delay(__t2/2);
    res = i2cGetSDA(i2c);
    i2c_com_delay((__t2/2) + (__t2%2));
    wait_for_scl;
    i2cSetSCL(i2c, 0);
    #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x5)
        i2ccom_log("[<<<] i2c_get_response(i2c)");
    #endif
    return res;
}

void i2c_config_headers(I2CCommunication* i2c);
I2CCommunication* createNewI2CCommunication(uint8_t SDA, uint8_t SCL);
void deleteI2CCommunication(I2CCommunication* i2cComAddr);
uint8_t i2c_send_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data);
uint8_t i2c_send_word(I2CCommunication* i2c, uint8_t address_7bit, uint8_t byte_h, uint8_t byte_l);
uint8_t i2c_send_byte_array(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data[], uint32_t array_size);
uint8_t i2c_send_byte_array_w_ctl_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t ctl_byte, uint8_t data[], uint32_t array_size);

#endif