
#include "Application.h"



void app_main(void){
    SysEntry("app_main() - User SW entry point");
    CreateTaskCPU0(Log1, "Log1", 108, NULL, 0, NULL);
    while (1){
        SysLog("[app_main] Running...");
        DelayMs(100);
    }
}
