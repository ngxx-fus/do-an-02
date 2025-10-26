#include "lcd32.h"


def __lcd32DeleteCanvasBuffer(lcd32Canvas_t *canvas){
    lcd32Log1("__lcd32DeleteCanvasBuffer(%p)", canvas);
    __lcd32NULLCheck(canvas, STR(*canvas), STR(__lcd32DeleteCanvasBuffer), goto returnErr;);

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
    
    __lcd32Exit("__lcd32DeleteCanvasBuffer() : OKE");
    return OKE;
returnErr:
    __lcd32Exit("__lcd32DeleteCanvasBuffer() : ERR");
    return ERR;
}

#if (LCD32_USE_PSRAM_FOR_LCD_CANVAS == 1)
def __lcd32ConfigCanvas(lcd32Canvas_t *canvas, dim_t maxRow, dim_t maxCol) {
    __lcd32Entry("__lcd32ConfigCanvas(%p, %d, %d)", canvas, maxRow, maxCol);
    __lcd32NULLCheck(canvas, STR(canvas), STR(__lcd32ConfigCanvas), return ERR_NULL;);

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
        lcd32Err("[__lcd32ConfigCanvas] PSRAM allocation failed for row pointers array!");
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
            lcd32Err("[__lcd32ConfigCanvas] PSRAM allocation failed for row %d!", r);
            return ERR_PSRAM_FAILED;
        }
        memset(canvas->buff[r], 0, sizeof(color_t) * maxCol);
    }

    __lcd32Log("[__lcd32ConfigCanvas] Canvas created successfully in PSRAM (%d x %d).", maxRow, maxCol);
    __lcd32Exit("__lcd32ConfigCanvas -> OKE");
    return OKE;
}
#else
/// @brief [Internal] Config LCD 3.2" canvas (allocate buffer, set-up maxRow, maxCol)
def __lcd32ConfigCanvas(lcd32Canvas_t *canvas, dim_t maxRow, dim_t maxCol) {
    __lcd32Entry("__lcd32ConfigCanvas(%p, %d, %d)", canvas, maxRow, maxCol);
    __lcd32NULLCheck(canvas, STR(canvas), STR(__lcd32ConfigCanvas), return ERR_NULL;);

    canvas->maxRow = maxRow;
    canvas->maxCol = maxCol;

    // Free old buffer if it exists
    if (__isnot_null(canvas->buff)) {
        lcd32Warn("[__lcd32ConfigCanvas] canvas->buff = %p", canvas->buff);
        __lcd32DeleteCanvasBuffer(canvas);
    }

    // Allocate array of row pointers (use normal malloc)
    canvas->buff = (color_t **) malloc(sizeof(color_t *) * maxRow);
    if (!canvas->buff) {
        lcd32Err("[__lcd32ConfigCanvas] DRAM allocation failed for row pointers array!");
        return ERR_MALLOC_FAILED;
    }

    // Allocate each row buffer
    for (dim_t r = 0; r < maxRow; ++r) {
        canvas->buff[r] = (color_t *) malloc(sizeof(color_t) * maxCol);
        if (!canvas->buff[r]) {
            // Free previously allocated rows
            for (dim_t i = 0; i < r; ++i) free(canvas->buff[i]);
            free(canvas->buff);
            canvas->buff = NULL;
            lcd32Err("[__lcd32ConfigCanvas] DRAM allocation failed for row %d!", r);
            return ERR_MALLOC_FAILED;
        }
        memset(canvas->buff[r], 0, sizeof(color_t) * maxCol);
    }

    __lcd32Log("[__lcd32ConfigCanvas] Canvas created successfully in DRAM (%d x %d).", maxRow, maxCol);
    __lcd32Exit("__lcd32ConfigCanvas -> OKE");
    return OKE;
}
#endif

