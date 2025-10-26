#include "lcd32.h"

/// OTHER FUNCTIONS /////////////////////////////////////////////////////////////////////////////////

/// @brief [Internal] Delete canvas buffer
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

#ifdef LCD32_USE_PSRAM_FOR_LCD_CANVAS == 1
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

/// @brief [Internal] Config Control/Data pin
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

/// @brief [Global] Create new LCD 3.2" object then store to devPtr.
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

/// @brief [Global] Config new LCD 3.2".
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

/// @brief [Global] Initialize and start up the LCD 3.2" (ILI9341 controller).
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

/// @brief [Global] Flush the entire canvas to the LCD (optimized inline window setup)
def lcd32FlushCanvas(lcd32Dev_t *dev) {
    // __lcd32Entry("lcd32FlushCanvas(%p)", dev);

    // --- Null check ---
    __lcd32NULLCheck(dev, STR(dev), STR(lcd32FlushCanvas), return ERR_NULL;);
    lcd32Canvas_t *canvas = &dev->canvas;

    // --- Begin LCD bus transaction (assert CS, prepare write mode) ---
    __lcd32StartTransaction(dev);

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

    // __lcd32Exit("lcd32FlushCanvas() : OKE");
    return OKE;
}

/// @brief [Global] Write a single pixel directly to the LCD.
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
