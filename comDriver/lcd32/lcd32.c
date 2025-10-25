#include "lcd32.h"

/// LOCAL HELPERS ////////////////////////////////////////////////////////////////////////////////

/// @brief [Internal] Configure GPIO pins as `output` based on a 64-bit mask.
esp_err_t __lcd32GPIOConfigOutputM(uint64_t mask){
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = mask,          // 64-bit pin mask
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    return gpio_config(&io_conf);
}

/// @brief [Internal] Configure GPIO pins as `input` based on a 64-bit mask.
esp_err_t __lcd32GPIOConfigInputM(uint64_t mask){
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = mask,          // 64-bit pin mask
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    return gpio_config(&io_conf);
}

/// @brief [Internal] [Fast] Configure GPIO pins as `output` based on a 64-bit mask
void __lcd32FastSetDataBusInputM(uint64_t mask) {
    uint32_t low = (uint32_t)(mask & 0xFFFFFFFFULL);
    uint32_t high = (uint32_t)(mask >> 32);

    if (low)  REG_WRITE(GPIO_ENABLE_W1TC_REG, low);
    if (high) REG_WRITE(GPIO_ENABLE1_W1TC_REG, high);
}

/// @brief [Internal] [Fast] Configure GPIO pins as `input` based on a 64-bit mask 
void __lcd32FastSetDataBusOutputM(uint64_t mask) {
    uint32_t low = (uint32_t)(mask & 0xFFFFFFFFULL);
    uint32_t high = (uint32_t)(mask >> 32);

    if (low)  REG_WRITE(GPIO_ENABLE_W1TS_REG, low);
    if (high) REG_WRITE(GPIO_ENABLE1_W1TS_REG, high);
}


/// OTHER FUNCTIONS /////////////////////////////////////////////////////////////////////////////////

/// @brief [Internal] Delete canvas buffer
def __lcd32DeleteCanvasBuffer(lcd32Canvas_t *canvas){
    lcd32Log1("__lcd32DeleteCanvasBuffer(%p)", canvas);
    lcd32NULLCheck(canvas, STR(*canvas), STR(__lcd32DeleteCanvasBuffer), goto returnErr;);

    for (dim_t r = 0; r < canvas->maxRow; ++r) {
        if (__isnot_null(canvas->buff[r])) {
            lcd32Log1("[__lcd32DeleteCanvasBuffer] Free existed: canvas->buff[%d]", r);
            // free(canvas->buff[r]);
            /// TODO: Fix bug
        }
    }
    lcd32Log1("[__lcd32DeleteCanvasBuffer] Free existed: canvas->buff");
    // free(canvas->buff);
    /// TODO: Fix bug
    canvas->buff = NULL;
    
    lcd32Exit("__lcd32DeleteCanvasBuffer() : OKE");
    return OKE;
returnErr:
    lcd32Exit("__lcd32DeleteCanvasBuffer() : ERR");
    return ERR;
}

/// @brief [Internal] Config LCD 3.2" canvas (allocate buffer, set-up maxRow, maxCol)
def __lcd32ConfigCanvas(lcd32Canvas_t *canvas, dim_t maxRow, dim_t maxCol) {
    lcd32Entry("__lcd32ConfigCanvas(%p, %d, %d)", canvas, maxRow, maxCol);
    lcd32NULLCheck(canvas, STR(canvas), STR(__lcd32ConfigCanvas), return ERR_NULL;);

    canvas->maxRow = maxRow;
    canvas->maxCol = maxCol;

    // Free old buffer if it exists
    if (__isnot_null(canvas->buff)) {
        lcd32Warn("[__lcd32ConfigCanvas] canvas->buff = %p", canvas->buff);
        __lcd32DeleteCanvasBuffer(canvas);
    }

    // Allocate array of row pointers
    canvas->buff = (color_t **) heap_caps_malloc(sizeof(color_t *) * maxRow, MALLOC_CAP_SPIRAM);
    if (!canvas->buff) {
        lcd32Err("lcd32", "PSRAM allocation failed for row pointers array!");
        return ERR_PSRAM_FAILED;
    }

    // Allocate each row buffer
    for (dim_t r = 0; r < maxRow; ++r) {
        canvas->buff[r] = (color_t *) heap_caps_malloc(sizeof(color_t) * maxCol, MALLOC_CAP_SPIRAM);
        if (!canvas->buff[r]) {
            // Free previously allocated rows
            for (dim_t i = 0; i < r; ++i) free(canvas->buff[i]);
            free(canvas->buff);
            canvas->buff = NULL;
            lcd32Err("lcd32", "PSRAM allocation failed for row %d!", r);
            return ERR_PSRAM_FAILED;
        }
        memset(canvas->buff[r], 0, sizeof(color_t) * maxCol);
    }

    lcd32Log("Canvas created successfully in PSRAM (%d x %d).", maxRow, maxCol);
    lcd32Exit("__lcd32ConfigCanvas -> OKE");
    return OKE;
}

