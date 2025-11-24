/// @file   myApp.c
/// @brief  Main application entry point; Initializes system based on configured mode (Sender/Receiver/Monitor) and manages main loop.

#include "../config/projectConfig.h"

#if ESP32_DEVICE_MODE == SENDER
    #include "sender.h"
#endif
#if ESP32_DEVICE_MODE == RECEIVER
    #include "receiver.h"
#endif
#if ESP32_DEVICE_MODE == MONITOR
    #include "monitor.h"
#endif


/// @cond
///////////////////////////////////////////////////////////////////////////////////////////////////
/// @endcond

/// @brief  Main application entry point.
/// @details Initializes system state, triggers system setup, and enters the main execution loop.
void app_main(void){
    __entry("app_main()");

    __tag_log(STR(app_main), "Set systemStage = SYSTEM_INIT");
    systemStage = SYSTEM_INIT;    ///< Set system global state to Initialization.

    systemInit();                 ///< Perform system-wide initialization (Drivers, GPIO, Tasks).

    __tag_log(STR(app_main), "Set systemStage = SYSTEM_RUNNING");
    systemStage = SYSTEM_RUNNING; ///< Set system global state to Running.
    
    /// Main loop: Keep the task alive and yield to scheduler until system is stopped.
    while (!IS_SYSTEM_STOPPED){

        vTaskDelay(1);
    }
    
    __exit("app_main()");
}