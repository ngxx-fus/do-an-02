#include "../helper/general.h"

#if ESP32_DEVICE_MODE == SENDER

#include "../comDriver/spi/spi.h"
#include "../comDriver/oled128x64/oled128x64.h"

/// DEFINITIONS ///////////////////////////////////////////////////////////////////////////////////

/// Enumeration defining the system operation stages.
enum ENUM_SYSTEM_STAGE {
    SYSTEM_INIT    = 0, /// System is initializing.
    SYSTEM_RUNNING = 1, /// System is currently running.
    SYSTEM_STOPPED = 2, /// System has been stopped or halted.
    SYSTEM_STAGE_COUNT,
};

/// Global system stage flag used to represent the overall runtime state.
/// Access to this variable must be protected by `systemStageMutex`.
volatile flag_t systemStage = SYSTEM_INIT;

/// Mutex for synchronizing access to the system stage flag.
portMUX_TYPE systemStageMutex = portMUX_INITIALIZER_UNLOCKED;

/// Enumeration defining OLED operation request types.
enum ENUM_OLED_FLAG {
    OLED_REQ_FLUSH       = 0, /// Request to flush (update) the OLED display.
    OLED_REQ_CLR_CANVAS  = 1, /// Request to clear the OLED display canvas.
    OLED_REQ_COUNT,
};

/// OLED control flag for requesting display operations.
/// Access to this variable must be protected by `oledFlagMutex`.
volatile flag_t oledFlag = 0;

/// Mutex for synchronizing access to the OLED flag.
portMUX_TYPE oledFlagMutex = portMUX_INITIALIZER_UNLOCKED;

/// Enumeration defining supported system communication modes.
enum ENUM_SYSTEM_MODE {
    SYSTEM_MODE_SPI    = 0, /// SPI communication mode.
    SYSTEM_MODE_I2C    = 1, /// I2C communication mode.
    SYSTEM_MODE_UART   = 2, /// UART communication mode
    SYSTEM_MODE_1WIRE  = 3, /// 1-Wire communication mode.
    SYSTEM_MODE_COUNT       /// Number of system modes
};

/// Global system mode flag representing active communication mode.
/// Access to this variable must be protected by `systemModeMutex`.
volatile flag_t systemMode = SYSTEM_MODE_SPI;

/// Current protocol mode
volatile flag_t currentSystemMode = -1;

/// Mutex for synchronizing access to the system mode flag.
portMUX_TYPE systemModeMutex = portMUX_INITIALIZER_UNLOCKED;


/// Communication object
/// It can be assigned as I2C, SPI, ...
void* comObject = NULL;

/// Contain 8-bit data frame 
volatile uint8_t byteData = '?';

/// Random 8-bit char (printable)
volatile uint8_t receivedByteData = '?';

/// Mutex for synchronizing access to the sendControlMutex.
portMUX_TYPE     sendControlMutex;

/// Send control flag
volatile flag_t  byteDataGenCtrlFlag;

/// Enumeration defining SEND operation request types.
enum BYTE_DATA_CONTROL_FLAG {
    BDCTRL_PLAYPAUSE      = 0,        /// 0: Play / 1: Pause
    BDCTRL_SEND_NOW       = 1,        /// 1: Immediately send data frame
    BDCTRL_REPEAT_SEND    = 2,        /// 0: Random / 1: Stop random data frame
    BYTE_DATA_CONTROL_FLAG_NUM
};

/// LOCAL HELPER //////////////////////////////////////////////////////////////////////////////////

#define IS_SYSTEM_INIT          (systemStage == SYSTEM_INIT)
#define IS_SYSTEM_RUNNING       (systemStage == SYSTEM_RUNNING)
#define IS_SYSTEM_STOPPED       (systemStage == SYSTEM_STOPPED)

/// Flag operation with MUTEX! 
#define FLAG_OP_W_MUTEX(p2mutex, flagOp, flag, bitOrder)        \
        do {                                                    \
            vPortEnterCritical(p2mutex);                        \
            flagOp(flag, bitOrder);                             \
            vPortExitCritical(p2mutex);                         \
        } while (0)

/// Wrap a piece of code in a MUTEX!
#define DO_WITH_MUTEX(p2mutex, anythingYouWantToDo)             \
        do {                                                    \
            vPortEnterCritical(p2mutex);                        \
            anythingYouWantToDo;                                \
            vPortExitCritical(p2mutex);                         \
        } while (0)


/// PREPAIR AND SEND DATA TASK ////////////////////////////////////////////////////////////////////