/// @brief [Internal] Config Control/Data pin
def __lcd32SetupPin(lcd32Dev_t *dev){
    lcd32Entry("__lcd32SetupPin(%p)", dev);
    lcd32NULLCheck(dev, STR(dev), STR(__lcd32SetupPin), return ERR_NULL;);

    lcd32Log("[__lcd32SetupPin] dataPin : %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | ", 
        dev->dataPin.__0, dev->dataPin.__1, dev->dataPin.__2, dev->dataPin.__3, 
        dev->dataPin.__4, dev->dataPin.__5, dev->dataPin.__6, dev->dataPin.__7, 
        dev->dataPin.__8, dev->dataPin.__9, dev->dataPin.__10, dev->dataPin.__11, 
        dev->dataPin.__12, dev->dataPin.__13, dev->dataPin.__14, dev->dataPin.__15
    );

    dev->dataPinMask = 0;

    lcd32Log("[__lcd32SetupPin] Config dataPin!");
    // Data pins
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__0, lcd32Err("GPIO_Err: %d", dev->dataPin.__0));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__1, lcd32Err("GPIO_Err: %d", dev->dataPin.__1));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__2, lcd32Err("GPIO_Err: %d", dev->dataPin.__2));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__3, lcd32Err("GPIO_Err: %d", dev->dataPin.__3));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__4, lcd32Err("GPIO_Err: %d", dev->dataPin.__4));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__5, lcd32Err("GPIO_Err: %d", dev->dataPin.__5));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__6, lcd32Err("GPIO_Err: %d", dev->dataPin.__6));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__7, lcd32Err("GPIO_Err: %d", dev->dataPin.__7));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__8, lcd32Err("GPIO_Err: %d", dev->dataPin.__8));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__9, lcd32Err("GPIO_Err: %d", dev->dataPin.__9));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__10, lcd32Err("GPIO_Err: %d", dev->dataPin.__10));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__11, lcd32Err("GPIO_Err: %d", dev->dataPin.__11));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__12, lcd32Err("GPIO_Err: %d", dev->dataPin.__12));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__13, lcd32Err("GPIO_Err: %d", dev->dataPin.__13));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__14, lcd32Err("GPIO_Err: %d", dev->dataPin.__14));
    lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__15, lcd32Err("GPIO_Err: %d", dev->dataPin.__15));

    lcd32Log("[__lcd32SetupPin] dataPin | pinMask = %p", dev->dataPinMask);

    esp_err_t ret = __lcd32GPIOConfigOutputM(dev->dataPinMask);
    if (ret != ESP_OK) {
        lcd32Err("[%s] gpio_config() failed: %s", STR(__lcd32SetupPin), esp_err_to_name(ret));
        return ERR;
    }

    dev->controlPinMask = 0;

    lcd32Log("[__lcd32SetupPin] Config controlPin!");
    // Control pins
    lcd32AddGPIO(dev->controlPinMask, dev->controlPin.r, lcd32Err("GPIO_Err: %d", dev->controlPin.r));
    lcd32AddGPIO(dev->controlPinMask, dev->controlPin.w, lcd32Err("GPIO_Err: %d", dev->controlPin.w));
    lcd32AddGPIO(dev->controlPinMask, dev->controlPin.rs, lcd32Err("GPIO_Err: %d", dev->controlPin.rs));
    lcd32AddGPIO(dev->controlPinMask, dev->controlPin.cs, lcd32Err("GPIO_Err: %d", dev->controlPin.cs));
    lcd32AddGPIO(dev->controlPinMask, dev->controlPin.rst, lcd32Err("GPIO_Err: %d", dev->controlPin.rst));
    lcd32AddGPIO(dev->controlPinMask, dev->controlPin.bl, lcd32Err("GPIO_Err: %d", dev->controlPin.bl));
    lcd32Log("[__lcd32SetupPin] controlPin | dev->controlPinMask = %p", dev->controlPinMask);

    ret = __lcd32GPIOConfigOutputM(dev->controlPinMask);
    if (ret != ESP_OK) {
        lcd32Err("[%s] gpio_config() failed: %s", STR(__lcd32SetupPin), esp_err_to_name(ret));
        return ERR;
    }

    __lcd32SetChipSelPin(dev, HIGH);
    __lcd32SetResetPin(dev, HIGH);
    __lcd32SetRegSelPin(dev, LOW);
    __lcd32SetReadPin(dev, HIGH);
    __lcd32SetWritePin(dev, HIGH);
    __lcd32SetParallelData(dev, LOW);
    __lcd32SetBrightnessLightPin(dev, HIGH);

    return OKE;
}

