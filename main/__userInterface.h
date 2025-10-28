#ifndef __userInterface_H__
#define __userInterface_H__

#include "../helper/general.h"
#include "../intercon/lcd32/lcd32.h"

/// GLOBAL VARIABLES /////////////////////////////////////////////////////////////////////////////

/// @brief  Pointer to the LCD 3.2" device structure
lcd32Dev_t * lcd;
/// @brief  Global screen flag for controlling screen operations
volatile flag_t screenFlag = SYSTEM_INIT;
/// @brief Mutex for synchronizing access to the screen flag
portMUX_TYPE screenFlagMutex = portMUX_INITIALIZER_UNLOCKED;
/// @brief Enumeration defining the bit order for screen flags
enum SCREEN_FLAG_BIT_ORDER {
    SCREEN_RENDER = 0,                          /// 0: Ignore,      1: Render
    SCREEN_TURN_ON = 0,                         /// 0: Ignore,      1: ON
    SCREEN_TURN_OFF = 0,                        /// 0: Ignore,      1: OFF
    SCREEN_FLAG_BIT_ORDER_NUMBER,
};

/// FUNCTION DEFINITIONS //////////////////////////////////////////////////////////////////////////

/// @brief Initializes the user interface components, including the LCD device.
void userInterfaceInitilize();

/// @brief Displays the introductory screen with author information.
void drawAuthorInfo();

/// @brief Draws a Caro (Gomoku) grid on the LCD screen. (By defautt 10x10)
void drawCaro();

/// @brief Task function for testing the LCD functionality.
void lcdTestTask0(void * pv);

/// @brief Pooling task to control screen operations based on the screenFlag.
/// @param pv Unused parameter.
void screenControlTask(void * pv);

/// FUNCTION DECLARATIONS /////////////////////////////////////////////////////////////////////////

void userInterfaceInitilize(){
    __entry("__userInterfaceInitilize()");
    lcd32CreateDevice(&lcd);
    lcd32DataPin_t dataPin = {
        .__0 = LCD32_DB0,   .__1 = LCD32_DB1,   .__2 = LCD32_DB2,   .__3 = LCD32_DB3,
        .__4 = LCD32_DB4,   .__5 = LCD32_DB5,   .__6 = LCD32_DB6,   .__7 = LCD32_DB7,
        .__8 = LCD32_DB8,   .__9 = LCD32_DB9,   .__10 = LCD32_DB10, .__11 = LCD32_DB11,
        .__12 = LCD32_DB12, .__13 = LCD32_DB13, .__14 = LCD32_DB14, .__15 = LCD32_DB15
    };
    lcd32ControlPin_t ctlPin = {
        .r = LCD32_RD,  .w = LCD32_WR,
        .rs= LCD32_RS,  .cs= LCD32_CS,
        .rst=LCD32_RST, .bl= LCD32_BL
    };
    lcd32ConfigDevice(lcd, &dataPin, &ctlPin, LCD32_MAX_ROW, LCD32_MAX_COL);
    lcd32StartUpDevice(lcd);

    lcd32FillCanvas(lcd, 0xFFFF);

    drawAuthorInfo();

    vTaskDelay(pdMS_TO_TICKS(2000)); // Display for 2 seconds

    __exit("__userInterfaceInitilize()");
}

void drawAuthorInfo(){
    __entry("lcdShowIntroScreen()");

    lcd32FillCanvas(lcd, 0xFFFF); // Black background

    lcd32DrawText(lcd, 40, 5, "DO AN 02", &fontHeading01, 0x0152);
    lcd32DrawText(lcd, 80, 5, "[1] Nguyen Thanh Phu - 22119211", &fontHeading03, 0x2925);
    lcd32DrawText(lcd, 100, 5, "[2] Nguyen Van Quoc  - 22119222", &fontHeading03, 0x2925);

    __exit("lcdShowIntroScreen()");
}

