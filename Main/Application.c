
#include "Application.h"

void app_main(void){
    SysEntry("app_main() : User SW entry point! ");
    // Call the main initialization routine which sets up all application tasks.
    AppInitialize();

    // The main task has completed its setup. Delete it to free up its stack memory.
    vTaskDelete(NULL);
}
