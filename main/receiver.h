#include "../helper/general.h"

#include "../comDriver/spi/spi.h"
#include "../comDriver/oled128x64/oled128x64.h"

/// DEFINITIONS ///////////////////////////////////////////////////////////////////////////////////

/// @brief Enumeration defining the system operation stages.
enum ENUM_SYSTEM_STAGE {
    SYSTEM_INIT    = 0, /// System is initializing.
    SYSTEM_RUNNING = 1, /// System is currently running.
    SYSTEM_STOPPED = 2, /// System has been stopped or halted.
    SYSTEM_STAGE_COUNT,
};

/// @brief Global system stage flag used to represent the overall runtime state.
/// @note Access to this variable must be protected by `systemStageMutex`.
extern volatile flag_t  systemStage;

/// @brief Mutex for synchronizing access to the system stage flag.
extern portMUX_TYPE     systemStageMutex;

/// @brief Enumeration defining OLED operation request types.
enum ENUM_OLED_FLAG {
    OLED_REQ_FLUSH       = 0, /// Request to flush (update) the OLED display.
    OLED_REQ_CLR_CANVAS  = 1, /// Request to clear the OLED display canvas.
    OLED_REQ_COUNT,
};

/// @brief OLED control flag for requesting display operations.
/// @note Access to this variable must be protected by `oledFlagMutex`.
extern volatile flag_t  oledFlag;

/// @brief Mutex for synchronizing access to the OLED flag.
extern portMUX_TYPE     oledFlagMutex;

/// @brief Enumeration defining supported system communication modes.
enum ENUM_SYSTEM_MODE {
    SYSTEM_MODE_SPI    = 0, /// SPI communication mode.
    SYSTEM_MODE_I2C    = 1, /// I2C communication mode.
    SYSTEM_MODE_UART   = 2, /// UART communication mode
    SYSTEM_MODE_1WIRE  = 3, /// 1-Wire communication mode.
    SYSTEM_MODE_COUNT       /// Number of system modes
};

/// @brief Global system mode flag representing active communication mode.
/// @note Access to this variable must be protected by `systemModeMutex`.
extern volatile flag_t  systemMode;

/// @brief Current protocol mode
extern volatile flag_t  currentSystemMode;

/// @brief Mutex for synchronizing access to the system mode flag.
extern portMUX_TYPE     systemModeMutex;

/// @brief Communication object
/// @note It can be assigned as I2C, SPI, ...
extern void* comObject;

/// @brief Contain 8-bit data frame 
extern volatile uint8_t byteData;

enum BYTE_DATA_CTRL {
    BYTEDATA_GEN_PLAYPAUSE = 0,         /// 0 - Play | 1 - Pause
    BYTEDATA_GEN_NOW = 0,               /// Set 1 to gen now
    BYTE_DATA_CTRL_NUM
};

extern volatile flag_t  byteDataControlFlag;

extern portMUX_TYPE     byteDataControlMutex;

/// @brief Received control flag
extern volatile flag_t  rcvdControlFlag;

/// @brief Mutex for synchronizing access to the rcvdControlFlag.
extern portMUX_TYPE     rcvdControlMutex;

/// @brief Enumeration defining SEND operation request types.
enum RECEIVE_CONTROL_FLAG {
    RCVCTRL_HAS_DATA       = 0,        /// 1: Has received a byte data frame
    RCVCTRL_NEW_DATA       = 1,        /// 1: Assign new sendback data, reset buffer
    RECEIVE_CONTROL_FLAG_NUM
};



/// LOCAL /////////////////////////////////////////////////////////////////////////////////////////

/// System Stage 
volatile flag_t systemStage = SYSTEM_INIT;
portMUX_TYPE systemStageMutex = portMUX_INITIALIZER_UNLOCKED;

/// OLED Flags
volatile flag_t oledFlag = OLED_REQ_COUNT;
portMUX_TYPE oledFlagMutex = portMUX_INITIALIZER_UNLOCKED;

/// System Mode
volatile flag_t systemMode = SYSTEM_MODE_SPI;
volatile flag_t currentSystemMode = -1;
portMUX_TYPE systemModeMutex = portMUX_INITIALIZER_UNLOCKED;

/// Communication Object
void* comObject = NULL;

