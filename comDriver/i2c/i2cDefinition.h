#ifndef __I2C_DEFINITIONS_H__
#define __I2C_DEFINITIONS_H__


#include "../../helper/general.h"

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
    I2C_MODE    = 2,        /// Set 1 to determine this is MASTER or SLAVE
};

/// @brief I2C configuration parameters
enum I2C_SPEED_MODE {
    I2C_STANDARD_MODE   = 0,    ///< Clk freq ~ 100 kHz
    I2C_FAST_MODE       = 1,    ///< Clk freq ~ 400 kHz
    I2C_FAST_PLUS_MODE  = 2,    ///< Clk freq ~ 1 MHz
};

/// @brief I2C configuration parameters
enum I2C_READWRITE_MODE {
    I2C_WRITE_MODE  = 0,        ///< 
    I2C_READ_MODE   = 1,        ///< 
};

/// @brief I2C RETURN BIT
enum I2C_RETURN_BIT {
    I2C_ACK         = LOW,        ///< 
    I2C_NACK        = HIGH,       ///< 
};

/// @brief I2C RETURN BIT
enum I2C_EXTENT_CONFIG_BIT_ORDER {
    I2C_DIS_START_CONDI         = 0,        ///< Set 1 to disable start condition
    I2C_DIS_STOP_CONDI          = 1,        ///< Set 1 to disable stop condition, 0 to keep the transaction
    I2C_DIS_ENTRY_CRITICAL_SEC  = 2,        ///< Set 1 to disable entry critical section 
    I2C_DIS_EXIT_CRITICAL_SEC   = 3,        ///< Set 1 to disable exit critical section
    I2C_RESET_TX_INDEX         = 4,        ///< Set 1 to reset transmit buffer index before transmit
    I2C_RESET_RX_INDEX         = 5,        ///< Set 1 to reset receive buffer index before transmit
};

#endif 