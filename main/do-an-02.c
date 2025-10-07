#include "../helper/general.h"

#if ESP32_DEVICE_MODE == SENDER
    #pragma message("ESP32_DEVICE_MODE: SENDER")
    #include "sender.h"
#endif

#if ESP32_DEVICE_MODE == RECEIVER
    #pragma message("ESP32_DEVICE_MODE: RECEIVER")
    #include "receiver.h"
#endif

/// VARS //////////////////////////////////////////////////////////////////////////////////////////

/// System Stage 
volatile flag_t systemStage = SYSTEM_INIT;
portMUX_TYPE systemStageMutex = portMUX_INITIALIZER_UNLOCKED;

/// OLED Flags
volatile flag_t oledFlag = OLED_REQ_COUNT;
portMUX_TYPE oledFlagMutex = portMUX_INITIALIZER_UNLOCKED;

/// System Mode
volatile flag_t systemMode = SYSTEM_MODE_SPI;
volatile flag_t currentSystemMode = -1;
portMUX_TYPE systemModeMutex = portMUX_INITIALIZER_UNLOCKED;

/// Communication Object
void* comObject = NULL;

/// Random 8-bit char (printable)
volatile uint8_t byteData = '?';
/// Random byte data control flag
volatile flag_t  byteDataControlFlag = 0;
/// Random byte data flag mutex
portMUX_TYPE     byteDataControlMutex = portMUX_INITIALIZER_UNLOCKED;

#if ESP32_DEVICE_MODE == SENDER

    /// Random 8-bit char (printable)
    volatile uint8_t sendByteData = '?';
    /// Random 8-bit char (printable)
    volatile uint8_t receivedByteData = '?';
    volatile flag_t  sendControlFlag = 0;

#endif

#if ESP32_DEVICE_MODE == RECEIVER

    /// Received data control flag
    volatile flag_t  rcvdControlFlag = 0;
    /// Received data control flag mutex
    portMUX_TYPE     rcvdControlMutex = portMUX_INITIALIZER_UNLOCKED;

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

    vTaskDelay(100);
    // __log("Enter test mode!");
    // drawLineText(oled, "Test mode", 0xF00 | 0x00 );
    // ledTest(NULL);

    __log("[+] oledTask()");
    drawLineText(oled, "[+] oledTask()", 0xF00 | 0x00 );
    xTaskCreate(oledTask, "oledTask", 2048, oled, 7, NULL);

    __log("[+] prepairByteDataTask()");
    drawLineText(oled, "[+] prepairByteDataTask()", 0xF00 | 0x00 );
    xTaskCreate(prepairByteDataTask, "prepairByteDataTask", 2048, NULL, 7, NULL);

    __log("[+] modeControlTask()");
    drawLineText(oled, "[+] modeControlTask()", 0xF00 | 0x00 );
    xTaskCreate(modeControlTask, "modeControlTask", 2048, NULL, 7, NULL);

    __log("Set systemStage = SYSTEM_RUNNING");
    systemStage = SYSTEM_RUNNING;

    char mode[] = /*2*/ "M: "  /*6*/ "1-WIRE" /*1*/;
    char rcvd[] = /*8*/ "> EMPTY";
    char send[] = /*8*/ "# ?";

    drawLineText(oled, "Running...", 0xF00 | 0x00 );

    uint8_t currentDataFrame = byteData;
    uint8_t receivedDataFrame = 0;
    
    vTaskDelay(100);

    setSendBackData(&currentDataFrame);
    setReceiveByteBuff(&receivedDataFrame);

    while (systemStage != SYSTEM_STOPPED){

        /// Display mode and dataframe
        switch (currentSystemMode){
            case SYSTEM_MODE_SPI:
                mode[3] = 'S'; mode[4] = 'P'; mode[5] = 'I'; mode[6] = ' '; mode[7] = ' '; mode[8] = ' ';
                break;
            case SYSTEM_MODE_I2C:
                mode[3] = 'I'; mode[4] = '2'; mode[5] = 'C'; mode[6] = ' '; mode[7] = ' '; mode[8] = ' ';
                break;
            case SYSTEM_MODE_UART:
                mode[3] = 'U'; mode[4] = 'A'; mode[5] = 'R'; mode[6] = 'T'; mode[7] = ' '; mode[8] = ' ';
                break;
            case SYSTEM_MODE_1WIRE:
                mode[3] = '1'; mode[4] = '-'; mode[5] = 'W'; mode[6] = 'I'; mode[7] = 'R'; mode[8] = 'E';
                break;
        }

        /// Show received data if any
        rcvd[2] = receivedDataFrame;
        rcvd[3] = '\0';
        rcvd[4] = '\0';
        rcvd[5] = '\0';
        rcvd[6] = '\0';
        rcvd[7] = '\0';

        /// Show current sendback dataframe
        send[2] = currentDataFrame;
        send[3] = '\0';

        drawLineText(oled, mode, __masks32(19) | 0xF00 | 0x00 );
        drawLineText(oled, rcvd, __masks32(16, 17, 18) | 0xF00 | 0x01 );
        drawLineText(oled, send, __masks32(16, 17, 18) | 0xF00 | 0x02 );

        // while(!checkNewData()) vTaskDelay(1);
    }

    __exit("app_main()");
}
