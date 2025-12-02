#include "../safeCom/safeCom.h"


/* ========================================================================== */
/* PROTOCOL DEFINITIONS (PACKED 4-BIT LOGIC)                                  */
/* ========================================================================== */

#define SC_PACK_ID(type, cmd)   ( ((type) << 4) | ((cmd) & 0x0F) )
#define SC_GET_TYPE(byte)       ( ((byte) >> 4) & 0x0F )
#define SC_GET_CMD(byte)        ( (byte) & 0x0F )

enum scFrameType_e {
    SC_TYPE_CMD_SET = 0x1,  /// [Master -> Slave] Write/Config
    SC_TYPE_REQ_DAT = 0x2,  /// [Master -> Slave] Request Data
    SC_TYPE_RET_DAT = 0xA,  /// [Slave -> Master] Return Data
    SC_TYPE_RET_ERR = 0xE   /// [Slave -> Master] Return Error/Status
};

enum scFuncCode_e {
    // Hardware Control (CMD_SET)
    FUNC_GPIO_CFG   = 0x1,  // Config Dir
    FUNC_GPIO_VAL   = 0x2,  // Write Value
    FUNC_SPI_CFG    = 0x3,
    FUNC_I2C_CFG    = 0x4,

    // Data Request (REQ_DAT)
    FUNC_READ_GPIO  = 0x1,  // Re-use 0x1 but context is different based on TYPE
    FUNC_READ_ADC   = 0x2,  
    FUNC_READ_SPI   = 0x3,
    FUNC_READ_I2C   = 0x4
};

// 3. STATUS CODES (Payload của SC_TYPE_RET_ERR)
enum scStatus_e {
    STAT_OK         = 0x00,
    STAT_FAIL       = 0x0F
};