/// @brief [Global] Create new LCD 3.2" object then store to devPtr.
def lcd32CreateDevice(lcd32Dev_t **devPtr){
    lcd32Entry("lcd32CreateDevice(%p)", devPtr);
    lcd32NULLCheck(devPtr, "devPtr", "lcd32CreateDev", goto ReturnERR_NULL;);

    if(__isnot_null(DATA(devPtr))){
        lcd32Warn("[lcd32CreateDevice] The (*dev) is not null!");
        __lcd32DeleteCanvasBuffer(ADDR(DATA(devPtr)->canvas));
    }

    lcd32Log1("[lcd32CreateDevice] Create new LCD32 object!");
    /// Allocate the device
    DATA(devPtr) = (lcd32Dev_t *)malloc(sizeof(lcd32Dev_t));
    lcd32NULLCheck(*devPtr, "DATA(devPtr)", "lcd32CreateDev", goto ReturnERR_NULL;);
    
    lcd32Log1("[lcd32CreateDevice] Fill Zero to LCD32 object!");
    /// Fill zero
    lcd32AssignZero(DATA(devPtr));

    /// Manual reset object
    DATA(devPtr)->canvas.buff = NULL;


    lcd32Exit("lcd32CreateDevice() : OKE");
    return OKE;

    ReturnERR_NULL:
        lcd32Exit("lcd32CreateDevice() : ERR_NULL");
        return ERR_NULL;
}

/// @brief [Global] Config new LCD 3.2".
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
    __lcd32ConfigCanvas(&(dev->canvas), maxRow, maxCol);

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

/// @brief [Global] Start up LCD 3.2".
def lcd32StartUpDevice(lcd32Dev_t *dev) {
    lcd32Entry("lcd32StartUpDevice(%p)", dev);

    // Reset cứng (RESX)
    __lcd32SetResetPin(dev, 0);
    esp_rom_delay_us(10000);    // tRW ≥ 10µs
    __lcd32SetResetPin(dev, 1);
    esp_rom_delay_us(120000);   // tRT ≥ 120ms

    def devId = 0;
    devId = __lcd32ContReadRegisterStart(dev, ILI9341_READ_ID4) & 0xFF; 
    lcd32Log("Device ID (1st read): 0x%4x", devId);

    devId = __lcd32ContReadRegister(dev) & 0xFF;
    lcd32Log("Device ID (2nd read): 0x%4x", devId);

    devId = __lcd32ContReadRegister(dev) & 0xFF;
    lcd32Log("Device ID (3rd read): 0x%4x", devId);

    devId = (devId << 8) | (__lcd32ContReadRegister(dev) & 0xFF);
    lcd32Log("Device ID (Final read) : 0x%04x", devId);

    __lcd32ContReadRegisterStop(dev);

    if( devId == 0x9341 ){
        lcd32Log("[lcd32StartUpDevice] Detected ILI9341 LCD controller.");
    } else {
        lcd32Err("lcd32", "[lcd32StartUpDevice] Unsupported LCD controller: 0x%04x", devId);
        lcd32Exit("lcd32StartUpDevice -> ERR_UNSUPPORTED");
        return ERR_UNSUPPORTED;
    }

    // --- 4. ILI9341 Initialization Sequence (based on datasheet) ---
    __lcd32WriteCommand(dev, ILI9341_PIXEL_FORMAT_SET);
    __lcd32WriteData(dev, 0x55);     // 16-bit RGB565

    __lcd32WriteCommand(dev, ILI9341_BLANKING_PORCH_CTRL);
    __lcd32WriteData(dev, 0x04);
    __lcd32WriteData(dev, 0x04);
    __lcd32WriteData(dev, 0x0A);
    __lcd32WriteData(dev, 0x14);

    __lcd32WriteCommand(dev, ILI9341_TEARING_LINE_ON);
    __lcd32WriteData(dev, 0x00);

    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_B);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0xEA);
    __lcd32WriteData(dev, 0xF0);

    __lcd32WriteCommand(dev, ILI9341_POWER_ON_SEQ_CTRL);
    __lcd32WriteData(dev, 0x64);
    __lcd32WriteData(dev, 0x03);
    __lcd32WriteData(dev, 0x12);
    __lcd32WriteData(dev, 0x81);

    __lcd32WriteCommand(dev, ILI9341_DRIVER_TIMING_CTRL_A_INT);
    __lcd32WriteData(dev, 0x85);
    __lcd32WriteData(dev, 0x10);
    __lcd32WriteData(dev, 0x78);

    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_A);
    __lcd32WriteData(dev, 0x39);
    __lcd32WriteData(dev, 0x2C);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x33);
    __lcd32WriteData(dev, 0x06);

    __lcd32WriteCommand(dev, ILI9341_PUMP_RATIO_CONTROL);
    __lcd32WriteData(dev, 0x20);

    __lcd32WriteCommand(dev, ILI9341_DRIVER_TIMING_CTRL_B);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x00);

    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_1);
    __lcd32WriteData(dev, 0x21);

    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_2);
    __lcd32WriteData(dev, 0x10);

    __lcd32WriteCommand(dev, ILI9341_VCOM_CONTROL_1);
    __lcd32WriteData(dev, 0x4F);
    __lcd32WriteData(dev, 0x38);

    __lcd32WriteCommand(dev, ILI9341_VCOM_CONTROL_2);
    __lcd32WriteData(dev, 0xB7);

    __lcd32WriteCommand(dev, ILI9341_MEMORY_ACCESS_CONTROL);
