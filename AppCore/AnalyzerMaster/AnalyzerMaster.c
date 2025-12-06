#include "All.h"

LCD32Dev_t * lcd32 = NULL; 

void TaskScreen(void * pv){
    SysEntry("TaskScreen(%p)", pv);

    // 1. Create a new LCD device instance
    lcd32 = LCD32New();
    if (IsNull(lcd32)) {
        SysErr("[TaskScreen] Failed to create LCD device object.");
        vTaskDelete(NULL); // Abort task if creation fails
        return;
    }

    // 2. Define pin configuration arrays
    // Control Pins: RD, WR, CS, RS, RST, BL
    const Pin_t ctl_pins[6] = {
        LCD32_RD, LCD32_WR, LCD32_CS, LCD32_RS, LCD32_RST, LCD32_BL
    };

    // Data Pins: D0 to D15
    const Pin_t dat_pins[16] = {
        LCD32_DB0,  LCD32_DB1,  LCD32_DB2,  LCD32_DB3,
        LCD32_DB4,  LCD32_DB5,  LCD32_DB6,  LCD32_DB7,
        LCD32_DB8,  LCD32_DB9,  LCD32_DB10, LCD32_DB11,
        LCD32_DB12, LCD32_DB13, LCD32_DB14, LCD32_DB15
    };

    // 3. Configure the LCD driver with the specified pins
    if (LCD32Config(lcd32, ctl_pins, dat_pins) != STAT_OKE) {
        SysErr("[TaskScreen] LCD32Config failed.");
        LCD32Delete(lcd32);
        vTaskDelete(NULL);
        return;
    }

    // 4. Initialize the LCD hardware
    if (LCD32Init(lcd32) != STAT_OKE) {
        SysErr("[TaskScreen] LCD32Init failed.");
        LCD32Delete(lcd32);
        vTaskDelete(NULL);
        return;
    }
    
    // 5. Main loop: Fill with random color every second
    while (1){
        // Generate a random 16-bit color (RGB565). 
        // esp_random() is preferred on ESP-IDF for true random numbers.
        Color_t random_color = (Color_t) esp_random();

        // Fill the entire canvas buffer with this color
        LCD32FillCanvas(lcd32, random_color);

        // Push the canvas buffer to the physical LCD screen
        LCD32FlushCanvas(lcd32);

        SysLog("[TaskScreen    ] Flushed new color: 0x%04X", random_color);
        
        // Wait for 1 second before the next color change
        DelayMs(1000);
    }
}
