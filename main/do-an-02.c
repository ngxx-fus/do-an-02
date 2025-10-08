#if ESP32_DEVICE_MODE == SENDER
    #include "sender.h"
#endif
#if ESP32_DEVICE_MODE == RECEIVER
    #include "receiver.h"
#endif

void app_main(void){
    __entry("app_main()");

    __tag_log(STR(app_main), "Set systemStage = SYSTEM_INIT");
    systemStage = SYSTEM_INIT;
    
    __tag_log(STR(app_main), "Start up OLED screen");
    oled128x64Dev_t *oled;
    createOLEDDevice(&oled);
    configOLEDDevice(oled, OLED_SCL, OLED_SDA, I2C_INFINITY_MODE);
    startupOLEDDevice(oled);
    __tag_log(STR(app_main), "Start up...");
    drawLineText(oled, "Start up ...", 0xF00 | 0x00 );

    __tag_log(STR(app_main), "[+] generateByteDataTask()");
    drawLineText(oled, "[+] generateByteDataTask()", 0xF00 | 0x00 );
    xTaskCreate(generateByteDataTask, "generateByteDataTask", 2048, NULL, 7, NULL);

    __tag_log(STR(app_main), "[+] modeControlTask()");
    drawLineText(oled, "[+] modeControlTask()", 0xF00 | 0x00 );
    xTaskCreate(modeControlTask, "modeControlTask", 2048, NULL, 7, NULL);

    __tag_log(STR(app_main), "Set systemStage = SYSTEM_RUNNING");
    systemStage = SYSTEM_RUNNING;

    char buff[64] = {};

    drawLineText(oled, "Running...", 0xF00 | 0x00 );

    uint8_t currentDataFrame = byteData;
    uint8_t receivedDataFrame = '0';

    setTransmitBuff(&currentDataFrame, 1);
    setReceiveBuff(&receivedDataFrame, 1);

    while (systemStage != SYSTEM_STOPPED){
        /// Catch current byteData
        currentDataFrame = byteData;
        setTransmitBuff(&currentDataFrame, 1);
        setReceiveBuff(&receivedDataFrame, 1);

        oledFillScreen(oled, 0);

        /// Display mode and dataframe
        switch (currentSystemMode){
            case SYSTEM_MODE_SPI: snprintf(buff, sizeof(buff), "M: SPI"); break;
            case SYSTEM_MODE_I2C: snprintf(buff, sizeof(buff), "M: I2C"); break;
            case SYSTEM_MODE_UART:snprintf(buff, sizeof(buff), "M: UART");break;
            case SYSTEM_MODE_1WIRE:snprintf(buff, sizeof(buff), "M: 1-WIRE");break;
        }
        oledDrawText(oled, 20, 0, buff, 1, 1, 20);
        /// Display send byte data
        snprintf(buff, sizeof(buff), "> %02XH : %c", currentDataFrame, currentDataFrame);
        oledDrawText(oled, 40, 0, buff, 1, 1, 20);

        oledFlush(oled);

        sendDataBuff();

        snprintf(buff, sizeof(buff), "< %02XH : %c", receivedDataFrame, receivedDataFrame);
        oledDrawText(oled, 60, 0, buff, 1, 1, 20);

        oledFlush(oled);

        vTaskDelay(DATAFR_INTERVAL);
    }

    __exit("app_main()");
}