/// Random 8-bit char (printable)
volatile uint8_t byteData = '?';
/// Random byte data control flag
volatile flag_t  byteDataControlFlag = 0;
/// Random byte data flag mutex
portMUX_TYPE     byteDataControlMutex = portMUX_INITIALIZER_UNLOCKED;

/// Received data control flag
volatile flag_t  rcvdControlFlag = 0;
/// Received data control flag mutex
portMUX_TYPE     rcvdControlMutex = portMUX_INITIALIZER_UNLOCKED;

/// PREPAIR AND SEND DATA TASK ////////////////////////////////////////////////////////////////////


def setSendBackData(uint8_t *byteData){
    __entry("setSendBackData(%p)", byteData);
    def ret;

    switch (currentSystemMode){
        case SYSTEM_MODE_SPI:            
            __log("SPI set sendback byte: %c", *byteData);
            ret = spiSetTransmitBuffer((spiDev_t *) comObject, byteData, 1);
            if(ret!=OKE) goto setSendBackData_ReturnERR;

        break;
        
        case SYSTEM_MODE_I2C:
            __log("I2C not available!");
        break;
        
        case SYSTEM_MODE_UART:
            __log("UART not available!");
        break;
        
        case SYSTEM_MODE_1WIRE:
            __log("UART not available!");
        break;
        
        default:
            __err("MODE: systemMode is not valid! --> Abort!");
            ret = ERR;
            goto setSendBackData_ReturnERR;
    }
    
    __exit("setSendBackData() : OKE");
    return OKE;

    setSendBackData_ReturnERR:
        __exit("setSendBackData() : ERR");
        return ERR;
}

def setReceiveByteBuff(uint8_t *byteData, size_t buffBize){
    __entry("setReceiveByteBuff(%p)", byteData);
    def ret;

    switch (currentSystemMode){
        case SYSTEM_MODE_SPI:            
            __log("SPI set sendback byte: %c", *byteData);
            ret = spiSetReceiveBuffer((spiDev_t *) comObject, byteData, buffBize);
            if(ret!=OKE) goto setReceiveByteBuff_ReturnERR;

        break;
        
        case SYSTEM_MODE_I2C:
            __log("I2C not available!");
        break;
        
        case SYSTEM_MODE_UART:
            __log("UART not available!");
        break;
        
        case SYSTEM_MODE_1WIRE:
            __log("UART not available!");
        break;
        
        default:
            __err("MODE: systemMode is not valid! --> Abort!");
            ret = ERR;
            goto setReceiveByteBuff_ReturnERR;
    }
    
    __exit("setReceiveByteBuff() : OKE");
    return OKE;

    setReceiveByteBuff_ReturnERR:
        __exit("setReceiveByteBuff() : ERR");
        return ERR;
}

def checkNewData(){
    // __entry("checkNewData()");
    
    def ret = ERR;
    
    if(__is_null(comObject)) {ret = ERR_NULL; goto checkNewData_ReturnERR;}

    switch (currentSystemMode){
        case SYSTEM_MODE_SPI:            
            return __hasFlagBitSet(((spiDev_t *) comObject)->stat, SPISTAT_RBUFF_FULL);
        
        case SYSTEM_MODE_I2C:
            __log("I2C not available!");
        break;
        
        case SYSTEM_MODE_UART:
            __log("UART not available!");
        break;
        
        case SYSTEM_MODE_1WIRE:
            __log("UART not available!");
        break;
        
        default:
            __err("MODE: systemMode is not valid! --> Abort!");
            ret = ERR; goto checkNewData_ReturnERR;
    }
    
    checkNewData_ReturnERR:
        // __exit("checkNewData() : [%d]", ret);
        return ret;
}

def resetReceiveBuff(){
    return OKE;
}

