#ifndef __I2C_H__
#define __I2C_H__

/// LOG CONTROL ///////////////////////////////////////////////////////////////////////////////////

#include "../../helper/general.h"

/*
 *  Set level of log, there are the description of the leves:
 *  Noted that: the greater value of log includes all lower log types!:
 *      1 : Only log error
 *      2 : Log entry (+ params) and exit (+ return status)
 *      3 : Log state inside the function, ...
 *      4 : The lowest level of logs, it log all things, includes every byte will be sent!
*/
#define I2C_LOG_LEVEL 3

#if I2C_LOG_LEVEL >= 1
    #define __i2cErr(...)       __err( ##__VA_ARGS__ )
#else 
    #define __i2cErr(...)
#endif
#if I2C_LOG_LEVEL >= 2
    #define __i2cEntry(...)     __entry( ##__VA_ARGS__ )
    #define __i2cExit(...)      __exit( ##__VA_ARGS__ )
#else 
    #define __i2cEntry(...)
    #define __i2cExit(...)
#endif 
#if I2C_LOG_LEVEL >= 3
    #define __i2cLog(...)      __log( ##__VA_ARGS__ )
#else 
    #define __i2cLog(...)
#endif 
#if I2C_LOG_LEVEL >= 4
    #define __i2cLog1(...)      __log( ##__VA_ARGS__ )
#else 
    #define __i2cLog1(...)
#endif 

/// THE DEFINITIONS RELATED TO I2C ////////////////////////////////////////////////////////////////

#define __I2C_ASSERT_PIN_OK(pin, name)                   \
    do {                                                 \
        if(__isnot_positive(pin)){                       \
            __i2cErr("[I2C] %s = %d is invalid",        \
                     name, pin);                         \
            esp_restart();                               \
            __builtin_unreachable();                     \
        }                                                \
    } while(0)

#define __i2cDelay(us) esp_rom_delay_us(us)

typedef struct i2cDev_t{
    uint8_t     addr;       ///< Slave mode
    pin_t       scl;        ///< Clock pin
    pin_t       sda;        ///< Data in/out pin
    flag_t      conf;       ///< Configuration flags. Note: conf[1:0] = speed mode; conf[2] = mode (0: master / 1: slave)
    uint32_t    freq;       ///< The freq of CLK, is set automatically via [speed mode]
    void*       txPtr;      ///< Pointer to TX buffer
    size_t      txSize;     ///< TX buffer size in bytes
    size_t      txIndByte;  ///< Current TX byte index
    uint8_t     txIndBit;   ///< TX bit index within current byte
    void*       rxPtr;      ///< Pointer to RX buffer
    size_t      rxSize;     ///< RX buffer size in bytes
    size_t      rxIndByte;  ///< Current RX byte index
    uint8_t     rxIndBit;   ///< RX bit index within current byte
    portMUX_TYPE mutex;     ///< Mutex lock for multi-tasking
}i2cDev_t;

/// @brief I2C device role
enum I2C_DEVICE {
    I2C_MASTER = 0, ///< Master mode
    I2C_SLAVE  = 1  ///< Slave mode
};

enum I2C_CONFIG_ORDER {
    I2C_SPEED_0 = 0,
    I2C_SPEED_1 = 1,
    I2C_MODE    = 2,
};

/// @brief I2C configuration parameters
enum I2C_SPEED_MODE {
    I2C_STANDARD_MODE   = 0,    ///< Clk freq ~ 100 kHz
    I2C_FAST_MODE       = 1,    ///< Clk freq ~ 400 kHz
    I2C_FAST_PLUS_MODE  = 2,    ///< Clk freq ~ 1 MHz
};

/// HELPER/INLINE FUNC ////////////////////////////////////////////////////////////////////////////

/// @brief Get I2C bus frequency of a given device.
/// @param dev Pointer to I2C device.
/// @return Frequency in Hz.
/// @note Will reset system if frequency is invalid.
static inline uint32_t __i2cFreq(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(dev->freq, STR(freq));
    return dev->freq;
}

/// @brief Get the SCL (clock) pin of a given I2C device.
/// @param dev Pointer to I2C device.
/// @return Pin number for SCL.
/// @note Will reset system if pin is invalid.
static inline pin_t __i2cSCL(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(dev->sda, STR(dev->sda));
    return dev->scl;
}

/// @brief Get the SDA (data) pin of a given I2C device.
/// @param dev Pointer to I2C device.
/// @return Pin number for SDA.
/// @note Will reset system if pin is invalid.
static inline pin_t __i2cSDA(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(dev->scl, STR(dev->scl));
    return dev->sda;
}

