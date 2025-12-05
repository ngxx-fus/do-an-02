
#define PRINT_HEADER_COMPILE_MESSAGE
#include "Application.h"


void app_main(void){
    SysEntry("app_main() - User SW entry point");

    while (1){
        SysLog("[app_main] Running...");
        vTaskDelay(100);
    }
}