def __lcd32SetupPin(lcd32Dev_t *dev){
    __lcd32Entry("__lcd32SetupPin(%p)", dev);
    __lcd32NULLCheck(dev, STR(dev), STR(__lcd32SetupPin), return ERR_NULL;);

    __lcd32Log("[__lcd32SetupPin] dataPin : %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | ", 
        dev->dataPin.__0, dev->dataPin.__1, dev->dataPin.__2, dev->dataPin.__3, 
        dev->dataPin.__4, dev->dataPin.__5, dev->dataPin.__6, dev->dataPin.__7, 
        dev->dataPin.__8, dev->dataPin.__9, dev->dataPin.__10, dev->dataPin.__11, 
        dev->dataPin.__12, dev->dataPin.__13, dev->dataPin.__14, dev->dataPin.__15
    );

    dev->dataPinMask = 0;

    __lcd32Log("[__lcd32SetupPin] Config dataPin!");
    // Data pins
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__0, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__1, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__2, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__3, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__4, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__5, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__6, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__7, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__8, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__9, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__10, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__11, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__12, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__13, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__14, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->dataPinMask, dev->dataPin.__15, return ERR_UNSUPPORTED;);

    __lcd32Log("[__lcd32SetupPin] dataPin | pinMask = %p", dev->dataPinMask);

    def ret = GPIOConfigOutputMask(dev->dataPinMask);
    if (ret != ESP_OK) {
        lcd32Err("[%s] gpio_config() failed: %s", STR(__lcd32SetupPin), esp_err_to_name(ret));
        return ERR;
    }

    dev->controlPinMask = 0;

    __lcd32Log("[__lcd32SetupPin] Config controlPin!");
    // Control pins
    __lcd32AddGPIO(dev->controlPinMask, dev->controlPin.r,   return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->controlPinMask, dev->controlPin.w,   return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->controlPinMask, dev->controlPin.rs,  return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->controlPinMask, dev->controlPin.cs,  return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->controlPinMask, dev->controlPin.rst, return ERR_UNSUPPORTED;);
    __lcd32AddGPIO(dev->controlPinMask, dev->controlPin.bl,  return ERR_UNSUPPORTED;);

    __lcd32Log("[__lcd32SetupPin] controlPin | dev->controlPinMask = %p", dev->controlPinMask);

    ret = GPIOConfigOutputMask(dev->controlPinMask);
    if (ret != ESP_OK) {
        lcd32Err("[%s] gpio_config() failed: %s", STR(__lcd32SetupPin), esp_err_to_name(ret));
        return ERR;
    }

    __lcd32SetHighChipSelPin(dev);
    __lcd32SetHighResetPin(dev);
    __lcd32SetLowRegSelPin(dev);
    __lcd32SetHighReadPin(dev);
    __lcd32SetHighWritePin(dev);
    __lcd32SetParallelData(dev, 0);
    __lcd32SetHighBrightLightPin(dev);

    return OKE;
}

def __lcd32SetAddressWindow(lcd32Dev_t *dev, dim_t row, dim_t col, dim_t height, dim_t width) {
    __lcd32Entry("__lcd32SetAddressWindow(%p, r=%u, c=%u, h=%u, w=%u)", dev, row, col, height, width);

    dim_t x1 = col;
    dim_t y1 = row;
    dim_t x2 = col + width - 1;
    dim_t y2 = row + height - 1;

#if LCD32_DISP_ORIENTATION == 0
    // Portrait - no change
#elif LCD32_DISP_ORIENTATION == 1
    // Rotate 90° CW
    x1 = dev->canvas.maxCol - 1 - (row + height - 1);
    x2 = dev->canvas.maxCol - 1 - row;
    y1 = col;
    y2 = col + width - 1;
#elif LCD32_DISP_ORIENTATION == 2
    // Rotate 180°
    x1 = dev->canvas.maxCol - 1 - (col + width - 1);
    x2 = dev->canvas.maxCol - 1 - col;
    y1 = dev->canvas.maxRow - 1 - (row + height - 1);
    y2 = dev->canvas.maxRow - 1 - row;
#elif LCD32_DISP_ORIENTATION == 3
    // Rotate 270° CW
    x1 = row;
    x2 = row + height - 1;
    y1 = dev->canvas.maxRow - 1 - (col + width - 1);
    y2 = dev->canvas.maxRow - 1 - col;
#endif

    // --- Column Address Set (0x2A) ---
    __lcd32WriteCommand(dev, ILI9341_COLUMN_ADDRESS_SET);
    __lcd32WriteData(dev, (x1 >> 8) & 0xFF);
    __lcd32WriteData(dev, x1 & 0xFF);
    __lcd32WriteData(dev, (x2 >> 8) & 0xFF);
    __lcd32WriteData(dev, x2 & 0xFF);

    // --- Page Address Set (0x2B) ---
    __lcd32WriteCommand(dev, ILI9341_PAGE_ADDRESS_SET);
    __lcd32WriteData(dev, (y1 >> 8) & 0xFF);
    __lcd32WriteData(dev, y1 & 0xFF);
    __lcd32WriteData(dev, (y2 >> 8) & 0xFF);
    __lcd32WriteData(dev, y2 & 0xFF);

    // --- Memory Write (0x2C) ---
    __lcd32WriteCommand(dev, ILI9341_MEMORY_WRITE);

    __lcd32Exit("__lcd32SetAddressWindow -> OKE");
    return OKE;
}

