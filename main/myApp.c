#include "../config/generalConfig.h"

#if ESP32_DEVICE_MODE == SENDER
    #include "sender.h"
#endif
#if ESP32_DEVICE_MODE == RECEIVER
    #include "receiver.h"
#endif
#if ESP32_DEVICE_MODE == MONITOR
    #include "monitor.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////


void app_main(void){
    __entry("app_main()");

    __tag_log(STR(app_main), "Set systemStage = SYSTEM_INIT");
    systemStage = SYSTEM_INIT;

    systemInit();

    __tag_log(STR(app_main), "Set systemStage = SYSTEM_RUNNING");
    systemStage = SYSTEM_RUNNING;
    
    lcdDrawCaro(); 

    while (!IS_SYSTEM_STOPPED){
        
        vTaskDelay(1);
    }
    
    __exit("app_main()");
}