def setTransmitBuff(uint8_t* byteArr, size_t size){
    __entry("setTransmitBuff(%p, %d)", byteArr, size);

    def ret = OKE;

    switch (currentSystemMode){
        case SYSTEM_MODE_SPI:            
            ret = spiSetTransmitBuffer((spiDev_t*)comObject, byteArr, size);
            goto ReturnRET;
        
        case SYSTEM_MODE_I2C:
            __tag_log(STR(setTransmitBuff), "Mode I2C not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        
        case SYSTEM_MODE_UART:
            __tag_log(STR(setTransmitBuff), "Mode UART not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        
        case SYSTEM_MODE_1WIRE:
            __tag_log(STR(setTransmitBuff), "Mode 1WIRE not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        
        default:
            __err("EXIT MODE: systemMode is not valid! --> Abort!");
            ret = ERR_NOT_FOUND; goto ReturnRET;
    }

    ReturnRET:
        __exit("setTransmitBuff() : %s", getDefRetStat_Str(ret));
        return ret;

}

def setReceiveBuff(uint8_t* byteArr, size_t size){
    __entry("setReceiveBuff(%p, %d)", byteArr, size);

    def ret = OKE;

    switch (currentSystemMode){
        case SYSTEM_MODE_SPI:            
            ret = spiSetReceiveBuffer((spiDev_t*)comObject, byteArr, size);
            goto ReturnRET;
        break;
        
        case SYSTEM_MODE_I2C:
            __tag_log(STR(setReceiveBuff), "Mode I2C not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        break;
        
        case SYSTEM_MODE_UART:
            __tag_log(STR(setReceiveBuff), "Mode UART not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        break;
        
        case SYSTEM_MODE_1WIRE:
            __tag_log(STR(setReceiveBuff), "Mode 1WIRE not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        break;
        
        default:
            __err("EXIT MODE: systemMode is not valid! --> Abort!");
            ret = ERR_NOT_FOUND; goto ReturnRET;
    }

    ReturnRET:
        __exit("setReceiveBuff() : %s", getDefRetStat_Str(ret));
        return ret;

}

def sendDataBuff(){
    __entry("sendDataBuff()");

    def ret = OKE;

    switch (currentSystemMode){
        case SYSTEM_MODE_SPI:            
            ret = spiStartTransaction((spiDev_t*)comObject);
            goto ReturnRET;
        
        case SYSTEM_MODE_I2C:
            __tag_log(STR(sendDataBuff), "Mode I2C not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        
        case SYSTEM_MODE_UART:
            __tag_log(STR(sendDataBuff), "Mode UART not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        
        case SYSTEM_MODE_1WIRE:
            __tag_log(STR(sendDataBuff), "Mode 1WIRE not found!");
            /// TODO: Add mode!
            ret = ERR_NOT_FOUND; goto ReturnRET;
        
        default:
            __err("EXIT MODE: systemMode is not valid! --> Abort!");
            ret = ERR_NOT_FOUND; goto ReturnRET;
    }

    ReturnRET:
        __exit("sendDataBuff() : %s", getDefRetStat_Str(ret));
        return ret;
}


def hasNewData(){

    return OKE;
}

/// @brief Random printable character
/// @param pv As required from. Pls pass NULL ptr!
void generateByteDataTask(void *pv){    
    /// Wait for the initializing done
    __tag_log(STR(generateByteDataTask), "Wait for the initializing have been done!");
    while(IS_SYSTEM_INIT) vTaskDelay(1);

    __entry("generateByteDataTask(%p)", pv);

    while (! IS_SYSTEM_STOPPED){
        if(__hasFlagBitSet(byteDataGenCtrlFlag, BDCTRL_REPEAT_SEND)) {
            vTaskDelay(1);
        }else{
            byteData = ((generateRandom(0)) % (126-32+1)) + 32;
            __tag_log1(STR(generateByteDataTask), "Generated : %c", byteData);
            vTaskDelay(DATAFR_INTERVAL);
        }
    }
    
    __exit("generateByteDataTask()");
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
        __log("[changeModeButtonISR] INPUT LEVEL : LOW");
        
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
        /// Clear current mode
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
                
                ret = configI2CDevice(i2c, 0, PIN0, PIN1, I2C_MASTER_SPEEDMODE0);
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
    __tag_log("modeControlTask", "Wait for the initializing have been done!");
    for( ; IS_SYSTEM_INIT; vTaskDelay(1));

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

    while( ! IS_SYSTEM_STOPPED ){
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




#endif