def lcd32CreateDevice(lcd32Dev_t **devPtr){
    __lcd32Entry("lcd32CreateDevice(%p)", devPtr);
    __lcd32NULLCheck(devPtr, "devPtr", "lcd32CreateDev", goto ReturnERR_NULL;);

    if(__isnot_null(DATA(devPtr))){
        lcd32Warn("[lcd32CreateDevice] The (*dev) is not null!");
        __lcd32DeleteCanvasBuffer(ADDR(DATA(devPtr)->canvas));
    }

    lcd32Log1("[lcd32CreateDevice] Create new LCD32 object!");
    /// Allocate the device
    DATA(devPtr) = (lcd32Dev_t *)malloc(sizeof(lcd32Dev_t));
    __lcd32NULLCheck(*devPtr, "DATA(devPtr)", "lcd32CreateDev", goto ReturnERR_NULL;);
    
    lcd32Log1("[lcd32CreateDevice] Fill Zero to LCD32 object!");
    /// Fill zero
    lcd32AssignZero(DATA(devPtr));

    /// Manual reset object
    DATA(devPtr)->canvas.buff = NULL;


    __lcd32Exit("lcd32CreateDevice() : OKE");
    return OKE;

    ReturnERR_NULL:
        __lcd32Exit("lcd32CreateDevice() : ERR_NULL");
        return ERR_NULL;
}

def lcd32ConfigDevice(lcd32Dev_t *dev, lcd32DataPin_t *dataPin, lcd32ControlPin_t *controlPin, dim_t maxRow, dim_t maxCol){
    __lcd32Entry("lcd32ConfigDev(%p, %p, %p, %d, %d)", dev, dataPin, controlPin, maxRow, maxCol);
    __lcd32NULLCheck(dev, "dev", "lcd32ConfigDev", goto returnERR_NULL;);
    __lcd32NULLCheck(dataPin, "dataPin", "lcd32ConfigDev", goto returnERR_NULL;);
    __lcd32NULLCheck(controlPin, "controlPin", "lcd32ConfigDev", goto returnERR_NULL;);
    
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

    __lcd32Log("[lcd32ConfigDev] Set data pin to input then output mode!");
    __lcd32SetInputData16(dev);
    __lcd32SetOutputData16(dev);


    /// Set-up mutex
    __lcd32Log("[lcd32ConfigDev] Set up mutex!");
    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
    dev->mutex = mutex;

    /// Reset transLevel
    dev->transLevel = 0;

    __lcd32Exit("lcd32ConfigDev() : OKE");
    return OKE;

    returnERR_NULL:
    __lcd32Exit("lcd32ConfigDev() : ERR_NULL");
    return ERR_NULL;
}

