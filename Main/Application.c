
#include "Application.h"

uint32_t volatile SYSTEM_STAGE = SYSTEM_INIT_N(0);

void app_main(void){
    SysEntry("app_main() : User SW entry point! ");
    
    SET_SYSTEM_INIT_N(0);
    AppInitialize();
    SET_SYSTEM_INIT_N(1);

    INIT_DO_WAIT(vTaskDelay(1));
    SET_SYSTEM_RUNNING();
    vTaskDelete(NULL);
}