/// @brief Set logic level for SDA line of an I2C device.
/// @param dev Pointer to I2C device.
/// @param level Logic level (0 = low, 1 = high).
/// @note Will reset system if SDA pin is invalid.
static inline void __i2cSetSDA(i2cDev_t * dev, def level){
    __I2C_ASSERT_PIN_OK(dev->sda, STR(dev->sda));
    if(level)
        GPIO.out_w1ts = __mask32(dev->sda);
    else
        GPIO.out_w1tc = __mask32(dev->sda);
}

/// @brief Set logic level for SCL line of an I2C device.
/// @param dev Pointer to I2C device.
/// @param level Logic level (0 = low, 1 = high).
/// @note Will reset system if SCL pin is invalid.
static inline void __i2cSetSCL(i2cDev_t * dev, def level){
    __I2C_ASSERT_PIN_OK(dev->scl, STR(dev->scl));
    if(level)
        GPIO.out_w1ts = __mask32(dev->scl);
    else
        GPIO.out_w1tc = __mask32(dev->scl);
}

/// @brief Read the current logic level of SDA line.
/// @param dev Pointer to I2C device.
/// @return Current logic level of SDA (0 or 1).
/// @note Will reset system if SDA pin is invalid.
static inline def __i2cGetSDA(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(dev->sda, STR(dev->sda));
    return boolCast(GPIO.in & __mask32(dev->sda));
}

/// @brief Read the current logic level of SCL line.
/// @param dev Pointer to I2C device.
/// @return Current logic level of SCL (0 or 1).
/// @note Will reset system if SCL pin is invalid.
static inline def __i2cGetSCL(i2cDev_t * dev){
    __I2C_ASSERT_PIN_OK(dev->scl, STR(dev->scl));
    return boolCast(GPIO.in & __mask32(dev->scl));
}


static inline void startCondition(i2cDev_t * dev){

    __i2cDelay(250000/(__i2cFreq(dev)));

    /// Release both SCL, SDA line
    __i2cSetSCL(dev, HIGH);
    __i2cSetSDA(dev, HIGH);

    /// Wait for both SCL and SDA are stable (a half of SCL period)
    __i2cDelay(500000/(__i2cFreq(dev)));

    /// Make start condition (SDA down to LOW when SCL is HIGH)
    __i2cSetSDA(dev, LOW);

    /// Wait for a quarter of SCL period
    __i2cDelay(250000/(__i2cFreq(dev)));
    
    /// Pull SCL down for 1st clock pulse
    __i2cSetSCL(dev, LOW);
    
}

static inline void stopCondition(i2cDev_t * dev){

    __i2cDelay(250000/(__i2cFreq(dev)));

    /// Make sure both SCL, SDA line are LOW 
    __i2cSetSCL(dev, LOW);
    __i2cSetSDA(dev, LOW);

    /// Wait for both SCL and SDA are stable (a half of SCL period)
    __i2cDelay(500000/(__i2cFreq(dev)));

    /// Pull SCL up to HIGH for stop condition
    __i2cSetSCL(dev, HIGH);
    
    /// Wait for a quarter of SCL period
    __i2cDelay(250000/(__i2cFreq(dev)));
    
    /// Make stop condition (SDA is pulled up to HIGH when SCL is HIGH)
    __i2cSetSDA(dev, HIGH);

}

/// MAIN FUNC /////////////////////////////////////////////////////////////////////////////////////

/// @brief Handle SCL interrupt in slave mode
/// @param pv Pointer to i2cDev_t
void IRAM_ATTR i2cHandleSCLIsr(void* pv);

/// @brief Handle SDA interrupt in slave mode
/// @param pv Pointer to i2cDev_t
void IRAM_ATTR i2cHandleSDAIsr(void* pv);

/// @brief Create a new I2C device instance
/// @param pDev Pointer to pointer of i2cDev_t (will store created device)
/// @return Default return status
def createI2CDevice(i2cDev_t ** pDev);

/// @brief Configure I2C device pins and options
/// @param dev Pointer to i2cDev_t
/// @param addr Address of this device (for slave mode)
/// @param scl Pin number for SCL
/// @param sda Pin number for SDA
/// @param config Configuration flags
/// @return Default return status
def configI2CDevice(i2cDev_t * dev, uint8_t addr, pin_t scl, pin_t sda, flag_t config);

/// @brief Startup the I2C device (initialize GPIO and configs)
/// @param dev Pointer to i2cDev_t
/// @return Default return status
int startupI2CDevice(i2cDev_t * dev);

/// @brief Set transmit buffer for I2C
/// @param dev Pointer to i2cDev_t
/// @param txdPtr Pointer to transmit buffer
/// @param size Size of transmit buffer (in bytes)
/// @return Default return status
def setTransmitBuffer(i2cDev_t * dev, void * txdPtr, size_t size);

/// @brief Set receive


#endif 


// #ifndef __I2C_COM_H__
// #define __I2C_COM_H__