def lcd32StartUpDevice(lcd32Dev_t *dev) {
    __lcd32Entry("lcd32StartUpDevice(%p)", dev);

    __lcd32SetLowBrightLightPin(dev);

    /// Hardware reset (RESX)
    __lcd32SetLowResetPin(dev);
    esp_rom_delay_us(10000);    /// tRW ≥ 10µs
    __lcd32SetHighResetPin(dev);
    esp_rom_delay_us(120000);   /// tRT ≥ 120ms after release

    __lcd32SetHighBrightLightPin(dev);
    __lcd32SetHighReadPin(dev);
    __lcd32SetHighWritePin(dev);

    /// Read LCD ID (should be 0x9341 for ILI9341)
    uint16_t devId = 0, readVal;
    
    __lcd32StartTransaction(dev);

    /// Write command to read ID1

    __lcd32SetOutputData16(dev);
    __lcd32WriteCommand(dev, 0);
    __lcd32Log("Write : 0x%04x", 0);

    __lcd32SetInputData16(dev);
    __lcd32ReadData(dev, readVal); // dummy
    __lcd32Log("Device ID (dummy read) : 0x%04x", readVal);

    __lcd32ReadData(dev, readVal); // dummy
    __lcd32Log("Device ID (dummy read) : 0x%04x", readVal);

    /// Write command to read ID4

    __lcd32SetOutputData16(dev);
    __lcd32WriteCommand(dev, ILI9341_READ_ID4);
    __lcd32Log("Write : 0x%04x", ILI9341_READ_ID4);

    __lcd32SetInputData16(dev);
    __lcd32ReadData(dev, readVal);
    __lcd32Log("Read (1st) : 0x%04x", readVal);

    __lcd32ReadData(dev, readVal); // dummy
    __lcd32Log("Read (2nd) : 0x%04x", readVal);

    __lcd32ReadData(dev, readVal);
    __lcd32Log("Read (3rd)     : 0x%04x", readVal);

    __lcd32ReadData(dev, readVal);
    __lcd32Log("Read (4th)     : 0x%04x", readVal);
    devId = (readVal << 8) & 0xFF00;

    __lcd32ReadData(dev, readVal);
    __lcd32Log("Device ID (byte 2)     : 0x%04x", readVal);
    devId = devId | ( readVal & 0x00FF);

    // For ILI9341 the device id is typically (b1 << 8) | b2 -> expect 0x9347
    __lcd32Log("Device ID (Final read) : 0x%04x", devId);

    __lcd32StopTransaction(dev);
    __lcd32SetOutputData16(dev); // Set data pins back to output

    if (devId == 0x9341) {
        __lcd32Log("[lcd32StartUpDevice] Detected ILI9341 LCD controller.");
    } else {
        lcd32Err("[__lcd32ConfigCanvas] [lcd32StartUpDevice] Unsupported LCD controller: 0x%04x", devId);
        __lcd32Exit("lcd32StartUpDevice -> ERR_UNSUPPORTED");
        return ERR_UNSUPPORTED;
    }

    // ------------------------------------------------------------
    // Initialization sequence (per ILI9341 datasheet, section 8.2)
    // ------------------------------------------------------------

    __lcd32StartTransaction(dev);

    /// Set pixel format: 16-bit RGB565
    __lcd32WriteCommand(dev, ILI9341_PIXEL_FORMAT_SET);
    __lcd32WriteData(dev, 0x55);

    /// Set blanking porch control
    __lcd32WriteCommand(dev, ILI9341_BLANKING_PORCH_CTRL);
    __lcd32WriteData(dev, 0x04);
    __lcd32WriteData(dev, 0x04);
    __lcd32WriteData(dev, 0x0A);
    __lcd32WriteData(dev, 0x14);

    /// Enable tearing line output
    __lcd32WriteCommand(dev, ILI9341_TEARING_LINE_ON);
    __lcd32WriteData(dev, 0x00);

    /// Power control B (default power sequence settings)
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_B);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0xEA);
    __lcd32WriteData(dev, 0xF0);

    /// Power on sequence control
    __lcd32WriteCommand(dev, ILI9341_POWER_ON_SEQ_CTRL);
    __lcd32WriteData(dev, 0x64);
    __lcd32WriteData(dev, 0x03);
    __lcd32WriteData(dev, 0x12);
    __lcd32WriteData(dev, 0x81);

    /// Driver timing control A (internal)
    __lcd32WriteCommand(dev, ILI9341_DRIVER_TIMING_CTRL_A_INT);
    __lcd32WriteData(dev, 0x85);
    __lcd32WriteData(dev, 0x10);
    __lcd32WriteData(dev, 0x78);

    /// Power control A
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_A);
    __lcd32WriteData(dev, 0x39);
    __lcd32WriteData(dev, 0x2C);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x33);
    __lcd32WriteData(dev, 0x06);

    /// Pump ratio control (boost voltage level)
    __lcd32WriteCommand(dev, ILI9341_PUMP_RATIO_CONTROL);
    __lcd32WriteData(dev, 0x20);

    /// Driver timing control B
    __lcd32WriteCommand(dev, ILI9341_DRIVER_TIMING_CTRL_B);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x00);

    /// Power control 1 (VRH - Vreg1out voltage)
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_1);
    __lcd32WriteData(dev, 0x21);

    /// Power control 2 (SAP/BT function control)
    __lcd32WriteCommand(dev, ILI9341_POWER_CONTROL_2);
    __lcd32WriteData(dev, 0x10);

    /// VCOM control 1 (adjust VCOMH/VCOML)
    __lcd32WriteCommand(dev, ILI9341_VCOM_CONTROL_1);
    __lcd32WriteData(dev, 0x4F);
    __lcd32WriteData(dev, 0x38);

    /// VCOM control 2 (VCOM offset)
    __lcd32WriteCommand(dev, ILI9341_VCOM_CONTROL_2);
    __lcd32WriteData(dev, 0xB7);

    /// Memory access control (orientation / color order)
    __lcd32WriteCommand(dev, ILI9341_MEMORY_ACCESS_CONTROL);
#if LCD32_DISP_ORIENTATION == 0
    __lcd32WriteData(dev, 0x08 | 0x00);
#elif LCD32_DISP_ORIENTATION == 90
    __lcd32WriteData(dev, 0x08 | 0xA0);
#elif LCD32_DISP_ORIENTATION == 180
    __lcd32WriteData(dev, 0x08 | 0xC0);
