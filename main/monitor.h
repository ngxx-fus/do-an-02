/// @file   monitor.h
/// @brief  Main project! For monitor/analyzer the bus!

#ifndef __MONITOR_H__
#define __MONITOR_H__

#include "../helper/general.h"
#include "../comDriver/lcd32/lcd32.h"

/// @cond
/// DEFINITIONS ///////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Enumeration defining the system operation stages.
enum ENUM_SYSTEM_STAGE {
    SYSTEM_INIT    = 0, ///< System is initializing.
    SYSTEM_RUNNING = 1, ///< System is currently running.
    SYSTEM_STOPPED = 2, ///< System has been stopped or halted.
    SYSTEM_STAGE_COUNT, ///< Total number of defined stages.
};

/// @brief Global system stage flag used to represent the overall runtime state.
/// @note Access to this variable must be protected by `systemStageMutex`.
volatile flag_t systemStage = SYSTEM_INIT;

/// @brief Mutex for synchronizing access to the system stage flag.
portMUX_TYPE systemStageMutex = portMUX_INITIALIZER_UNLOCKED;

/// @cond
/// LOCAL HELPER //////////////////////////////////////////////////////////////////////////////////
/// @endcond

#define IS_SYSTEM_INIT          (systemStage == SYSTEM_INIT)    ///< Check if system is in INIT stage.
#define IS_SYSTEM_RUNNING       (systemStage == SYSTEM_RUNNING) ///< Check if system is in RUNNING stage.
#define IS_SYSTEM_STOPPED       (systemStage == SYSTEM_STOPPED) ///< Check if system is in STOPPED stage.

/// @brief Perform a flag operation safely within a critical section (Mutex).
/// @param p2mutex Pointer to the mutex.
/// @param flagOp  Flag operation macro/function (e.g., __setFlagBit).
/// @param flag    The flag variable to modify.
/// @param bitOrder The bit position to operate on.
#define FLAG_OP_W_MUTEX(p2mutex, flagOp, flag, bitOrder)        \
        do {                                                    \
            vPortEnterCritical(p2mutex);                        \
            flagOp(flag, bitOrder);                             \
            vPortExitCritical(p2mutex);                         \
        } while (0)

/// @brief Execute a block of code safely within a critical section (Mutex).
/// @param p2mutex Pointer to the mutex.
/// @param anythingYouWantToDo Code block or function call to execute.
#define DO_WITH_MUTEX(p2mutex, anythingYouWantToDo)             \
        do {                                                    \
            vPortEnterCritical(p2mutex);                        \
            anythingYouWantToDo;                                \
            vPortExitCritical(p2mutex);                         \
        } while (0)


/// @cond
/// LCD 3.2" //////////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Pointer to the LCD 3.2" device driver instance.
lcd32Dev_t * lcd;

/// @cond
/// TASK //////////////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Task to test LCD rendering by filling random colors.
/// @param pv FreeRTOS task parameter (unused).
void lcdTestTask(void * pv){
    __entry("lcdTestTask()");

    uint64_t all_pins_mask = lcd->dataPinMask | lcd->controlPinMask;

    __sys_log("[lcdTestTask] all_pins_mask = 0x%08x", all_pins_mask);

    const uint32_t mask_low  = (uint32_t)(all_pins_mask);
    const uint32_t mask_high = (uint32_t)(all_pins_mask >> 32);

    uint16_t mask = 1;

    while(!IS_SYSTEM_STOPPED){
        
        lcd32FillCanvas(lcd, genRandNum(esp_timer_get_time())%65536);
        lcd32FlushCanvas(lcd);

        // vTaskDelay(50);
        __sys_log("[lcdTestTask] Running...\n\n");
    }
    __exit("lcdTestTask()");
}

/// @cond
/// INIT //////////////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief Initialize the LCD device, configure pins, and perform startup sequence.
void lcdInit(){
    __entry("lcdInit()");
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
        .rst=LCD32_RST
    };
    lcd32ConfigDevice(lcd, &dataPin, &ctlPin, LCD32_MAX_ROW, LCD32_MAX_COL);
    lcd32StartUpDevice(lcd);
    lcd32FillCanvas(lcd, 15);
    lcd32FlushCanvas(lcd);

    __exit("lcdInit()");
}

/// @brief Initialize the entire system, including drivers and tasks.
void systemInit(){
    __entry("systemInit()");
    lcdInit();

    __tag_log(STR(app_main), "[+] lcdTestTask()");
    xTaskCreate(lcdTestTask, "lcdTestTask", 2048, NULL, 7, NULL);

    __exit("systemInit()");
}

#endif