// /*Params of delay:
// Start condition:
//             ______
//                   |
//                   |
//         SDA       |____________
//             ____________
//                         |
//                         |
//         SCL             |______

//               ... |t1   | ...               : Time

// Stop condition:
//                          ______
//                         |
//                         |
//         SDA ____________|
//                    ____________
//                   |
//                   |
//         SCL ______|
        
//               ... |t1   | ...               : Time

// Normal send (write op):
//                    _______________
//                   |               |
//                   |               |
//         SDA ______|_ _ _ _ _ _ _ _|______
//                        _______
//                       |       |
//                       |       |
//         SCL __________|       |__________

//             |??   |t1 |t2     |??  |??   |   : Time
// ----> Period: 2*(t0+t1)+t2


// */

// #include "../../helper/general.h"

// #ifndef logic_cast
//     #define logic_cast(value) ((value)>0?1:0)
// #endif


// // #define I2CCOM_LOG 
// // #define I2CCOM_LOG_LEVEL 0
// // #define I2CCOM_LOG_DATAFRAME
// #if defined(I2CCOM_LOG)

//     #ifdef __log
//         #define i2ccom_log __log
//     #else
//         #include "rom/ets_sys.h"
//         #include "esp_timer.h"
//         #include <stdarg.h>

//         #define i2ccom_log(fmt, ...) \
//                 ets_printf("[%lld] [I2CCOM] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)

//     #endif

//     #ifndef I2CCOM_LOG_LEVEL
//         // #define I2CCOM_LOG_LEVEL 
//         // #define I2CCOM_LOG_LEVEL 0x01 // LOG send byte array functions, startup
//         // #define I2CCOM_LOG_LEVEL 0x02 // LOG send word/byte functions
//         // #define I2CCOM_LOG_LEVEL 0x03 // LOG send adress/data (a part of frame)
//         // #define I2CCOM_LOG_LEVEL 0x04 // LOG start/stop  condition
//         // #define I2CCOM_LOG_LEVEL 0x05 // LOG mono pulse 
//     #endif

//     #ifndef I2CCOM_LOG_DATAFRAME
//         // #define I2CCOM_LOG_DATAFRAME
//     #endif
        
// #endif

// typedef struct I2CCommunication{
//     uint8_t sda, scl;
//     portMUX_TYPE mutex;
// }I2CCommunication;

// #ifndef __SCL
//     #define __SCL(i2c) (i2c->scl)
// #endif
// #ifndef __SDA
//     #define __SDA(i2c) (i2c->sda)
// #endif
// #ifndef __MUTEX
//     #define __MUTEX(i2c) (i2c->mutex)
// #endif
// #ifndef __t1
//     #define __t1        1
// #endif
// #ifndef __t2
//     #define __t2        2
// #endif

// #ifndef __i2cSetSDA
//     #define __i2cSetSDA
//     static inline void i2cSetSDA(I2CCommunication* i2c, bool level){
//         if (level) {
//             GPIO.out_w1ts = (1 << __SDA(i2c));
//         } else {
//             GPIO.out_w1tc = (1 << __SDA(i2c));
//         }
//     }
// #endif 
// #ifndef __i2cSetSCL
//     #define __i2cSetSCL
//     static inline void i2cSetSCL(I2CCommunication* i2c, bool level){
//         if (level) {
//             GPIO.out_w1ts = (1 << __SCL(i2c));
//         } else {
//             GPIO.out_w1tc = (1 << __SCL(i2c));
//         }
//     }
// #endif 

// #ifndef __i2cGetSCL
//     #define __i2cGetSCL
//     static inline uint8_t i2cGetSCL(I2CCommunication* i2c){
//         return logic_cast((GPIO.in >> __SCL(i2c)) & 0x1);
//     }
// #endif 

// #ifndef __i2cGetSDA
//     #define __i2cGetSDA
//     static inline uint8_t i2cGetSDA(I2CCommunication* i2c){
//         return logic_cast((GPIO.in >> __SDA(i2c)) & 0x1);
//     }
// #endif 

// #ifndef i2c_com_delay
//     // #define i2c_com_delay(__stick)
//     // #define i2c_com_delay(__stick) vTaskDelay(__stick);
//     #define i2c_com_delay(__us) esp_rom_delay_us(__us)
// #endif 

// #ifndef wait_for_scl
//     #define wait_for_scl 
//     // #define wait_for_scl for(uint32_t t_wait = 0; t_wait < 1000 && get_scl() == 0; ++t_wait);
//     // #define wait_for_scl for(uint32_t t_wait = 0; t_wait < 1000 && get_scl() == 0; ++t_wait) i2c_com_delay(1);
// #endif 