#else
    __lcd32WriteData(dev, 0x08 | 0x60);
#endif

    /// Frame rate control (normal mode)
    __lcd32WriteCommand(dev, ILI9341_FRAME_RATE_NORMAL);
    __lcd32WriteData(dev, 0x00);
    __lcd32WriteData(dev, 0x13);

    /// Display function control
    __lcd32WriteCommand(dev, ILI9341_DISPLAY_FUNCTION_CTRL);
    __lcd32WriteData(dev, 0x0A);
    __lcd32WriteData(dev, 0xA2);

    /// 3G gamma control (enable)
    __lcd32WriteCommand(dev, ILI9341_ENABLE_3G);
    __lcd32WriteData(dev, 0x02);

    /// Gamma set
    __lcd32WriteCommand(dev, ILI9341_GAMMA_SET);
    __lcd32WriteData(dev, 0x01);

    /// Positive gamma correction
    __lcd32WriteCommand(dev, ILI9341_POSITIVE_GAMMA_CORR);
    // (Values adjusted per datasheet example)
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

    /// Negative gamma correction
    __lcd32WriteCommand(dev, ILI9341_NEGATIVE_GAMMA_CORR);
    // (Values adjusted per datasheet example)
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

    /// Exit sleep mode (SLPOUT)
    __lcd32WriteCommand(dev, ILI9341_SLEEP_OUT);
    esp_rom_delay_us(120000);

    /// Turn on display (DISPON)
    __lcd32WriteCommand(dev, ILI9341_DISPLAY_ON);
    esp_rom_delay_us(50000);

    /// Turn on backlight
    __lcd32SetHighBrightLightPin(dev);

    __lcd32StopTransaction(dev);

    __lcd32Log("[lcd32StartUpDevice] ILI9341 initialized successfully.");
    __lcd32Exit("lcd32StartUpDevice -> OKE");
    return OKE;
}

def lcd32PutToSleep(lcd32Dev_t *dev) {
    __lcd32Entry("lcd32TurnOff(%p)", dev);

    // --- Input Validation ---
    if (!dev) {
        __sys_err("[lcd32TurnOff] dev is NULL");
        return ERR_INVALID_ARG;
    }

    __lcd32StartTransaction(dev);

    // 1. Turn off backlight immediately
    __lcd32SetLowBrightLightPin(dev);
    __lcd32Log("Backlight turned OFF");

    // 2. Send Display OFF command (0x28) [cite: 13]
    __lcd32WriteCommand(dev, ILI9341_DISPLAY_OFF);
    __lcd32Log("Sent CMD: DISPLAY_OFF (0x%02X)", ILI9341_DISPLAY_OFF);
    // Datasheet doesn't mandate a delay here, but a small one might be safe
    esp_rom_delay_us(1000); // 1ms delay (optional)

    // 3. Send Enter Sleep Mode command (0x10) [cite: 13]
    __lcd32WriteCommand(dev, ILI9341_ENTER_SLEEP);
    __lcd32Log("Sent CMD: ENTER_SLEEP (0x%02X)", ILI9341_ENTER_SLEEP);
    // It takes time to enter sleep, but we don't need to wait here unless
    // we immediately power down VCI/VDDI. Delay is needed *after* waking up.

    __lcd32StopTransaction(dev);

    __lcd32Exit("lcd32TurnOff -> OKE");
    return OKE;
}

def lcd32WakeFromSleep(lcd32Dev_t *dev) {
    __lcd32Entry("lcd32TurnOn(%p)", dev);

    // --- Input Validation ---
    if (!dev) {
        __sys_err("[lcd32TurnOn] dev is NULL");
        return ERR_INVALID_ARG;
    }

    __lcd32StartTransaction(dev);

    // 1. Send Sleep Out command (0x11) [cite: 13]
    __lcd32WriteCommand(dev, ILI9341_SLEEP_OUT);
    __lcd32Log("Sent CMD: SLEEP_OUT (0x%02X)", ILI9341_SLEEP_OUT);

    // 2. Wait for stabilization after Sleep Out [cite: 1314, 1329]
    // The datasheet mentions 5ms minimum before the *next command*,
    // but 120ms is needed *after SLEEP OUT* before SLEEP IN can be sent again.
    // The startup sequence waits 120ms after SLEEP_OUT, let's follow that for safety.
    esp_rom_delay_us(120000); // 120ms delay

    // 3. Send Display ON command (0x29) [cite: 13]
    __lcd32WriteCommand(dev, ILI9341_DISPLAY_ON);
    __lcd32Log("Sent CMD: DISPLAY_ON (0x%02X)", ILI9341_DISPLAY_ON);

    // 4. Wait briefly after Display ON (matching startup sequence)
    esp_rom_delay_us(50000); // 50ms delay

    // 5. Turn on backlight
    __lcd32SetHighBrightLightPin(dev);
    __lcd32Log("Backlight turned ON");

    __lcd32StopTransaction(dev);

    __lcd32Exit("lcd32TurnOn -> OKE");
    return OKE;
}

