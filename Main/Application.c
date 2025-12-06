
#include "Application.h"


void AppInitialize(){
    SysEntry("AppInitialize()");
    
    SysLog("[AppInitialize] [+Task] TaskScreen");
    CreateTaskCPU0(TaskScreen, "TaskScreen", 4096, NULL, 2, NULL);

    SysExit("AppInitialize()");
}

/// @brief Main entry point for the ESP-IDF application.
/// @details This function is called by the ESP-IDF startup code. Its primary role is to
///          initialize the application by creating tasks and then delete itself to free up memory.
void app_main(void){
    SysEntry("app_main() : User SW entry point! ");
    // Call the main initialization routine which sets up all application tasks.
    AppInitialize();

    // The main task has completed its setup. Delete it to free up its stack memory.
    vTaskDelete(NULL);
}
