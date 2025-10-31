#include "../helper/general.h"
#include "../comDriver/lcd32/lcd32.h"

/// DEFINITIONS ///////////////////////////////////////////////////////////////////////////////////

/// SYSTEM STAGE //////////////////////////////////////////////////////////////////////////////////

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
volatile portMUX_TYPE systemStageMutex = portMUX_INITIALIZER_UNLOCKED;

/// SYSTEM MODE ///////////////////////////////////////////////////////////////////////////////////

enum ENUM_SYSTEM_MODE {
    SYSTEM_MODE_DEMO_OSC_CAP        = 0, /// Sample analog signal
    SYSTEM_MODE_DEMO_SPI_CAP        = 1, /// Capture pakages on SPI bus
    SYSTEM_MODE_DEMO_I2C_CAP        = 2, /// Capture pakages on SPI bus
    SYSTEM_MODE_COUNT,
};


static const char* SYSTEM_MODE_STR[] = {
    "SYSTEM_MODE_DEMO_OSC_CAP",
    "SYSTEM_MODE_DEMO_SPI_CAP",
    "SYSTEM_MODE_DEMO_I2C_CAP",
};

volatile flag_t systemMode = SYSTEM_MODE_DEMO_OSC_CAP;


/// LOCAL HELPER //////////////////////////////////////////////////////////////////////////////////

#define IS_SYSTEM_INIT          (systemStage == SYSTEM_INIT)
#define IS_SYSTEM_RUNNING       (systemStage == SYSTEM_RUNNING)
#define IS_SYSTEM_STOPPED       (systemStage == SYSTEM_STOPPED)
#define ADD_NEW_TASK_TO_CPU0(tagName, taskFunc, taskName, stackSize, params, priority, taskHandle)  \
    __sys_log("[%s] [CPU0 +] %s", tagName, taskName);                                               \
    xTaskCreatePinnedToCore(taskFunc, taskName, stackSize, params, priority, taskHandle, 0)
    
#define ADD_NEW_TASK_TO_CPU1(tagName, taskFunc, taskName, stackSize, params, priority, taskHandle)  \
    __sys_log("[%s] [CPU1 +] %s", tagName, taskName);                                               \
    xTaskCreatePinnedToCore(taskFunc, taskName, stackSize, params, priority, taskHandle, 1)

#define WAIT_SYSTEM_INIT_COMPLETED() \
    while (IS_SYSTEM_INIT) { vTaskDelay(1); }

/// TASKS /////////////////////////////////////////////////////////////////////////////////////////

#include "ui.h"

/// INIT //////////////////////////////////////////////////////////////////////////////////////////

void systemInit(){
    __entry("systemInit()");

    lcdInit();

    lcdShowIntroScreen();

    // ADD_NEW_TASK_TO_CPU0("systemInit", lcdTestTask0, "lcdTestTask0", 2048, NULL, 7, NULL);
    ADD_NEW_TASK_TO_CPU1("systemInit", screenControlTask, "screenControlTask", 2048, NULL, 7, NULL);

    __exit("systemInit()");
}

