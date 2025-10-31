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
    
    int64_t startTime = 0;
    int32_t currentModeRepeat = 0;
    
    char buff[64];

    while (!IS_SYSTEM_STOPPED){
        startTime = esp_timer_get_time();
        __sys_log("[main_app] running...");    


        switch (systemMode){
        case SYSTEM_MODE_DEMO_I2C_CAP:
            lcd32FillCanvas(lcd, 0xFFFF);
            oscDemo();
            __setFlagBit(screenFlag, SCREEN_RENDER);
            break;
        
        default:
            lcd32FillCanvas(lcd, 0xFFFF);
            sprintf(buff, "This mode \n%s\nis not available now! (%ld)", SYSTEM_MODE_STR[systemMode], currentModeRepeat);
            lcd32DrawText(lcd, 50, 5, buff, &fontHeading03,COLOR_ERROR);
            __setFlagBit(screenFlag, SCREEN_RENDER);
            break;
        }

        if(++currentModeRepeat > 10){
            systemMode = (systemMode+1)%SYSTEM_MODE_COUNT;
            currentModeRepeat = 0;
        }

        __sys_log("[main_app] sleeping...");    
        while(esp_timer_get_time() - startTime < 800U*1000U) portYIELD();/// pdMS_TO_TICKS(500)
    }
    
    __exit("app_main()");
}
