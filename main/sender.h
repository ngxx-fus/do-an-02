#include "../helper/general.h"

#include "../comDriver/spi/spi.h"
#include "../comDriver/oled128x64/oled128x64.h"

/// LOCAL /////////////////////////////////////////////////////////////////////////////////////////


/// TEXT //////////////////////////////////////////////////////////////////////////////////////////

// /// @brief Clear canvas then draw new text then show. 
// /// @param oled 
// /// @param line 
// /// @param text 
// void drawLineText(oled128x64Dev_t * oled, uint8_t line, const char * text, uint8_t lineH){
//     oledResetView(oled);
//     oledShowRAMContent(oled);
//     oledFillScreen(oled, 0);
//     oledDrawText(oled, (line+1)*lineH + line*2, 0, text, 1, 1, lineH);
//     oledFlush(oled);
// }

/// LED TEST MODE /////////////////////////////////////////////////////////////////////////////////

void IRAM_ATTR buttonISR(void* pv){
    int64_t* testModeVariable = (int64_t*) pv;
    int64_t entryButtonISR =  esp_timer_get_time();
    if((GPIO.in & __mask32(BTN0)) == LOW){
        __log("Button is pressed!");
        __log("entryButtonISR = %ld", entryButtonISR);
        if(entryButtonISR - testModeVariable[1] < 100000){
            __log("testModeVariable[1] = %ld", testModeVariable[1]);
            __log("Button is pressed lesthan than 100000us, break test mode!");
            testModeVariable[0] = 0;
        }
        testModeVariable[1] = entryButtonISR;
    }else{
        __log("Button is released!");
    }
}

void ledTest(void *pv){
    __entry("ledTest()");
    
    /// Output
    gpio_config_t outPin = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = __mask32(PIN0) | __mask32(PIN1) |  
                        __mask32(PIN2) | __mask32(PIN3),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&outPin);

    /// Input + Int
    gpio_config_t inPin = {
        .intr_type      = GPIO_INTR_ANYEDGE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   = __mask32(BTN0),
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_DISABLE
    };
    gpio_config(&inPin);

    /// Status variable 
    /// testModeVariable[0] : Control the test state, Set 0 to stop test mode
    /// testModeVariable[1] : Save last pressed
    int64_t testModeVariable[2] = {2, 0};

    /// Enable all interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(BTN0, buttonISR, (void*) testModeVariable);

    /// Running test mode
    while(testModeVariable[0]-- != 0){
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1ts = (__mask32(PIN0));
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1ts =  __mask32(PIN1);
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1ts = __mask32(PIN2);
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1ts = __mask32(PIN3);
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1tc = (__mask32(PIN0));
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1tc =  __mask32(PIN1);
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1tc = __mask32(PIN2);
        vTaskDelay(50/portTICK_PERIOD_MS);
        GPIO.out_w1tc = __mask32(PIN3);
    };

    gpio_uninstall_isr_service();
    gpio_reset_pin(PIN0);
    gpio_reset_pin(PIN1);
    gpio_reset_pin(PIN2);
    gpio_reset_pin(PIN3);

    __exit("ledTest()");
}

/// OLED TASK /////////////////////////////////////////////////////////////////////////////////////

void oledTask(void *pv){

    __log("[oledTask] Wait for the initializing have been done!");
    /// Wait for the initializing done
    for( ; systemStage == SYSTEM_INIT; vTaskDelay(1));

    __entry("oledTest()");

    oled128x64Dev_t *oled = (oled128x64Dev_t*)pv;

    while (systemStage != SYSTEM_STOPPED){
        if(__hasFlagBitSet(oledFlag, OLED_REQ_FLUSH)){
            // __log("[oledTask] Receive flush request!");

            // __log("[oledTask] Clear flush request!");
            vPortEnterCritical(&oledFlagMutex);
            __clearFlagBit(oledFlag, OLED_REQ_FLUSH);
            vPortExitCritical(&oledFlagMutex);

            // __log("[oledTask] Oled flush ...");
            oledFlush(oled);
        }
        vTaskDelay(1);
    }
    
    __exit("oledTest()");
}

/// PREPAIR AND SEND DATA TASK ////////////////////////////////////////////////////////////////////

/// @brief Send data frame with corresponding com descriptor
/// @return Default return state
def sendDataFrame(uint8_t byteData, uint8_t * rcvdByteData){
    __entry("sendDataFrame(%c, %p)", byteData, rcvdByteData);
    
    def ret;
    
    switch (currentSystemMode){
        case SYSTEM_MODE_SPI:            
            __log("SPI Send: %c", byteData);

            spiDev_t * spi = (spiDev_t *) comObject;

            ret = spiSetTransmitBuffer(spi, &byteData, 1);
            if(ret!=OKE) goto sendDataFrame_ReturnERR;

            ret = spiSetReceiveBuffer(spi, rcvdByteData, 1);
            if(ret!=OKE) goto sendDataFrame_ReturnERR;

            ret = spiStartTransaction(spi);
            if(ret!=OKE) goto sendDataFrame_ReturnERR;

        break;
        
        case SYSTEM_MODE_I2C:
            __log("I2C Send: %c", byteData);
            __log("I2C not available!");
        break;
        
        case SYSTEM_MODE_UART:
            __log("UART Send: %c", byteData);
            __log("UART not available!");
        break;
        
        case SYSTEM_MODE_1WIRE:
            __log("1-WIRE Send: %c", byteData);
            __log("UART not available!");
        break;
        
        default:
            __err("MODE: systemMode is not valid! --> Abort!");
            ret = ERR;
            goto sendDataFrame_ReturnERR;
    }
    

    __exit("sendDataFrame()");
    return OKE;

    sendDataFrame_ReturnERR:
        __exit("sendDataFrame() : [%d]", ret);
        return ret;
}

/// @brief Random printable character
/// @param pv As required from. Pls pass NULL ptr!
void prepairNSendDataTask(void *pv){    
    /// Wait for the initializing done
    __log("[prepairNSendDataTask] Wait for the initializing have been done!");
    for( ; systemStage == SYSTEM_INIT; vTaskDelay(1));

    __entry("prepairNSendDataTask(%p)", pv);

    while (systemStage != SYSTEM_STOPPED){
        if(__hasFlagBitSet(sendControlFlag, SENDCTRL_REPEAT_SEND)) {
            vTaskDelay(1);
        }else{
            sendByteData = ((generateRandom(0)) % (126-32+1)) + 32;
            // __log("[prepairNSendDataTask] Generated : %c", sendByteData);
            vTaskDelay(DATAFR_INTERVAL);
        }
    }
    
    __exit("prepairNSendDataTask()");
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
                
                ret = configSPIDevice(spi, PIN0, PIN1, PIN2, PIN3, COM_SPI_FREQ, SPI_10_MASTER);
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