/// @brief Random printable character
/// @param pv As required from. Pls pass NULL ptr!
void prepairByteDataTask(void *pv){    
    /// Wait for the initializing done
    __log("[prepairByteDataTask] Wait for the initializing have been done!");
    for( ; systemStage == SYSTEM_INIT; vTaskDelay(1));

    __entry("prepairByteDataTask(%p)", pv);

    while (systemStage != SYSTEM_STOPPED){
        /// Main loop, period ~ DATAFR_INTERVAL
        if(__hasFlagBitClr(byteDataControlFlag, BYTEDATA_GEN_PLAYPAUSE)) {
            byteData = ((generateRandom(0)) % (126-32+1)) + 32;
            // __log("[prepairByteDataTask] Generated : %c", byteData);
        }
        REP(i, 0, DATAFR_INTERVAL) {
            /// Seccond loop, generate new byteData immediately
            if(__hasFlagBitSet(byteDataControlFlag, BYTEDATA_GEN_NOW)){
                /// Generate
                byteData = ((generateRandom(0)) % (126-32+1)) + 32;
                /// Reset flag
                vPortEnterCritical(&byteDataControlMutex);
                __clearFlagBit(byteDataControlFlag, BYTEDATA_GEN_NOW);
                vPortExitCritical(&byteDataControlMutex);
            }
            /// Delay 1 tick
            vTaskDelay(1);
        }
    }
    
    __exit("prepairByteDataTask()");
}

/// MODE CONTROL TASK /////////////////////////////////////////////////////////////////////////////

/// @brief ISR callback for the mode-change button.
/// @details This ISR captures timestamps or counts of short/long presses
///          depending on the button's edge state. It is designed to execute
///          quickly, without any blocking calls (no long delays or logging).
///
/// @param pv Pointer to an array of two int64_t values:
///           - [0] = number of short presses (<= SHORTPRESS_TIME)
///           - [2] = number of long presses  (>= LONGPRESS_TIME)
///           - [1] = number of nrom presses  Otherwise
void IRAM_ATTR changeModeButtonISR(void* pv) {

    /// Get entry time
    int64_t timeNow = esp_timer_get_time();

    /// Cast to correct form
    int32_t* shortPressedNum = (int32_t*) pv;
    int32_t* normPressedNum  = (int32_t*) pv + 1;
    int32_t* longPressedNum  = (int32_t*) pv + 2;

    /// static variable for debouncing (store last stable state)
    static int64_t lastEdgeTime = 0;

    /// static veriable for last stable raising time
    static int64_t lastPosEdgeTime = 0;

    /// static veriable for last stable falling time
    static int64_t lastNegEdgeTime = 0;

    if (timeNow - lastEdgeTime < DEBOUNCE_TIME) return; // ignore bounces
    lastEdgeTime = timeNow;

    // __entry("changeModeButtonISR(%p, %ld, %ld, %ld, %ld, %ld)", pv, *shortPressedNum, *longPressedNum);

    /// 
    if (boolCast(GPIO.in & __mask32(BTN0))) {
        // __log("[changeModeButtonISR] INPUT LEVEL: HIGH");
        lastPosEdgeTime = timeNow;
    } else {
        // __log("[changeModeButtonISR] INPUT LEVEL : LOW");
        
        /// Check for short press (e.g. < 500ms)
        // __log("[changeModeButtonISR] Time delta = %ld", timeNow - lastPosEdgeTime);

        if ( (timeNow - lastPosEdgeTime) <= SHORTPRESS_TIME) {
            // __log("[changeModeButtonISR] Is short pressed!");
            (*shortPressedNum)++;
        } else if ( (timeNow - lastPosEdgeTime) >= LONGPRESS_TIME) {
            // __log("[changeModeButtonISR] Is long pressed!");
            (*longPressedNum)++;
        } else {
            // __log("[changeModeButtonISR] Is normal pressed!");
            (*normPressedNum)++;
        }

        lastNegEdgeTime = timeNow;
    }

    // __exit("changeModeButtonISR(%p, %ld, %ld)\n", pv, *shortPressedNum, *longPressedNum);

}

