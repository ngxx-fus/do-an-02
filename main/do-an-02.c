#if ESP32_DEVICE_MODE == SENDER
    #include "sender.h"
#endif
#if ESP32_DEVICE_MODE == RECEIVER
    #include "receiver.h"
#endif

void app_main(void){
    __entry("app_main()");

    __log("[+] ledTest()");
    ledTest(NULL);
    
    __log("[+] oledTest()");
    xTaskCreate(oledTest, "oledTest", 2048, NULL, 7, NULL);
    
    // __log("[+] i2cTest()");
    // xTaskCreate(i2cTest, "i2cTest", 2048, NULL, 7, NULL);

    while (1){
        vTaskDelay(100);
    }
    

    __exit("app_main()");
}
