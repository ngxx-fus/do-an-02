#include "../helper/general.h"

#if ESP32_DEVICE_MODE == SENDER
    #pragma message("ESP32_DEVICE_MODE: SENDER")
    #include "sender.h"
#endif

#if ESP32_DEVICE_MODE == RECEIVER
    #pragma message("ESP32_DEVICE_MODE: RECEIVER")
    #include "receiver.h"
#endif

/// MAIN APP //////////////////////////////////////////////////////////////////////////////////////

void app_main(void){
    __entry("app_main()");

    __log("Set systemStage = SYSTEM_INIT");
    systemStage = SYSTEM_INIT;
    
    __log("Start up OLED screen");
    oled128x64Dev_t *oled;
    createOLEDDevice(&oled);
    configOLEDDevice(oled, OLED_SCL, OLED_SDA, I2C_INFINITY_MODE);
    startupOLEDDevice(oled);
    drawLineText(oled, "Start up ...", 0xF00 | 0x00 );

    vTaskDelay(10);

    __log("[+] prepairByteDataTask()");
    drawLineText(oled, "[+] prepairByteDataTask()", 0xF00 | 0x00 );
    xTaskCreate(prepairByteDataTask, "prepairByteDataTask", 2048, NULL, 7, NULL);

    __log("[+] modeControlTask()");
    drawLineText(oled, "[+] modeControlTask()", 0xF00 | 0x00 );
    xTaskCreate(modeControlTask, "modeControlTask", 2048, NULL, 7, NULL);

    __log("Set systemStage = SYSTEM_RUNNING");
    systemStage = SYSTEM_RUNNING;

    char buff[64] = {};

    drawLineText(oled, "Running...", 0xF00 | 0x00 );

    uint8_t currentDataFrame = byteData;
    uint8_t receivedDataFrame = 0;

    vTaskDelay(10);

    setSendBackData(&currentDataFrame);
    setReceiveByteBuff(&receivedDataFrame, 1);

    while (systemStage != SYSTEM_STOPPED){

        oledFillScreen(oled, 0);

        /// Display mode and dataframe
        switch (currentSystemMode){
            case SYSTEM_MODE_SPI: snprintf(buff, sizeof(buff), "M: SPI"); break;
            case SYSTEM_MODE_I2C: snprintf(buff, sizeof(buff), "M: I2C"); break;
            case SYSTEM_MODE_UART:snprintf(buff, sizeof(buff), "M: UART");break;
            case SYSTEM_MODE_1WIRE:snprintf(buff, sizeof(buff), "M: 1-WIRE");break;
        }
        oledDrawText(oled, 20, 0, buff, 1, 1, 20);
        /// Display received byte data
        snprintf(buff, sizeof(buff), "> %02XH : %c", receivedDataFrame, receivedDataFrame);
        oledDrawText(oled, 40, 0, buff, 1, 1, 20);
        /// Display send back byte data
        snprintf(buff, sizeof(buff), "# %02XH : %c", currentDataFrame, currentDataFrame);
        oledDrawText(oled, 60, 0, buff, 1, 1, 20);

        oledFlush(oled);

        while(!checkNewData()) vTaskDelay(1);
        __log("[app_main] Has new data!");
        currentDataFrame = byteData;
        setSendBackData(&currentDataFrame);
        setReceiveByteBuff(&receivedDataFrame, 1);

        // vTaskDelay(10);
    }

    __exit("app_main()");
}