def lcd32FillCanvas(lcd32Dev_t *dev, color_t color){
    // __lcd32Entry("lcd32FillCanvas(%p, 0x%04x)", dev, color);
    __lcd32NULLCheck(dev, STR(dev), STR(lcd32FillCanvas), return ERR_NULL;);
    
    REPT(dim_t, r, 0, dev->canvas.maxRow){
        REPT(dim_t, c, 0, dev->canvas.maxCol){
            dev->canvas.buff[r][c] = color;
        }
    }
    // __lcd32Exit("lcd32FillCanvas() : OKE");
    return OKE;
}

def lcd32FlushCanvas(lcd32Dev_t *dev) {
    // __lcd32Entry("lcd32FlushCanvas(%p)", dev);

    // --- Null check ---
    __lcd32NULLCheck(dev, STR(dev), STR(lcd32FlushCanvas), return ERR_NULL;);
    lcd32Canvas_t *canvas = &dev->canvas;

    // --- Begin LCD bus transaction (assert CS, prepare write mode) ---
    __lcd32StartTransaction(dev);
    vPortEnterCritical(ADDR(dev->mutex));

    // --- Inline address window setup (merged from __lcd32SetAddressWindow) ---
    dim_t row = 0, col = 0;
    dim_t height = canvas->maxRow, width = canvas->maxCol;

    dim_t x1 = col;
    dim_t y1 = row;
    dim_t x2 = col + width - 1;
    dim_t y2 = row + height - 1;

    #if LCD32_DISP_ORIENTATION == 0
        // Portrait - no change
    #elif LCD32_DISP_ORIENTATION == 1
        x1 = dev->canvas.maxCol - 1 - (row + height - 1);
        x2 = dev->canvas.maxCol - 1 - row;
        y1 = col;
        y2 = col + width - 1;
    #elif LCD32_DISP_ORIENTATION == 2
        x1 = dev->canvas.maxCol - 1 - (col + width - 1);
        x2 = dev->canvas.maxCol - 1 - col;
        y1 = dev->canvas.maxRow - 1 - (row + height - 1);
        y2 = dev->canvas.maxRow - 1 - row;
    #elif LCD32_DISP_ORIENTATION == 3
        x1 = row;
        x2 = row + height - 1;
        y1 = dev->canvas.maxRow - 1 - (col + width - 1);
        y2 = dev->canvas.maxRow - 1 - col;
    #endif

    // --- Column Address Set (0x2A) ---
    __lcd32WriteCommand(dev, ILI9341_COLUMN_ADDRESS_SET);
    __lcd32WriteData(dev, (x1 >> 8) & 0xFF);
    __lcd32WriteData(dev, x1 & 0xFF);
    __lcd32WriteData(dev, (x2 >> 8) & 0xFF);
    __lcd32WriteData(dev, x2 & 0xFF);

    // --- Page Address Set (0x2B) ---
    __lcd32WriteCommand(dev, ILI9341_PAGE_ADDRESS_SET);
    __lcd32WriteData(dev, (y1 >> 8) & 0xFF);
    __lcd32WriteData(dev, y1 & 0xFF);
    __lcd32WriteData(dev, (y2 >> 8) & 0xFF);
    __lcd32WriteData(dev, y2 & 0xFF);

    // --- Memory Write (0x2C) ---
    __lcd32WriteCommand(dev, ILI9341_MEMORY_WRITE);

    // --- Flush pixel data over 16-bit parallel bus ---
    // int64_t tStart = esp_timer_get_time();
    __lcd32SetDataTransaction(dev);

    for (dim_t r = 0; r < canvas->maxRow; ++r) {
        for (dim_t c = 0; c < canvas->maxCol; ++c) {
            __lcd32SetParallelData(dev, canvas->buff[r][c]);
            __LCD32_WRITE_SIG(dev);   // toggle WR pin to latch data
        }
    }

    // int64_t tStop = esp_timer_get_time();
    // __lcd32Log("[lcd32FlushCanvas] Parallel write time: %lld us", (tStop - tStart));

    // --- End LCD bus transaction (release CS) ---
    __lcd32StopTransaction(dev);
    vPortExitCritical(ADDR(dev->mutex));

    // __lcd32Exit("lcd32FlushCanvas() : OKE");
    return OKE;
}