#if DISP_ORIENTATION == 0
    __lcd32WriteData(dev, 0x08 | 0x00);
#elif DISP_ORIENTATION == 90
    __lcd32WriteData(dev, 0x08 | 0xA0);
#elif DISP_ORIENTATION == 180
    __lcd32WriteData(dev, 0x08 | 0xC0);
#else
    __lcd32WriteData(dev, 0x08 | 0x60);
#endif

    __lcd32WriteCommand(dev, ILI9341_FRAME_RATE_NORMAL);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x13);

    __lcd32WriteCommand(dev, ILI9341_DISPLAY_FUNCTION_CTRL);
    __lcd32WriteData(dev, 0x0A);
    __lcd32WriteData(dev, 0xA2);

    __lcd32WriteCommand(dev, ILI9341_ENABLE_3G);
    __lcd32WriteData(dev, 0x02);

    __lcd32WriteCommand(dev, ILI9341_GAMMA_SET);
    __lcd32WriteData(dev, 0x01);

    __lcd32WriteCommand(dev, ILI9341_POSITIVE_GAMMA_CORR);
    __lcd32WriteData(dev, 0x0F);
    __lcd32WriteData(dev, 0x27);
    __lcd32WriteData(dev, 0x24);
    __lcd32WriteData(dev, 0x0C);
    __lcd32WriteData(dev, 0x10);
    __lcd32WriteData(dev, 0x08);
    __lcd32WriteData(dev, 0x55);
    __lcd32WriteData(dev, 0x87);
    __lcd32WriteData(dev, 0x45);
    __lcd32WriteData(dev, 0x08);
    __lcd32WriteData(dev, 0x14);
    __lcd32WriteData(dev, 0x07);
    __lcd32WriteData(dev, 0x13);
    __lcd32WriteData(dev, 0x08);
    __lcd32WriteData(dev, 0x00);

    __lcd32WriteCommand(dev, ILI9341_NEGATIVE_GAMMA_CORR);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x0F);
    __lcd32WriteData(dev, 0x12);
    __lcd32WriteData(dev, 0x05);
    __lcd32WriteData(dev, 0x11);
    __lcd32WriteData(dev, 0x06);
    __lcd32WriteData(dev, 0x25);
    __lcd32WriteData(dev, 0x34);
    __lcd32WriteData(dev, 0x37);
    __lcd32WriteData(dev, 0x01);
    __lcd32WriteData(dev, 0x08);
    __lcd32WriteData(dev, 0x07);
    __lcd32WriteData(dev, 0x2B);
    __lcd32WriteData(dev, 0x34);
    __lcd32WriteData(dev, 0x0F);

    __lcd32WriteCommand(dev, ILI9341_SLEEP_OUT);
    esp_rom_delay_us(120000);

    __lcd32WriteCommand(dev, ILI9341_DISPLAY_ON);
    esp_rom_delay_us(50000);

    // --- 5. Backlight ON ---
    __lcd32SetBrightnessLightPin(dev, 1);

    lcd32Log("[lcd32StartUpDevice] ILI9341 initialized successfully.");
    lcd32Exit("lcd32StartUpDevice -> OKE");
    return OKE;
}