void drawCaro() {
    color_t gridColor = 0x0000; 

    dim_t rowStep = LCD32_MAX_ROW / 10;
    dim_t colStep = LCD32_MAX_COL / 10;

    for (int i = 1; i < 10; i++) {
        dim_t current_row = i * rowStep;
        // Draw line across the full width (from column 0 to max column - 1)
        lcd32DrawLine(lcd, current_row, 0, current_row, LCD32_MAX_COL - 1, gridColor);
    }

    for (int i = 1; i < 10; i++) {
        dim_t current_col = i * colStep;
        // Draw line across the full height (from row 0 to max row - 1)
        lcd32DrawLine(lcd, 0, current_col, LCD32_MAX_ROW - 1, current_col, gridColor);
    }
    
}

void lcdTestTask0(void * pv){
    WAIT_SYSTEM_INIT_COMPLETED();

    __entry("lcdTestTask0()");

    uint64_t all_pins_mask = lcd->dataPinMask | lcd->controlPinMask;

    __sys_log("[lcdTestTask0] all_pins_mask = 0x%08x", all_pins_mask);

    const uint32_t mask_low  = (uint32_t)(all_pins_mask);
    const uint32_t mask_high = (uint32_t)(all_pins_mask >> 32);

    uint16_t mask = 1;

    while(!IS_SYSTEM_STOPPED){
        
        lcd32FillCanvas(lcd, genRandNum(esp_timer_get_time())%65536);

        __sys_log("[lcdTestTask0] defaultTextH(fontTitle) = %d", defaultTextH(fontTitle));

        lcd32DrawText(lcd, 50, 10, "HelloWorld!", &fontTitle, 0xFFFF);
        lcd32DrawLine(lcd, 50-defaultTextH(fontTitle), 10, 50-defaultTextH(fontTitle), 100, 0xFFFF);


        int64_t tStart = esp_timer_get_time();
        REP(i, 0, 50) {lcd32FlushCanvas(lcd); taskYIELD();}
        int64_t tStop = esp_timer_get_time();
        __sys_log("[lcdTestTask0] 50x Flush time: %lld us", (tStop - tStart));

        __sys_log("[lcdTestTask0] Sleep...\n\n");
        vTaskDelay(50);

        // lcd32DirectlyWritePixel(lcd, genRandNum(esp_timer_get_time())%LCD32_MAX_ROW, genRandNum(esp_timer_get_time())%LCD32_MAX_COL, genRandNum(esp_timer_get_time())%65536);
        // __lcd32SetLowData15Pin(lcd);
        // vTaskDelay(10);
        // __lcd32SetHighData15Pin(lcd);
        // vTaskDelay(10);
    }
    __exit("lcdTestTask0()");
}

void screenControlTask(void * pv){
    WAIT_SYSTEM_INIT_COMPLETED();
    __entry("screenControlTask()");
    
    flag_t localScreenFlag;
    while(!IS_SYSTEM_STOPPED){
        /// Get and clear screenFlag atomically (get one time only)
        PERFORM_IN_CRITICAL(&screenFlagMutex,
            localScreenFlag = screenFlag;
            screenFlag = 0;
        );

        /// Process screen operations based on the retrieved flags
        /// until all flags are handled. 
        while (__isnot_zero(localScreenFlag)){
            if(__hasFlagBitSet(localScreenFlag, SCREEN_RENDER)){
                __clearFlagBit(localScreenFlag, SCREEN_RENDER);
                lcd32FlushCanvas(lcd);
            }
            portYIELD(); // Yield to allow other tasks to run between operations
            if(__hasFlagBitSet(localScreenFlag, SCREEN_TURN_ON)){
                __clearFlagBit(localScreenFlag, SCREEN_TURN_ON);
                lcd32WakeFromSleep(lcd);
            }
            portYIELD(); // Yield to allow other tasks to run between operations
            if(__hasFlagBitSet(localScreenFlag, SCREEN_TURN_OFF)){
                __clearFlagBit(localScreenFlag, SCREEN_TURN_OFF);
                lcd32PutToSleep(lcd);
            }
            portYIELD(); // Yield to allow other tasks to run between operations
            if(__isnot_zero(localScreenFlag)){
                __sys_err("[screenControlTask] Warning: Unrecognized screenFlag bits: 0x%08x", localScreenFlag);
                localScreenFlag = 0;
            }
        }

        vTaskDelay(0);
    }

    __exit("screenControlTask()");
}

#endif