def lcd32DirectlyWritePixel(lcd32Dev_t *dev, dim_t r, dim_t c, color_t color) {
    lcd32Log1("lcd32DirectlyWritePixel(%p, r=%d, c=%d, color=0x%04x)", dev, r, c, color);

    __lcd32StartTransaction(dev);

    // Set write region to one pixel at (r, c)
    __lcd32SetAddressWindow(dev, r, c, 1, 1);

    // Write pixel color
    __lcd32WriteCommand(dev, ILI9341_MEMORY_WRITE);
    __lcd32WriteData(dev, color);

    __lcd32StopTransaction(dev);

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
        lcd32SetCanvasPixel(lcd, x, cTopLeft, color);
        lcd32SetCanvasPixel(lcd, x, cBottomRight, color);
    }

    // Draw horizontal edges
    for (dim_t y = cTopLeft; y <= cBottomRight; ++y) {
        lcd32SetCanvasPixel(lcd, rTopLeft, y, color);
        lcd32SetCanvasPixel(lcd, rBottomRight, y, color);
    }

    // Recursive shrink for thicker borders
    return lcd32DrawEmptyRect(lcd,
                              rTopLeft + 1, cTopLeft + 1,
                              rBottomRight - 1, cBottomRight - 1,
                              edgeSize - 1, color);
}

def lcd32DrawChar(lcd32Dev_t *dev, dim_t r, dim_t c, char ch, GFXfont *f, color_t color){
    // --- Input Validation ---
    if (!dev) {
        __sys_err("[lcd32DrawChar] dev is NULL");
        return ERR_INVALID_ARG;
    }
    if (!f) {
        __sys_err("[lcd32DrawChar] f (font) is NULL");
        return ERR_INVALID_ARG;
    }

    // Check if char is supported by the font
    if (ch < f->first || ch > f->last) {
        // This log can be noisy if a string contains many unsupported chars.
        // __sys_err("[lcd32DrawChar] Char '0x%X' out of font range (0x%X-0x%X)", (unsigned int)ch, f->first, f->last);
        return ERR_INVALID_ARG; // Character not supported
    }

    // --- Get Glyph Info ---
    
    // Get glyph metadata for the specific char
    GFXglyph *glyph = &(f->glyph[ch - f->first]);
    
    // Get pointer to the font's shared bitmap data
    uint8_t  *bitmap = f->bitmap;

    // Get glyph-specific properties
    uint16_t  bo     = glyph->bitmapOffset; // Bitmap offset in the shared bitmap
    dim_t     w      = glyph->width;        // Glyph width (pixels)
    dim_t     h      = glyph->height;       // Glyph height (pixels)
    dim_t     xo     = glyph->xOffset;      // X offset from cursor
    dim_t     yo     = glyph->yOffset;      // Y offset from cursor (baseline)

    // --- Calculate Pixel Coordinates ---

    // (r, c) is the cursor origin (top-left of the char cell).
    // bitmap_top_r/c is the top-left corner where actual plotting starts.
    dim_t bitmap_top_r  = r + yo;
    dim_t bitmap_left_c = c + xo;

    uint8_t bits = 0;       // Current byte being processed from bitmap
    uint8_t bitCount = 0;   // Number of bits left in the 'bits' variable

    // --- Plotting Loop (Iterate through glyph pixels) ---
    
    // Loop over glyph height (Y-axis)
    for (dim_t yy = 0; yy < h; yy++) {
        // Loop over glyph width (X-axis)
        for (dim_t xx = 0; xx < w; xx++) {
            
            // Read a new byte from bitmap if current one is used up
            if (bitCount == 0) {
                bits = bitmap[bo++]; // Read next byte
                bitCount = 8;        // Reset bit counter
            }

            // Check if the highest bit (MSB) is set (pixel is ON)
            if (bits & 0x80) {
                // Calculate final pixel location on canvas
                dim_t pixel_r = bitmap_top_r + yy;
                dim_t pixel_c = bitmap_left_c + xx;

                // Boundary check (CRITICAL, as SetCanvasPixel has no check)
                if (pixel_r >= 0 && pixel_r < dev->canvas.maxRow &&
                    pixel_c >= 0 && pixel_c < dev->canvas.maxCol)
                {
                    // Plot the pixel
                    lcd32SetCanvasPixel(dev, pixel_r, pixel_c, color);
                }
            }
            
            bits <<= 1; // Move to the next bit
            bitCount--;
        }
    }
    
    return OKE;
}