def __lcd32SetAddressWindow(lcd32Dev_t *dev, dim_t row, dim_t col, dim_t height, dim_t width) {
    lcd32Entry("__lcd32SetAddressWindow(%p, r=%u, c=%u, h=%u, w=%u)", dev, row, col, height, width);

    dim_t row1 = row;
    dim_t row2 = row + height - 1;
    dim_t col1 = col;
    dim_t col2 = col + width - 1;

    //--- Column Address Set (0x2A) ---
    __lcd32WriteCommand(dev, ILI9341_COLUMN_ADDRESS_SET);
    __lcd32WriteData(dev, col1 >> 8);
    __lcd32WriteData(dev, col1 & 0xFF);
    __lcd32WriteData(dev, col2 >> 8);
    __lcd32WriteData(dev, col2 & 0xFF);

    //--- Page Address Set (0x2B) ---
    __lcd32WriteCommand(dev, ILI9341_PAGE_ADDRESS_SET);
    __lcd32WriteData(dev, row1 >> 8);
    __lcd32WriteData(dev, row1 & 0xFF);
    __lcd32WriteData(dev, row2 >> 8);
    __lcd32WriteData(dev, row2 & 0xFF);

    lcd32Exit("__lcd32SetAddressWindow -> OKE");
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

/// @brief [Global] Flush the entire canvas to the LCD.
def lcd32FlushCanvas(lcd32Dev_t *dev) {
    lcd32Entry("lcd32FlushCanvas(%p)", dev);

    // Check for null pointer
    lcd32NULLCheck(dev, STR(dev), STR(lcd32FlushCanvas), return ERR_NULL;);
    lcd32Canvas_t *canvas = &dev->canvas;

    //--- Configure write region: full-screen area ---
    // Reuse the address window API to set the region (0,0) → (maxRow,maxCol)
    __lcd32SetAddressWindow(dev, 0, 0, canvas->maxRow, canvas->maxCol);

    //--- Send command to start memory write ---
    __lcd32WriteCommand(dev, ILI9341_MEMORY_WRITE);

    //--- Flush the entire canvas buffer to the LCD ---
    __lcd32SetDataTransaction(dev);
    __lcd32StartTransaction(dev);
    __lcd32SetReadPin(dev, 1);
    for (dim_t r = 0; r < canvas->maxRow; ++r) {
        for (dim_t c = 0; c < canvas->maxCol; ++c) {
            __lcd32SetParallelData(dev, canvas->buff[r][c]);
            __lcd32SetWritePin(dev, 0);
            __lcd32SetWritePin(dev, 1);
        }
    }
    __lcd32StopTransaction(dev);
    lcd32Exit("lcd32FlushCanvas() : OKE");
    return OKE;
}

/// @brief [Global] Write a single pixel directly to the LCD.
def lcd32DirectlyWritePixel(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color) {
    lcd32Log1("lcd32DirectlyWritePixel(%p, r=%d, c=%d, color=0x%04x)", dev, r, c, color);

    // Set write region to one pixel at (r, c)
    __lcd32SetAddressWindow(dev, r, c, 1, 1);

    // Write pixel color
    __lcd32WriteCommand(dev, ILI9341_MEMORY_WRITE);
    __lcd32WriteData(dev, color);

    return OKE;
}


def lcd32DrawEmptyRect(lcd32Dev_t *lcd, dim_t rTopLeft, dim_t cTopLeft, dim_t rBottomRight, dim_t cBottomRight, dim_t edgeSize, color_t color){
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
        __lcd32SetPixel(lcd, x, cTopLeft, color);
        __lcd32SetPixel(lcd, x, cBottomRight, color);
    }

    // Draw horizontal edges
    for (dim_t y = cTopLeft; y <= cBottomRight; ++y) {
        __lcd32SetPixel(lcd, rTopLeft, y, color);
        __lcd32SetPixel(lcd, rBottomRight, y, color);
    }

    // Recursive shrink for thicker borders
    return lcd32DrawEmptyRect(lcd,
                              rTopLeft + 1, cTopLeft + 1,
                              rBottomRight - 1, cBottomRight - 1,
                              edgeSize - 1, color);
}

def lcd32FillRect(lcd32Dev_t *lcd, dim_t rTopLeft, dim_t cTopLeft, dim_t rBottomRight, dim_t cBottomRight, color_t color){
    if (rTopLeft < 0 || cTopLeft < 0 ||
        rBottomRight >= lcd->canvas.maxRow ||
        cBottomRight >= lcd->canvas.maxCol)
        return ERR_INVALID_ARG;

    for (dim_t r = rTopLeft; r <= rBottomRight; ++r)
        for (dim_t c = cTopLeft; c <= cBottomRight; ++c)
            __lcd32SetPixelNC(lcd, r, c, color);

    return OKE;
}
