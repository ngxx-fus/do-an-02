#include "../helper/general.h"
#include "../comDriver/lcd32/lcd32.h"

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


/// LCD 3.2" //////////////////////////////////////////////////////////////////////////////////////

lcd32Dev_t * lcd;

/// TASK //////////////////////////////////////////////////////////////////////////////////////////

void lcdTestTask(void * pv){
    __entry("lcdTestTask()");

    while(!IS_SYSTEM_STOPPED){
        vTaskDelay(1);
    }
    __exit("lcdTestTask()");
}

/// INIT //////////////////////////////////////////////////////////////////////////////////////////

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
    lcd32FillCanvas(lcd, 134);
    lcd32FlushCanvas(lcd);

    __exit("lcdInit()");
}

void systemInit(){
    __entry("systemInit()");
    lcdInit();

    __tag_log(STR(app_main), "[+] lcdTestTask()");
    xTaskCreate(lcdTestTask, "lcdTestTask", 2048, NULL, 7, NULL);

    __exit("systemInit()");
}