/// @brief Do change mode
/// @return Default return status.
def modeSwitch(){
    __entry("modeSwitch(%d --> %d)", currentSystemMode, systemMode);

    def ret = 0;

    if(currentSystemMode != systemMode){
        /// Exit/Clear current mode
        switch (currentSystemMode){
            case SYSTEM_MODE_SPI:            
                __log("EXIT MODE: SYSTEM_MODE_SPI");
                ret = destroySPIDevice((spiDev_t**)&comObject);
                if(ret!=OKE) goto modeSwitch_ReturnERR;
            break;
            
            case SYSTEM_MODE_I2C:
                __log("EXIT MODE: SYSTEM_MODE_I2C");
                ret = destroyI2CDevice((i2cDev_t*)comObject);
                if(ret!=OKE) goto modeSwitch_ReturnERR;
            break;
            
            case SYSTEM_MODE_UART:
                __log("EXIT MODE: SYSTEM_MODE_UART");
            break;
            
            case SYSTEM_MODE_1WIRE:
                __log("EXIT MODE: SYSTEM_MODE_1WIRE");
            break;
            
            default:
                __err("EXIT MODE: systemMode is not valid! --> Abort!");
                break;
        }

        /// Set-up new mode
        switch (systemMode){
            case SYSTEM_MODE_SPI:            
                __log("ENTRY MODE: SYSTEM_MODE_SPI");
                spiDev_t * spi = NULL;

                ret = createSPIDevice(&spi);
                if(ret!=OKE) goto modeSwitch_ReturnERR;
                
                ret = configSPIDevice(spi, PIN0, PIN1, PIN2, PIN3, COM_SPI_FREQ, COM_SPI_CONF);
                if(ret!=OKE) goto modeSwitch_ReturnERR;

                ret = startupSPIDevice(spi);
                if(ret!=OKE) goto modeSwitch_ReturnERR;

                comObject = (void*) spi;
            break;
            
            case SYSTEM_MODE_I2C:
                __log("ENTRY: SYSTEM_MODE_I2C");

                i2cDev_t * i2c = NULL;

                ret = createI2CDevice(&i2c);
                if(ret!=OKE) goto modeSwitch_ReturnERR;
                
                ret = configI2CDevice(i2c, 0, PIN0, PIN1, I2C_SLAVE_SPEEDMODE0);
                if(ret!=OKE) goto modeSwitch_ReturnERR;

                ret = startupI2CDevice(i2c);
                if(ret!=OKE) goto modeSwitch_ReturnERR;

                comObject = (void*) i2c;

            break;
            
            case SYSTEM_MODE_UART:
                __log("ENTRY: SYSTEM_MODE_UART");
            break;
            
            case SYSTEM_MODE_1WIRE:
                __log("ENTRY: SYSTEM_MODE_1WIRE");
            break;
            
            default:
                __err("systemMode is not valid! Reset to STARTUP_COM_MODE");
                systemMode = STARTUP_COM_MODE;
                goto modeSwitch_ReturnERR;
                break;
        }

        /// Record new mode
        currentSystemMode = systemMode;
    }
        
    __exit("modeControl() : OKE");
    return OKE;

    modeSwitch_ReturnERR:
        __exit("modeSwitch() : ERR");
        return ERR;
}

/// @brief Track and call modeSwitch() to change mode.
/// @param pv As required from. Pls pass NULL ptr!
void modeControlTask(void * pv){
    
    /// Wait for the initializing done
    __log("[modeControlTask] Wait for the initializing have been done!");
    for( ; systemStage == SYSTEM_INIT; vTaskDelay(1));

    __entry("modeControlTask(%p)", pv);
    
    /// param[0] = number of short pressed (<= SHORTPRESS_TIME)
    /// param[1] = number of long  pressed (>  SHORTPRESS_TIME)
    int32_t pressedRecord[3] = {0, 0, 0};

    int32_t* shortPressedNum = pressedRecord;
    int32_t* normPressedNum  = pressedRecord+ 1;
    int32_t* longPressedNum  = pressedRecord + 2;

    /// Input + Int
    gpio_config_t inPin = {
        .intr_type      = GPIO_INTR_ANYEDGE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   = __mask32(BTN0),
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_DISABLE
    };
    gpio_config(&inPin);

    /// Enable all interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(BTN0, changeModeButtonISR, pressedRecord);

    __log("[modeControlTask] Default mode: %d", systemMode);
    if(modeSwitch() != OKE) __err("[modeControlTask] Can not switch mode!");

    while( systemStage != SYSTEM_STOPPED ){
        if((*longPressedNum) > 0 ){

            __log("Detected %d shorts pressed --> Changed mode!", (*longPressedNum));
            /// Change comunication mode
            systemMode = (systemMode + 1) % SYSTEM_MODE_COUNT;

            /// Reset all press count
            (*shortPressedNum) = 0;
            (*normPressedNum) = 0;
            (*longPressedNum) = 0;
            
            /// Call function to switch mode
            if(modeSwitch() != OKE) __err("[modeControlTask] Can not switch mode!");

        }
        vTaskDelay(1);
    }

    __exit("modeControlTask()");
}

/// OTHERS ////////////////////////////////////////////////////////////////////////////////////////
