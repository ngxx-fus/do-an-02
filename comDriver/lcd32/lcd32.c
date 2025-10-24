#include "lcd32.h"

def __lcd32DeleteCanvas(lcd32Canvas_t **canvasPtr){
    lcd32Entry("__lcd32DeleteCanvas(%p)", canvasPtr);

    lcd32NULLCheck(canvasPtr, STR(canvasPtr), STR(__lcd32DeleteCanvas), goto returnErr;);
    lcd32NULLCheck(*canvasPtr, STR(*canvasPtr), STR(__lcd32DeleteCanvas), goto returnErr;);

    for (dim_t r = 0; r < DATA(canvasPtr)->maxRow; ++r) {
        if (__isnot_null(DATA(canvasPtr)->buff[r])) {
            lcd32Log("[__lcd32DeleteCanvas] Free existed: DATA(canvasPtr)->buff[%d]", r);
            // free(canvas->buff[r]);
            /// TODO: Fix bug
        }
    }
    lcd32Log("[__lcd32DeleteCanvas] Free existed: canvas->buff");
    // free(canvas->buff);
    /// TODO: Fix bug
    DATA(canvasPtr)->buff = NULL;
    
    lcd32Exit("__lcd32DeleteCanvas() : OKE");
    return OKE;
returnErr:
    lcd32Exit("__lcd32DeleteCanvas() : ERR");
    return ERR;
}

def __lcd32CreateCanvas(lcd32Canvas_t **canvasPtr, dim_t maxRow, dim_t maxCol) {
    lcd32Entry("__lcd32CreateCanvas(%p, %d, %d)", *canvasPtr, maxRow, maxCol);
    lcd32NULLCheck(DATA(canvasPtr), STR(DATA(canvasPtr)), STR(__lcd32CreateCanvas), return ERR_NULL;);

    DATA(canvasPtr)->maxRow = maxRow;
    DATA(canvasPtr)->maxCol = maxCol;

    // Free old buffer if it exists
    if (DATA(canvasPtr)->buff) {
        __lcd32DeleteCanvas(ADDR(DATA(canvasPtr)->buff));
    }

    // Allocate array of row pointers
    DATA(canvasPtr)->buff = (color_t **) heap_caps_malloc(sizeof(color_t *) * maxRow, MALLOC_CAP_SPIRAM);
    if (!DATA(canvasPtr)->buff) {
        lcd32Err("lcd32", "PSRAM allocation failed for row pointers array!");
        return ERR_PSRAM_FAILED;
    }

    // Allocate each row buffer
    for (dim_t r = 0; r < maxRow; ++r) {
        DATA(canvasPtr)->buff[r] = (color_t *) heap_caps_malloc(sizeof(color_t) * maxCol, MALLOC_CAP_SPIRAM);
        if (!DATA(canvasPtr)->buff[r]) {
            // Free previously allocated rows
            for (dim_t i = 0; i < r; ++i) free(DATA(canvasPtr)->buff[i]);
            free(DATA(canvasPtr)->buff);
            DATA(canvasPtr)->buff = NULL;
            lcd32Err("lcd32", "PSRAM allocation failed for row %d!", r);
            return ERR_PSRAM_FAILED;
        }
        memset(DATA(canvasPtr)->buff[r], 0, sizeof(color_t) * maxCol);
    }

    lcd32Log("Canvas created successfully in PSRAM (%d x %d).", maxRow, maxCol);
    lcd32Exit("__lcd32CreateCanvas -> OKE");
    return OKE;
}

