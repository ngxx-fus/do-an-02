
#include "Application.h"

uint32_t volatile SYSTEM_STAGE = SYSTEM_INIT_N(0);

void app_main(void){
    SysEntry("app_main() : User SW entry point! ");
    
    SET_SYSTEM_INIT_N(0);
    AppInitialize();
    SET_SYSTEM_INIT_N(1);

    REPN(i, 0x10){
        if(SYSTEM_STAGE < SYSTEM_INIT_N(i)){
            DelayMs(100);
            SET_SYSTEM_INIT_N(i);
            SysLog("[app_main] Auto move to init state (%X)!", i);
        }
    }
    SET_SYSTEM_RUNNING();
    vTaskDelete(NULL);
}