// static inline void i2c_start_condition(I2CCommunication* i2c){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
//         i2ccom_log("[>>>] i2c_start_condition(%p)", i2c);
//     #endif
//     i2cSetSDA(i2c, 1); i2cSetSCL(i2c, 1);
//     i2c_com_delay(__t1);
//     i2cSetSDA(i2c, 0);
//     i2c_com_delay(__t1);
//     i2cSetSCL(i2c, 0);
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
//         i2ccom_log("[<<<] [i2c:%p] i2c_start_condition(i2c)", i2c);
//     #endif
// }

// static inline void i2c_stop_condition(I2CCommunication* i2c){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
//         i2ccom_log("[>>>] i2c_stop_condition(%p)", i2c);
//     #endif
//     i2cSetSDA(i2c, 0); i2cSetSCL(i2c, 0);
//     i2c_com_delay(__t1);
//     i2cSetSCL(i2c, 1);
//     i2c_com_delay(__t1);
//     i2cSetSDA(i2c, 1);
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x3)
//         i2ccom_log("[<<<] i2c_stop_condition(%p)", i2c);
//     #endif
// }

// static inline void scl_mono_pulse(I2CCommunication* i2c){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x4)
//         i2ccom_log("[>>>] scl_mono_pulse(%p)", i2c);
//     #endif
//     i2cSetSCL(i2c, 0);
//     i2c_com_delay(__t1);
//     i2cSetSCL(i2c, 1);
//     i2c_com_delay(__t2);
//     wait_for_scl;
//     i2cSetSCL(i2c, 0);
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x4)
//         i2ccom_log("[<<<] scl_mono_pulse(%p)", i2c);
//     #endif
// }

// static inline void i2c_send_address_frame(I2CCommunication* i2c, uint8_t address_7bit, uint8_t mode){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
//         i2ccom_log("[>>>] i2c_send_address_frame(%p, %d, %d)", i2c, address_7bit, mode);
//     #endif
//     /// 7 pulses of scl 
//     uint8_t i = 0x40;
//     while(i > 0){
//         i2cSetSDA(i2c, logic_cast(address_7bit & i));
//         scl_mono_pulse(i2c);
//         i>>=1;
//     };
//     /// last pulse for R/W
//     i2cSetSDA(i2c, logic_cast(mode));
//     scl_mono_pulse(i2c);
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
//         i2ccom_log("[<<<] i2c_send_address_frame()");
//     #endif
// }

// static inline void i2c_send_data_frame(I2CCommunication* i2c, uint8_t data){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
//         i2ccom_log("[>>>] i2c_send_data_frame(%p, %d)", i2c, data);
//     #endif
//     #if defined(I2CCOM_LOG) && defined(I2CCOM_LOG_DATAFRAME)
//         i2ccom_log("[-->] <0x%02X>", data);
//     #endif
//     /// 8 pulses of scl 
//     uint8_t i = 0x80;
//     while(i > 0){
//         i2cSetSDA(i2c, logic_cast(data & i));
//         scl_mono_pulse(i2c);
//         i>>=1;
//     };
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x2)
//         i2ccom_log("[<<<] i2c_send_data_frame()");
//     #endif
// }

// static inline uint8_t i2c_get_response(I2CCommunication* i2c){
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x5)
//         i2ccom_log("[>>>] i2c_get_response(%p)", i2c);
//     #endif
//     uint8_t res; // 0-ACK, 1-NCK
//     i2c_com_delay(__t1);
//     i2cSetSDA(i2c, 1); /// release the SDA line
//     i2c_com_delay(__t1);
//     i2cSetSCL(i2c, 1);
//     i2c_com_delay(__t2/2);
//     res = i2cGetSDA(i2c);
//     i2c_com_delay((__t2/2) + (__t2%2));
//     wait_for_scl;
//     i2cSetSCL(i2c, 0);
//     #if defined(I2CCOM_LOG) && (I2CCOM_LOG_LEVEL > 0x5)
//         i2ccom_log("[<<<] i2c_get_response(i2c)");
//     #endif
//     return res;
// }

// void i2c_config_headers(I2CCommunication* i2c);
// I2CCommunication* createNewI2CCommunication(uint8_t SDA, uint8_t SCL);
// void deleteI2CCommunication(I2CCommunication* i2cComAddr);
// uint8_t i2c_send_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data);
// uint8_t i2c_send_word(I2CCommunication* i2c, uint8_t address_7bit, uint8_t byte_h, uint8_t byte_l);
// uint8_t i2c_send_byte_array(I2CCommunication* i2c, uint8_t address_7bit, uint8_t data[], uint32_t array_size);
// uint8_t i2c_send_byte_array_w_ctl_byte(I2CCommunication* i2c, uint8_t address_7bit, uint8_t ctl_byte, uint8_t data[], uint32_t array_size);

// #endif