def __lcd32SetupPin(lcd32Dev_t *dev){
    lcd32Entry("__lcd32SetupPin(%p)", dev);
    lcd32NULLCheck(dev, STR(dev), STR(__lcd32SetupPin), return ERR_NULL;);

    uint64_t pinMask = 0;

    lcd32Log("[__lcd32SetupPin] Config dataPin!");
    // Data pins
    lcd32AddGPIO(pinMask, dev->dataPin.__0);
    lcd32AddGPIO(pinMask, dev->dataPin.__1);
    lcd32AddGPIO(pinMask, dev->dataPin.__2);
    lcd32AddGPIO(pinMask, dev->dataPin.__3);
    lcd32AddGPIO(pinMask, dev->dataPin.__4);
    lcd32AddGPIO(pinMask, dev->dataPin.__5);
    lcd32AddGPIO(pinMask, dev->dataPin.__6);
    lcd32AddGPIO(pinMask, dev->dataPin.__7);
    lcd32AddGPIO(pinMask, dev->dataPin.__8);
    lcd32AddGPIO(pinMask, dev->dataPin.__9);
    lcd32AddGPIO(pinMask, dev->dataPin.__10);
    lcd32AddGPIO(pinMask, dev->dataPin.__11);
    lcd32AddGPIO(pinMask, dev->dataPin.__12);
    lcd32AddGPIO(pinMask, dev->dataPin.__13);
    lcd32AddGPIO(pinMask, dev->dataPin.__14);
    lcd32AddGPIO(pinMask, dev->dataPin.__15);

    /// Config all data pin as IN/OUTput
    gpio_config_t dataPin = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = pinMask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    esp_err_t ret = gpio_config(&dataPin);
    if (ret != ESP_OK) {
        lcd32Err("[%s] gpio_config() failed: %s", STR(__lcd32SetupPin), esp_err_to_name(ret));
        return ERR;
    }

    pinMask = 0;

    lcd32Log("[__lcd32SetupPin] Config controlPin!");
    // Control pins
    lcd32AddGPIO(pinMask, dev->controlPin.r);
    lcd32AddGPIO(pinMask, dev->controlPin.w);
    lcd32AddGPIO(pinMask, dev->controlPin.rs);
    lcd32AddGPIO(pinMask, dev->controlPin.cs);
    lcd32AddGPIO(pinMask, dev->controlPin.rst);
    lcd32AddGPIO(pinMask, dev->controlPin.bl);

    // Config all control pin as OUTPUT
    gpio_config_t controlPin = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = pinMask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    ret = gpio_config(&controlPin);
    if (ret != ESP_OK) {
        lcd32Err("[%s] gpio_config() failed: %s", STR(__lcd32SetupPin), esp_err_to_name(ret));
        return ERR;
    }

    __lcd32SetChipSelPin(dev, LOW);
    __lcd32SetResetPin(dev, LOW);
    __lcd32SetRegSelPin(dev, LOW);
    __lcd32SetReadPin(dev, LOW);
    __lcd32SetWritePin(dev, LOW);
    __lcd32SetParallelData(dev, LOW);
    __lcd32SetBrightnessLightPin(dev, HIGH);

    return OKE;
}

def lcd32CreateDevice(lcd32Dev_t **devPtr){
    lcd32Entry("cd32CreateDev(%p)", devPtr);
    lcd32NULLCheck(devPtr, "devPtr", "lcd32CreateDev", goto ReturnERR_NULL;);

    if(__isnot_nullDATA(devPtr)){
        lcd32Warn("[lcd32CreateDevice] The (*dev) is not null!");
        __lcd32DeleteCanvas(&(DATA(devPtr)->canvas))
    }

    /// Allocate the device
    DATA(devPtr) = (lcd32Dev_t *)malloc(sizeof(lcd32Dev_t));
    lcd32NULLCheck(*devPtr, "DATA(devPtr)", "lcd32CreateDev", goto ReturnERR_NULL;);
    
    /// Fill zero
    lcd32AssignZeroDATA(devPtr);

    lcd32Exit("cd32CreateDev() : OKE");
    return OKE;

    ReturnERR_NULL:
        lcd32Exit("cd32CreateDev() : ERR_NULL");
        return ERR_NULL;
}

def lcd32ConfigDevice(lcd32Dev_t *dev, lcd32DataPin_t *dataPin, lcd32ControlPin_t *controlPin, dim_t maxRow, dim_t maxCol){
    lcd32Entry("lcd32ConfigDev(%p, %p, %p, %d, %d)", dev, dataPin, controlPin, maxRow, maxCol);
    lcd32NULLCheck(dev, "dev", "lcd32ConfigDev", goto returnERR_NULL;);
    lcd32NULLCheck(dataPin, "dataPin", "lcd32ConfigDev", goto returnERR_NULL;);
    lcd32NULLCheck(controlPin, "controlPin", "lcd32ConfigDev", goto returnERR_NULL;);
    
    /// Record data pin value
    
    dev->dataPin.__0 = dataPin->__0;
    dev->dataPin.__1 = dataPin->__1;
    dev->dataPin.__2 = dataPin->__2;
    dev->dataPin.__3 = dataPin->__3;
    dev->dataPin.__4 = dataPin->__4;
    dev->dataPin.__5 = dataPin->__5;
    dev->dataPin.__6 = dataPin->__6;
    dev->dataPin.__7 = dataPin->__7;
    dev->dataPin.__8 = dataPin->__8;
    dev->dataPin.__9 = dataPin->__9;
    dev->dataPin.__10 = dataPin->__10;
    dev->dataPin.__11 = dataPin->__11;
    dev->dataPin.__12 = dataPin->__12;
    dev->dataPin.__13 = dataPin->__13;
    dev->dataPin.__14 = dataPin->__14;
    dev->dataPin.__15 = dataPin->__15;
    
    /// Record control pin value
    dev->controlPin.r   = controlPin->r;
    dev->controlPin.w   = controlPin->w;
    dev->controlPin.bl  = controlPin->bl;
    dev->controlPin.cs  = controlPin->cs;
    dev->controlPin.rs  = controlPin->rs;
    dev->controlPin.rst = controlPin->rst;

    /// Create canvas
    __lcd32CreateCanvas(&(dev->canvas), maxRow, maxCol);

    /// Set-up config pin
    __lcd32SetupPin(dev);

    /// Set-up mutex
    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
    dev->mutex = mutex;

    /// Reset transLevel
    dev->transLevel = 0;

    lcd32Exit("lcd32ConfigDev() : OKE");
    return OKE;

    returnERR_NULL:
    lcd32Exit("lcd32ConfigDev() : ERR_NULL");
    return ERR_NULL;
}