def lcd32DrawText(lcd32Dev_t *dev, dim_t r, dim_t c, const char *str, GFXfont *f, color_t color) {
    
    // --- Input Validation ---
    if (!dev) {
        __sys_err("[lcd32DrawText] dev is NULL");
        return ERR_INVALID_ARG;
    }
    if (!f) {
        __sys_err("[lcd32DrawText] f (font) is NULL");
        return ERR_INVALID_ARG;
    }
    if (!str) {
        __sys_err("[lcd32DrawText] str is NULL");
        return ERR_INVALID_ARG;
    }

    // --- Handle Empty String ---
    if (*str == '\0') {
        return OKE; // Not an error, just nothing to draw
    }

    dim_t cursor_r = r;
    dim_t cursor_c = c;

    // --- Iterate through string ---
    while (*str) {
        char ch = *str++;

        // Handle newline
        if (ch == '\n') {
            cursor_c = c; // Reset column
            cursor_r += f->yAdvance; // Move to next line
            continue;
        }

        // Filter unsupported characters
        if (ch < f->first || ch > f->last) {
            continue;
        }
        
        GFXglyph *glyph = &(f->glyph[ch - f->first]);

        // Handle horizontal wrapping
        if (cursor_c + glyph->xAdvance >= dev->canvas.maxCol) {
            cursor_c = c; // Reset column
            cursor_r += f->yAdvance; // Move to next line
        }

        // Draw the character (relies on lcd32DrawChar)
        lcd32DrawChar(dev, cursor_r, cursor_c, ch, f, color);
        
        // Advance cursor
        cursor_c += glyph->xAdvance;
    }

    return OKE;
}

def lcd32DrawLine(lcd32Dev_t *dev, dim_t r0, dim_t c0, dim_t r1, dim_t c1, color_t color) {
    
    // --- Input Validation ---
    if (!dev) {
        __sys_err("[lcd32DrawLine] dev is NULL"); 
        return ERR_INVALID_ARG;
    }

    // --- Bresenham setup ---
    dim_t dx = abs(c1 - c0);
    dim_t sx = c0 < c1 ? 1 : -1;
    dim_t dy = -abs(r1 - r0);
    dim_t sy = r0 < r1 ? 1 : -1;
    dim_t err = dx + dy; // Error value e_xy

    // --- Plotting loop ---
    while (true) {
        
        // Plot pixel with boundary check
        if (r0 >= 0 && r0 < dev->canvas.maxRow && c0 >= 0 && c0 < dev->canvas.maxCol) {
            lcd32SetCanvasPixel(dev, r0, c0, color);
        }
        
        // Check stop condition
        if (c0 == c1 && r0 == r1) break;
        
        // Bresenham iteration
        dim_t e2 = err << 1; // Optimized (e2 = 2 * err)
        if (e2 >= dy) {
            err += dy;
            c0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            r0 += sy;
        }
    }
    return OKE;
}

def lcd32DrawThickLine(lcd32Dev_t *dev, dim_t r0, dim_t c0, dim_t r1, dim_t c1, color_t color, dim_t thickness) {
    if (thickness <= 0) {
    }
    if (thickness == 1) {
        return lcd32DrawLine(dev, r0, c0, r1, c1, color);
    }

    dim_t dx = abs(c1 - c0);
    dim_t sx = c0 < c1 ? 1 : -1;
    dim_t dy = -abs(r1 - r0);
    dim_t sy = r0 < r1 ? 1 : -1;
    dim_t err = dx + dy;

    dim_t t_start = -(thickness - 1) / 2;
    dim_t t_end = thickness / 2;
    
    const dim_t max_r = dev->canvas.maxRow;
    const dim_t max_c = dev->canvas.maxCol;

    const bool is_steep = abs(dy) > dx;

    while (true) {
        
        if (is_steep) {
            for (dim_t i = t_start; i <= t_end; i++) {
                dim_t current_c = c0 + i;
                if (current_c >= 0 && current_c < max_c && r0 >= 0 && r0 < max_r) {
                    lcd32SetCanvasPixel(dev, r0, current_c, color);
                }
            }
        } else {
            for (dim_t i = t_start; i <= t_end; i++) {
                dim_t current_r = r0 + i;
                if (current_r >= 0 && current_r < max_r && c0 >= 0 && c0 < max_c) {
                    lcd32SetCanvasPixel(dev, current_r, c0, color);
                }
            }
        }
        
        if (c0 == c1 && r0 == r1) break;

        dim_t e2 = err << 1; 
        if (e2 >= dy) {
            err += dy;
            c0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            r0 += sy;
        }
    }
    return OKE;
}