def lcd32StartUpDevice(lcd32Dev_t *dev) {
    lcd32Entry("lcd32StartUpDevice(%p)", dev);
    lcd32NULLCheck(dev, STR(dev), STR(lcd32StartUpDevice), return ERR_NULL;);

    // --- Hardware Reset ---
    lcd32Log("Performing hardware reset...");
    __lcd32SetResetPin(dev, 0);
    esp_rom_delay_us(50);  // Min 10us
    __lcd32SetResetPin(dev, 1);
    esp_rom_delay_us(120); // Min 5ms (or 120ms if exiting sleep)

    // --- Initialization Sequence (Revised) ---
    lcd32Log("Sending initialization sequence...");

    // 1. Software Reset
    __lcd32WriteCommand(dev, ILI9341_SWRESET); // 0x01
    vTaskDelay(pdMS_TO_TICKS(120)); // Must wait >5ms, 120ms recommended after sleep out

    // 2. Display OFF
    __lcd32WriteCommand(dev, ILI9341_DISPLAY_OFF); // 0x28

    // --- Extended Commands (Often module-specific, trying defaults first) ---

    // 3. Power Control A (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_A); // 0xCB
    __lcd32WriteData(dev, 0x39); // Default
    __lcd32WriteData(dev, 0x2C); // Default
    __lcd32WriteData(dev, 0x00); // Default
    __lcd32WriteData(dev, 0x34); // Default, Vcore = 1.6V
    __lcd32WriteData(dev, 0x02); // Default, DDVDH = 5.6V

    // 4. Power Control B (Using Datasheet Default - 0x81 might be safer than 0xA2)
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_B); // 0xCF
    __lcd32WriteData(dev, 0x00); // Default
    __lcd32WriteData(dev, 0x81); // Default is 0x81 or 0xA2, trying 0x81
    __lcd32WriteData(dev, 0x30); // Default

    // 5. Driver Timing Control A (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_DRIVER_TIMING_CTRL_A_INT); // 0xE8
    __lcd32WriteData(dev, 0x84); // Default (Note: datasheet typo, should be 85h? Let's try 84h first)
    __lcd32WriteData(dev, 0x11); // Default
    __lcd32WriteData(dev, 0x7A); // Default

    // 6. Driver Timing Control B (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_DRIVER_TIMING_CTRL_B); // 0xEA
    __lcd32WriteData(dev, 0x66); // Datasheet default, maybe 0x00? Trying 66h.
    __lcd32WriteData(dev, 0x00); // Default

    // 7. Power On Sequence Control (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_POWER_ON_SEQ_CTRL); // 0xED
    __lcd32WriteData(dev, 0x55); // Default
    __lcd32WriteData(dev, 0x01); // Default
    __lcd32WriteData(dev, 0x23); // Default
    __lcd32WriteData(dev, 0x01); // Default

    // 8. Pump Ratio Control (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_PUMP_RATIO_CONTROL); // 0xF7
    __lcd32WriteData(dev, 0x10); // Default (DDVDH=2xVCI) (User code used 0x20 also means DDVDH=2xVCI)

    // --- Standard Level 2 Commands ---

    // 9. Power Control 1 (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_1); // 0xC0
    __lcd32WriteData(dev, 0x21); // Default VRH[5:0] (User code used 0x23)

    // 10. Power Control 2 (Using Datasheet Default equivalent)
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_2); // 0xC1
    __lcd32WriteData(dev, 0x00); // Default BT[2:0]=0 (User code used 0x10 also means BT=0)

    // 11. VCOM Control 1 (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_VCOM_CONTROL_1); // 0xC5
    __lcd32WriteData(dev, 0x31); // Default VMH (User code used 0x3E)
    __lcd32WriteData(dev, 0x3C); // Default VML (User code used 0x28)

    // 12. VCOM Control 2 (Using Datasheet Default)
    __lcd32WriteCommand(dev, ILI9341_VCOM_CONTROL_2); // 0xC7
    __lcd32WriteData(dev, 0xC0); // Default VMF (User code used 0x86)

    // --- Standard Level 1 Configuration ---

    // 13. Memory Access Control (MADCTL) (Using user's common setting)
    __lcd32WriteCommand(dev, ILI9341_MEMORY_ACCESS_CONTROL); // 0x36
    __lcd32WriteData(dev, 0x48); // MX=1, BGR=1 common for landscape (Keep user's setting)

    // 14. Pixel Format Set (COLMOD) (Using user's common setting)
    __lcd32WriteCommand(dev, ILI9341_PIXEL_FORMAT_SET); // 0x3A
    __lcd32WriteData(dev, 0x55); // 16 bits/pixel (Keep user's setting)

    // 15. Frame Rate Control (Normal Mode) (Adding default)
    __lcd32WriteCommand(dev, ILI9341_FRAME_RATE_NORMAL); // 0xB1
    __lcd32WriteData(dev, 0x00); // Default DIVA=0
    __lcd32WriteData(dev, 0x1B); // Default RTNA=1Bh (~70Hz)

    // (Optional but recommended) Display Function Control (Default GS=0, SS=0, SM=0 are often ok)
    // __lcd32WriteCommand(dev, ILI9341_DISPLAY_FUNCTION_CTRL); // 0xB6
    // __lcd32WriteData(dev, 0x0A); // Default PTG=Interval, PT=V63/V0
    // __lcd32WriteData(dev, 0x82); // Default REV=NW, GS=0, SS=0, SM=0, ISC=5 frames
    // __lcd32WriteData(dev, 0x27); // Default NL=320 lines

    // (Optional but recommended) Gamma Set (Selects default curve)
    __lcd32WriteCommand(dev, ILI9341_GAMMA_SET); // 0x26
    __lcd32WriteData(dev, 0x01); // Select Gamma Curve 1 (G2.2)

    // (Gamma Correction - E0h, E1h usually not needed if using default curve 1)
    // __lcd32WriteCommand(dev, ILI9341_POSITIVE_GAMMA_CORR); // 0xE0
    // ... send 15 default bytes ...
    // __lcd32WriteCommand(dev, ILI9341_NEGATIVE_GAMMA_CORR); // 0xE1
    // ... send 15 default bytes ...

    // --- Exit Sleep and Turn On ---

    // 16. Sleep Out
    __lcd32WriteCommand(dev, ILI9341_SLEEP_OUT); // 0x11
    vTaskDelay(pdMS_TO_TICKS(120)); // MUST wait 120ms after sleep out

    // 17. Display ON
    __lcd32WriteCommand(dev, ILI9341_DISPLAY_ON); // 0x29
    vTaskDelay(pdMS_TO_TICKS(50)); // Short delay after display on

    // 18. Turn on Backlight
    lcd32Log("Turning on backlight.");
    __lcd32SetBrightnessLightPin(dev, 1);

    lcd32Log("Initialization complete.");
    lcd32Exit("lcd32StartUpDevice -> OKE");
    return OKE;
}

def lcd32FillCanvas(lcd32Dev_t *dev, color_t color){
    lcd32Entry("lcd32FillCanvas(%p, 0x%04x)", dev, color);
    lcd32NULLCheck(dev, STR(dev), STR(lcd32FillCanvas), return ERR_NULL;);
    REPT(dim_t, r, 0, dev->canvas.maxRow){
        REPT(dim_t, c, 0, dev->canvas.maxCol){
            dev->canvas.buff[r][c] = color;
        }
    }
    return OKE;
}

def lcd32FlushCanvas(lcd32Dev_t *dev){
    lcd32Entry("lcd32FlushCanvas(%p)", dev);

    lcd32NULLCheck(dev, STR(dev), STR(lcd32FlushCanvas), return ERR_NULL;);
    lcd32Canvas_t *canvas = &dev->canvas;

    __lcd32WriteCommand(dev, 0x2A); // Column address set
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, (canvas->maxCol - 1) >> 8);
    __lcd32WriteData(dev, (canvas->maxCol - 1) & 0xFF);

    __lcd32WriteCommand(dev, 0x2B); // Page address set
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, (canvas->maxRow - 1) >> 8);
    __lcd32WriteData(dev, (canvas->maxRow - 1) & 0xFF);

    __lcd32WriteCommand(dev, 0x2C); // Memory Write

    __lcd32SetRegSelPin(dev, 1);  // RS = 1 → Data
    __lcd32SetChipSelPin(dev, 0); // CS low

    for (dim_t r = 0; r < canvas->maxRow; ++r) {
        for (dim_t c = 0; c < canvas->maxCol; ++c) {
            color_t color = canvas->buff[r][c];
            __lcd32SetParallelData(dev, color >> 8);
            __lcd32SetWritePin(dev, 0);
            __lcd32SetWritePin(dev, 1);
            __lcd32SetParallelData(dev, color & 0xFF);
            __lcd32SetWritePin(dev, 0);
            __lcd32SetWritePin(dev, 1);
        }
    }

    __lcd32SetChipSelPin(dev, 1); // CS high
    return OKE;
}

void lcd32DirectWritePixel(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color){
    lcd32Log1("lcd32FlushCanvas(%p, %d, %d, 0x%04x)", dev, r, c, color);
    // --- Set column address ---
    __lcd32WriteCommand(dev, 0x2A);
    __lcd32WriteData(dev, (c >> 8) & 0xFF);
    __lcd32WriteData(dev, c & 0xFF);
    __lcd32WriteData(dev, ((c + 1) >> 8) & 0xFF);
    __lcd32WriteData(dev, (c + 1) & 0xFF);

    // --- Set page address ---
    __lcd32WriteCommand(dev, 0x2B);
    __lcd32WriteData(dev, (r >> 8) & 0xFF);
    __lcd32WriteData(dev, r & 0xFF);
    __lcd32WriteData(dev, ((r + 1) >> 8) & 0xFF);
    __lcd32WriteData(dev, (r + 1) & 0xFF);

    // --- Memory write ---
    __lcd32WriteCommand(dev, 0x2C);
    __lcd32WriteData(dev, color);
}

def lcd32DrawEmptyRect(lcd32Dev_t *lcd,
                       dim_t rTopLeft, dim_t cTopLeft,
                       dim_t rBottomRight, dim_t cBottomRight,
                       dim_t edgeSize, color_t color){
    if (edgeSize < 1) return OKE;

    if (rTopLeft < 0 || rTopLeft >= lcd->canvas.maxRow ||
        cTopLeft < 0 || cTopLeft >= lcd->canvas.maxCol ||
        rBottomRight < 0 || rBottomRight >= lcd->canvas.maxRow ||
        cBottomRight < 0 || cBottomRight >= lcd->canvas.maxCol)
        return ERR_INVALID_ARG;

    if (rTopLeft > rBottomRight || cTopLeft > cBottomRight)
        return ERR_INVALID_ARG;

    if (rBottomRight - rTopLeft <= 1 || cBottomRight - cTopLeft <= 1)
        return OKE;

    // Draw vertical edges
    for (dim_t x = rTopLeft; x <= rBottomRight; ++x) {
        lcd32SetPixel(lcd, x, cTopLeft, color);
        lcd32SetPixel(lcd, x, cBottomRight, color);
    }

    // Draw horizontal edges
    for (dim_t y = cTopLeft; y <= cBottomRight; ++y) {
        lcd32SetPixel(lcd, rTopLeft, y, color);
        lcd32SetPixel(lcd, rBottomRight, y, color);
    }

    // Recursive shrink for thicker borders
    return lcd32DrawEmptyRect(lcd,
                              rTopLeft + 1, cTopLeft + 1,
                              rBottomRight - 1, cBottomRight - 1,
                              edgeSize - 1, color);
}

def lcd32FillRect(lcd32Dev_t *lcd,
                  dim_t rTopLeft, dim_t cTopLeft,
                  dim_t rBottomRight, dim_t cBottomRight,
                  color_t color){
    if (rTopLeft < 0 || cTopLeft < 0 ||
        rBottomRight >= lcd->canvas.maxRow ||
        cBottomRight >= lcd->canvas.maxCol)
        return ERR_INVALID_ARG;

    for (dim_t r = rTopLeft; r <= rBottomRight; ++r)
        for (dim_t c = cTopLeft; c <= cBottomRight; ++c)
            lcd32SetPixelNC(lcd, r, c, color);

    return OKE;
}

def lcd32ClearCanvas(lcd32Dev_t *lcd, color_t color)
{
    if (!lcd || !lcd->canvas.buff) return ERR_NULL;

    for (dim_t r = 0; r < lcd->canvas.maxRow; ++r)
        memset(lcd->canvas.buff[r], color, sizeof(color_t) * lcd->canvas.maxCol);

    return OKE;
}
