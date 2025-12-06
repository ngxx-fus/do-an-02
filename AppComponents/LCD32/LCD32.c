/**
 * @file LCD32.c
 * @brief Driver Implementation for 320x240 LCD (ILI9341 via P16Com)
 * @author Nguyen Thanh Phu
 */

#include "LCD32.h"
#include <math.h> // For abs, ceilf, floorf

/// @brief Allocates memory for a new LCD32Dev_t object and Canvas
LCD32Dev_t * LCD32New(){
    LCD32Dev_t * DevPtr = (LCD32Dev_t *) malloc(sizeof(LCD32Dev_t));
    if(IsNull(DevPtr)){
        LCD32Err("[LCD32New] Malloc failed for Struct");
        return NULL;
    }

    /// Reset P16Com part (Initialize internal arrays to -1)
    REPN(i, P16COM_DAT_PIN_NUM_W_PADDING){
        (DevPtr->P16Com).DatPinArr[i] = -1;
    }
    REPN(i, P16COM_CTL_PIN_NUM_W_PADDING){
        (DevPtr->P16Com).CtlPinArr[i] = -1;
    }
    
    /// Preset internal P16Com flags/masks
    (DevPtr->P16Com).StatusFlag = 0;
    (DevPtr->P16Com).CtlIOMask = 0;
    (DevPtr->P16Com).DatIOMask = 0;

    /// Preset LCD specific fields
    DevPtr->BrightLight = -1;
    DevPtr->Orientation = LCD32_DEFAULT_ORIENTATION;
    DevPtr->Width       = LCD32_DEFAULT_W;
    DevPtr->Height      = LCD32_DEFAULT_H;
    
    #if (LCD32_THREAD_SAFE_EN == 1)
        /// Init Mutex
        DevPtr->mutex = xSemaphoreCreateMutex();
        if (IsNull(DevPtr->mutex)) {
            LCD32Err("[LCD32New] Failed to create mutex");
            free(DevPtr);
            return NULL;
        }
    #endif

    /// Allocate Canvas
    #if (LCD32_CANVAS_IN_PSRAM_EN == 1)
        /// Allocate in PSRAM (approx 150KB for 320x240x16bit)
        DevPtr->Canvas = (Color_t *)heap_caps_malloc(sizeof(Color_t) * (DevPtr->Width) * (DevPtr->Height), MALLOC_CAP_SPIRAM);
    #else
        /// Allocate in DRAM
        DevPtr->Canvas = (Color_t *)malloc(sizeof(Color_t) * (DevPtr->Width) * (DevPtr->Height));
    #endif

    if(IsNull(DevPtr->Canvas)){
        LCD32Err("[LCD32New] Malloc failed for Canvas");
        #if (LCD32_THREAD_SAFE_EN == 1)
            if (DevPtr->mutex) vSemaphoreDelete(DevPtr->mutex);
        #endif
        free(DevPtr);
        return NULL;
    }

    /// Clear Canvas (Black)
    for(int32_t i = 0; i < (DevPtr->Width * DevPtr->Height); i++){
        (DevPtr->Canvas)[i] = COLOR_BLACK;
    }

    LCD32Log("[LCD32New] Created %p (Canvas @ %p)", DevPtr, DevPtr->Canvas);
    return DevPtr;
}

/// @brief Deallocate memory for LCD32Dev_t object and Canvas
void LCD32Delete(LCD32Dev_t * Dev){
    if(Dev != NULL){
        if(Dev->Canvas != NULL){
            free(Dev->Canvas);
        }
        #if (LCD32_THREAD_SAFE_EN == 1)
            if (Dev->mutex) {
                vSemaphoreDelete(Dev->mutex);
            }
        #endif
        free(Dev);
    }
}

/// @brief Configure pin mapping for LCD
DefaultRet_t LCD32Config(LCD32Dev_t * Dev, const Pin_t * CtlPins, const Pin_t * DatPins){
    LCD32Entry("LCD32Config(%p, %p, %p)", Dev, CtlPins, DatPins);

    if(IsNull(Dev) || IsNull(CtlPins) || IsNull(DatPins)){
        LCD32ReturnWithLog(STAT_ERR_NULL, "LCD32Config() : STAT_ERR_NULL");
    }

    /// 1. Extract BrightLight (Index 5 in User Array)
    /// User array expected: [RD, WR, CS, RS, RST, BL]
    if(IsStandardPin(CtlPins[5])){
        Dev->BrightLight = CtlPins[5];
    } else {
        LCD32Err("[LCD32Config] Invalid BrightLight Pin");
        LCD32ReturnWithLog(STAT_ERR_INVALID_ARG, "LCD32Config() : STAT_ERR_INVALID_ARG");
    }

    /// 2. Config Control Pins (First 5)
    DefaultRet_t ret = P16ComConfigCtl(&(Dev->P16Com), CtlPins);
    if(ret != STAT_OKE){
        LCD32ReturnWithLog(ret, "LCD32Config() : Error");
    }

    /// 3. Config Data Pins (16)
    ret = P16ComConfigDat(&(Dev->P16Com), DatPins);
    if(ret != STAT_OKE){
        LCD32ReturnWithLog(ret, "LCD32Config() : Error");
    }

    LCD32ReturnWithLog(STAT_OKE, "LCD32Config() : STAT_OKE");
}

/// @brief Initialize the LCD (GPIOs, Reset, Startup Sequence)
DefaultRet_t LCD32Init(LCD32Dev_t * Dev){
    LCD32Entry("LCD32Init(%p)", Dev);
    DefaultRet_t ret;
    P16Dev_t * P16Dev = &(Dev->P16Com);

    /// 1. Initialize P16 Bus (GPIOs, Directions)
    ret = P16ComInit(P16Dev);
    if(ret != STAT_OKE){
        LCD32ReturnWithLog(ret, "LCD32Init() : Error");
    }

    /// 2. Initialize BrightLight Pin
    if(IsValidPin(Dev->BrightLight)){
        IOConfigAsOutput(Mask64(Dev->BrightLight), -1, -1);
        LCD32SetHighBrightLightPin(Dev); // Turn off/Default state
    }

    /// 3. Hardware Reset Sequence
    P16SetLowResetPin(P16Dev);
    P16BlockingDelay(10000);   // 10ms
    P16SetHighResetPin(P16Dev);
    P16BlockingDelay(120000);  // 120ms wait after reset

    /// 4. Initial Pin States
    P16SetHighChipSelPin(P16Dev);
    P16SetHighReadPin(P16Dev);
    P16SetHighWritePin(P16Dev);

    // VERIFY DEVICE ID (0xD3 Read ID4)
    LCD32StartTransaction(Dev);
    
    // Command 0xD3: Read ID4
    LCD32WriteCmd(Dev, 0xD3);
    
    // Switch to Data Mode (D/CX = 1) for reading parameters
    LCD32SetDataTransaction(Dev);

    // Read Sequence: Dummy -> Dummy (00) -> ID High (93) -> ID Low (41)
    // Note: P16ComRead handles direction switching and strobe logic
    volatile P16Data_t d1 = P16ComRead(P16Dev); // Dummy
    volatile P16Data_t d2 = P16ComRead(P16Dev); // Dummy (Should be 0x00)
    volatile P16Data_t d3 = P16ComRead(P16Dev); // ID High (Should be 0x93)
    volatile P16Data_t d4 = P16ComRead(P16Dev); // ID Low  (Should be 0x41)

    LCD32StopTransaction(Dev);

    uint16_t devId = ((d3 & 0xFF) << 8) | (d4 & 0xFF);
    LCD32Log("[LCD32Init] Read ID: 0x%04X (Raw: %02X %02X %02X %02X)", devId, d1, d2, d3, d4);

    if (devId != 0x9341) {
        LCD32Err("[LCD32Init] Invalid Device ID: 0x%04X (Expected 0x9341)", devId);
        // LCD32ReturnWithLog(STAT_ERR_UNSUPPORTED, "LCD32Init() : Wrong Device ID");
        // Uncomment above line to enforce check, currently just logging error for debugging
    }

    // ILI9341 INITIALIZATION SEQUENCE
    LCD32StartTransaction(Dev);

    // Power Control B
    LCD32WriteCmd(Dev, ILI9341_POWER_CONTROL_B);
    LCD32WriteData(Dev, 0x00);
    LCD32WriteData(Dev, 0xEA);
    LCD32WriteData(Dev, 0xF0);

    // Power On Sequence Control
    LCD32WriteCmd(Dev, ILI9341_POWER_ON_SEQ_CTRL);
    LCD32WriteData(Dev, 0x64);
    LCD32WriteData(Dev, 0x03);
    LCD32WriteData(Dev, 0x12);
    LCD32WriteData(Dev, 0x81);

    // Driver Timing Control A
    LCD32WriteCmd(Dev, ILI9341_DRIVER_TIMING_CTRL_A_INT);
    LCD32WriteData(Dev, 0x85);
    LCD32WriteData(Dev, 0x10);
    LCD32WriteData(Dev, 0x78);

    // Power Control A
    LCD32WriteCmd(Dev, ILI9341_POWER_CONTROL_A);
    LCD32WriteData(Dev, 0x39);
    LCD32WriteData(Dev, 0x2C);
    LCD32WriteData(Dev, 0x00);
    LCD32WriteData(Dev, 0x33);
    LCD32WriteData(Dev, 0x06);

    // Pump Ratio Control
    LCD32WriteCmd(Dev, ILI9341_PUMP_RATIO_CONTROL);
    LCD32WriteData(Dev, 0x20);

    // Driver Timing Control B
    LCD32WriteCmd(Dev, ILI9341_DRIVER_TIMING_CTRL_B);
    LCD32WriteData(Dev, 0x00);
    LCD32WriteData(Dev, 0x00);

    // Power Control 1
    LCD32WriteCmd(Dev, ILI9341_POWER_CONTROL_1);
    LCD32WriteData(Dev, 0x21);

    // Power Control 2
    LCD32WriteCmd(Dev, ILI9341_POWER_CONTROL_2);
    LCD32WriteData(Dev, 0x10);

    // VCOM Control 1
    LCD32WriteCmd(Dev, ILI9341_VCOM_CONTROL_1);
    LCD32WriteData(Dev, 0x4F);
    LCD32WriteData(Dev, 0x38);

    // VCOM Control 2
    LCD32WriteCmd(Dev, ILI9341_VCOM_CONTROL_2);
    LCD32WriteData(Dev, 0xB7);

    // Memory Access Control
    LCD32WriteCmd(Dev, ILI9341_MEMORY_ACCESS_CONTROL);
    LCD32WriteData(Dev, 0x48); // BGR Order

    // Pixel Format Set
    LCD32WriteCmd(Dev, ILI9341_PIXEL_FORMAT_SET);
    LCD32WriteData(Dev, 0x55); // 16-bit

    // Frame Rate Control
    LCD32WriteCmd(Dev, ILI9341_FRAME_RATE_NORMAL);
    LCD32WriteData(Dev, 0x00);
    LCD32WriteData(Dev, 0x1B);

    // Display Function Control
    LCD32WriteCmd(Dev, ILI9341_DISPLAY_FUNCTION_CTRL);
    LCD32WriteData(Dev, 0x08);
    LCD32WriteData(Dev, 0x82);
    LCD32WriteData(Dev, 0x27);

    // 3G Gamma
    LCD32WriteCmd(Dev, ILI9341_ENABLE_3G);
    LCD32WriteData(Dev, 0x00);

    // Gamma Set
    LCD32WriteCmd(Dev, ILI9341_GAMMA_SET);
    LCD32WriteData(Dev, 0x01);

    // Positive Gamma Correction
    LCD32WriteCmd(Dev, ILI9341_POSITIVE_GAMMA_CORR);
    LCD32WriteData(Dev, 0x0F); LCD32WriteData(Dev, 0x31); LCD32WriteData(Dev, 0x2B);
    LCD32WriteData(Dev, 0x0C); LCD32WriteData(Dev, 0x0E); LCD32WriteData(Dev, 0x08);
    LCD32WriteData(Dev, 0x4E); LCD32WriteData(Dev, 0xF1); LCD32WriteData(Dev, 0x37);
    LCD32WriteData(Dev, 0x07); LCD32WriteData(Dev, 0x10); LCD32WriteData(Dev, 0x03);
    LCD32WriteData(Dev, 0x0E); LCD32WriteData(Dev, 0x09); LCD32WriteData(Dev, 0x00);

    // Negative Gamma Correction
    LCD32WriteCmd(Dev, ILI9341_NEGATIVE_GAMMA_CORR);
    LCD32WriteData(Dev, 0x00); LCD32WriteData(Dev, 0x0E); LCD32WriteData(Dev, 0x14);
    LCD32WriteData(Dev, 0x03); LCD32WriteData(Dev, 0x11); LCD32WriteData(Dev, 0x07);
    LCD32WriteData(Dev, 0x31); LCD32WriteData(Dev, 0xC1); LCD32WriteData(Dev, 0x48);
    LCD32WriteData(Dev, 0x08); LCD32WriteData(Dev, 0x0F); LCD32WriteData(Dev, 0x0C);
    LCD32WriteData(Dev, 0x31); LCD32WriteData(Dev, 0x36); LCD32WriteData(Dev, 0x0F);

    // Sleep Out
    LCD32WriteCmd(Dev, ILI9341_SLEEP_OUT);
    P16BlockingDelay(120000);

    // Display On
    LCD32WriteCmd(Dev, ILI9341_DISPLAY_ON);
    
    LCD32StopTransaction(Dev);

    /// 6. Turn On Backlight (Active Low logic per macro)
    LCD32SetLowBrightLightPin(Dev);

    Dev->StatusFlag |= LCD32_INITIALIZED;

    LCD32ReturnWithLog(STAT_OKE, "LCD32Init() : STAT_OKE");
}

/// @brief Re-run configuration logic
DefaultRet_t LCD32ReConfig(LCD32Dev_t * Dev){
    LCD32Entry("LCD32ReConfig(%p)", Dev);
    return LCD32Init(Dev);
}

/// @brief Helper to write Command
void LCD32WriteCmd(LCD32Dev_t * Dev, uint16_t Cmd){
    LCD32SetCommandTransaction(Dev);
    P16ComWrite(&(Dev->P16Com), Cmd);
}

/// @brief Helper to write Data
void LCD32WriteData(LCD32Dev_t * Dev, uint16_t Data){
    LCD32SetDataTransaction(Dev);
    P16ComWrite(&(Dev->P16Com), Data);
}

/// @brief Set the active drawing area window on the LCD
void LCD32SetAddressWindow(LCD32Dev_t * Dev, Dim_t x, Dim_t y, Dim_t w, Dim_t h){
    LCD32StartTransaction(Dev);

    // Column Address Set
    LCD32WriteCmd(Dev, ILI9341_COLUMN_ADDRESS_SET);
    LCD32WriteData(Dev, x >> 8);
    LCD32WriteData(Dev, x & 0xFF);
    LCD32WriteData(Dev, (x + w - 1) >> 8);
    LCD32WriteData(Dev, (x + w - 1) & 0xFF);

    // Page Address Set
    LCD32WriteCmd(Dev, ILI9341_PAGE_ADDRESS_SET);
    LCD32WriteData(Dev, y >> 8);
    LCD32WriteData(Dev, y & 0xFF);
    LCD32WriteData(Dev, (y + h - 1) >> 8);
    LCD32WriteData(Dev, (y + h - 1) & 0xFF);

    // Memory Write
    LCD32WriteCmd(Dev, ILI9341_MEMORY_WRITE);
    
    LCD32StopTransaction(Dev);
}

/// @brief Flush the internal Canvas buffer to the display
void LCD32FlushCanvas(LCD32Dev_t * Dev){
    // LCD32Entry("LCD32FlushCanvas(%p)", Dev);
    
    if(IsNull(Dev) || IsNull(Dev->Canvas)){
        return;
    }

    #if (LCD32_THREAD_SAFE_EN == 1)
    if (xSemaphoreTake(Dev->mutex, portMAX_DELAY) != pdTRUE) {
        LCD32Err("[LCD32FlushCanvas] Failed to take mutex");
        return;
    }
    #endif

    /// 1. Set Address Window to Full Screen
    LCD32SetAddressWindow(Dev, 0, 0, Dev->Width, Dev->Height);
    /// 2. Start Data Stream
    LCD32StartTransaction(Dev);
    LCD32SetDataTransaction(Dev);
    /// 3. Burst Write Pixels using P16Com optimized driver
    P16ComWriteArray(&(Dev->P16Com), (P16Data_t *)Dev->Canvas, (Dev->Width * Dev->Height));
    LCD32StopTransaction(Dev);

    #if (LCD32_THREAD_SAFE_EN == 1)
    xSemaphoreGive(Dev->mutex);
    #endif
}

/// @brief Fill the entire canvas with a single color
void LCD32FillCanvas(LCD32Dev_t *Dev, Color_t Color) {
    if (IsNull(Dev) || IsNull(Dev->Canvas)) return;
    int32_t size = Dev->Width * Dev->Height;
    for (int32_t i = 0; i < size; i++) {
        Dev->Canvas[i] = Color;
    }
}

/// @brief Draw a single pixel on the canvas (with bounds check)
void LCD32SetCanvasPixel(LCD32Dev_t *Dev, Dim_t Row, Dim_t Col, Color_t Color) {
    if (Row >= 0 && Row < Dev->Height && Col >= 0 && Col < Dev->Width) {
        Dev->Canvas[Row * Dev->Width + Col] = Color;
    }
}

/// @brief Write a single pixel directly to the LCD (bypassing canvas)
void LCD32DirectlyWritePixel(LCD32Dev_t *Dev, Dim_t Row, Dim_t Col, Color_t Color) {
    if (Row >= 0 && Row < Dev->Height && Col >= 0 && Col < Dev->Width) {
        #if (LCD32_THREAD_SAFE_EN == 1)
        if (xSemaphoreTake(Dev->mutex, portMAX_DELAY) != pdTRUE) {
            LCD32Err("[LCD32DirectlyWritePixel] Failed to take mutex");
            return;
        }
        #endif

        LCD32SetAddressWindow(Dev, Col, Row, 1, 1); // Note: AddressWindow usually takes x, y (Col, Row)
        LCD32StartTransaction(Dev);
        LCD32SetDataTransaction(Dev);
        P16ComWrite(&(Dev->P16Com), Color);
        LCD32StopTransaction(Dev);
        #if (LCD32_THREAD_SAFE_EN == 1)
        xSemaphoreGive(Dev->mutex);
        #endif
    }
}

/* --- DRAWING PRIMITIVES (Ported from Old Code) --- */

DefaultRet_t LCD32DrawLine(LCD32Dev_t *Dev, Dim_t r0, Dim_t c0, Dim_t r1, Dim_t c1, Color_t Color) {
    if (IsNull(Dev)) return STAT_ERR_NULL;

    int16_t dx = abs(c1 - c0), sx = c0 < c1 ? 1 : -1;
    int16_t dy = -abs(r1 - r0), sy = r0 < r1 ? 1 : -1;
    int16_t err = dx + dy, e2;

    while (1) {
        LCD32SetCanvasPixel(Dev, r0, c0, Color);
        if (r0 == r1 && c0 == c1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; c0 += sx; }
        if (e2 <= dx) { err += dx; r0 += sy; }
    }
    return STAT_OKE;
}

/// @brief Draw a filled circle on the canvas
static void LCD32DrawFilledCircle(LCD32Dev_t *Dev, Dim_t x0, Dim_t y0, Dim_t r, Color_t Color) {
    int x = -r, y = 0, err = 2 - 2 * r;
    do {
        LCD32DrawLine(Dev, y0 - y, x0 + x, y0 - y, x0 - x, Color);
        LCD32DrawLine(Dev, y0 + y, x0 + x, y0 + y, x0 - x, Color);
        r = err;
        if (r > x) err += ++x * 2 + 1;
        if (r <= y) err += ++y * 2 + 1;
    } while (x < 0);
}

/// @brief Helper to draw a filled quadrilateral (4-sided polygon)
static void LCD32DrawQuad(LCD32Dev_t *Dev, const LCDPoint_t p[4], Color_t Color) {
    LCDPoint_t tri1[3] = { p[0], p[1], p[2] };
    LCDPoint_t tri2[3] = { p[0], p[2], p[3] };
    LCD32DrawFilledPolygon(Dev, tri1, 3, Color);
    LCD32DrawFilledPolygon(Dev, tri2, 3, Color);
}

DefaultRet_t LCD32DrawThickLine(LCD32Dev_t *Dev, Dim_t r0, Dim_t c0, Dim_t r1, Dim_t c1, Color_t Color, Dim_t Thickness) {
    if (IsNull(Dev)) return STAT_ERR_NULL;
    if (Thickness <= 1) return LCD32DrawLine(Dev, r0, c0, r1, c1, Color);

    // Advanced implementation using polygon rendering for sharp corners and flat ends
    int16_t dx = c1 - c0;
    int16_t dy = r1 - r0;
    float len = sqrtf(dx * dx + dy * dy);
    if (len == 0) return STAT_OKE;

    // Calculate the perpendicular vector
    float nx = -dy / len;
    float ny = dx / len;

    float half_thick = (float)Thickness / 2.0f;

    // Calculate the 4 corners of the rectangle representing the thick line
    LCDPoint_t p[4];
    p[0] = (LCDPoint_t){ .row = (Dim_t)roundf(r0 - ny * half_thick), .col = (Dim_t)roundf(c0 - nx * half_thick) };
    p[1] = (LCDPoint_t){ .row = (Dim_t)roundf(r1 - ny * half_thick), .col = (Dim_t)roundf(c1 - nx * half_thick) };
    p[2] = (LCDPoint_t){ .row = (Dim_t)roundf(r1 + ny * half_thick), .col = (Dim_t)roundf(c1 + nx * half_thick) };
    p[3] = (LCDPoint_t){ .row = (Dim_t)roundf(r0 + ny * half_thick), .col = (Dim_t)roundf(c0 + nx * half_thick) };

    // Draw the rectangle as two filled triangles
    LCDPoint_t tri1[3] = { p[0], p[1], p[2] };
    LCDPoint_t tri2[3] = { p[0], p[2], p[3] };

    LCD32DrawFilledPolygon(Dev, tri1, 3, Color);
    LCD32DrawFilledPolygon(Dev, tri2, 3, Color);

    return STAT_OKE;
}

/// @brief Helper to draw a filled rectangle on the canvas
static void LCD32DrawFilledRect(LCD32Dev_t *Dev, Dim_t r, Dim_t c, Dim_t h, Dim_t w, Color_t Color) {
    if (IsNull(Dev)) return;
    for (Dim_t i = r; i < r + h; i++) {
        for (Dim_t j = c; j < c + w; j++) {
            LCD32SetCanvasPixel(Dev, i, j, Color);
        }
    }
}

DefaultRet_t LCD32DrawEmptyRect(LCD32Dev_t *Dev, Dim_t rTopLeft, Dim_t cTopLeft, Dim_t rBottomRight, Dim_t cBottomRight, Dim_t EdgeSize, Color_t Color) {
    if (IsNull(Dev)) return STAT_ERR_NULL;
    if (EdgeSize < 1) return STAT_OKE;

    Dim_t width = cBottomRight - cTopLeft + 1;
    Dim_t height = rBottomRight - rTopLeft + 1;

    // Draw 4 filled rectangles for the border
    LCD32DrawFilledRect(Dev, rTopLeft, cTopLeft, EdgeSize, width, Color); // Top
    LCD32DrawFilledRect(Dev, rBottomRight - EdgeSize + 1, cTopLeft, EdgeSize, width, Color); // Bottom
    LCD32DrawFilledRect(Dev, rTopLeft + EdgeSize, cTopLeft, height - 2 * EdgeSize, EdgeSize, Color); // Left
    LCD32DrawFilledRect(Dev, rTopLeft + EdgeSize, cBottomRight - EdgeSize + 1, height - 2 * EdgeSize, EdgeSize, Color); // Right

    return STAT_OKE;
}

DefaultRet_t LCD32DrawPolygon(LCD32Dev_t *Dev, const LCDPoint_t *Points, size_t N, Color_t Color) {
    if (IsNull(Dev) || IsNull(Points) || N < 2) return STAT_ERR_INVALID_ARG;

    for (size_t i = 0; i < N - 1; i++) {
        LCD32DrawLine(Dev, Points[i].row, Points[i].col, Points[i+1].row, Points[i+1].col, Color);
    }
    // Close polygon
    LCD32DrawLine(Dev, Points[N-1].row, Points[N-1].col, Points[0].row, Points[0].col, Color);
    return STAT_OKE;
}

// Helper function for qsort
static int compare_int32(const void *a, const void *b) {
    return (*(int32_t *)a - *(int32_t *)b);
}

DefaultRet_t LCD32DrawFilledPolygon(LCD32Dev_t *Dev, const LCDPoint_t *Points, size_t N, Color_t Color) {
    if (IsNull(Dev) || IsNull(Points) || N < 3) return STAT_ERR_INVALID_ARG;

    Dim_t minRow = Points[0].row, maxRow = Points[0].row;
    for (size_t i = 1; i < N; i++) {
        if (Points[i].row < minRow) minRow = Points[i].row;
        if (Points[i].row > maxRow) maxRow = Points[i].row;
    }
    if (minRow < 0) minRow = 0;
    if (maxRow >= Dev->Height) maxRow = Dev->Height - 1;

    int32_t interX[128]; // Max 128 intersections

    for (Dim_t y = minRow; y <= maxRow; y++) {
        size_t interCount = 0;
        for (size_t i = 0; i < N; i++) {
            size_t j = (i + 1) % N;
            int32_t x0 = Points[i].col, y0 = Points[i].row;
            int32_t x1 = Points[j].col, y1 = Points[j].row;

            if (y0 == y1) continue; // Horizontal edge
            if (y0 > y1) { // Swap
                int32_t tx = x0; x0 = x1; x1 = tx;
                int32_t ty = y0; y0 = y1; y1 = ty;
            }

            if (y >= y0 && y < y1) {
                // Calculate intersection
                int32_t x = x0 + (int32_t)((int64_t)(x1 - x0) * (y - y0) / (y1 - y0));
                if (interCount < 128) interX[interCount++] = x;
            }
        }

        // Sort intersections using qsort for better performance
        qsort(interX, interCount, sizeof(int32_t), compare_int32);

        // Fill pairs
        for (size_t k = 0; k + 1 < interCount; k += 2) {
            Dim_t xStart = (Dim_t)interX[k];
            Dim_t xEnd = (Dim_t)interX[k+1];
            if (xStart < 0) xStart = 0;
            if (xEnd >= Dev->Width) xEnd = Dev->Width - 1;
            
            for (Dim_t x = xStart; x <= xEnd; x++) {
                LCD32SetCanvasPixel(Dev, y, x, Color);
            }
        }
    }
    return STAT_OKE;
}

DefaultRet_t LCD32DrawChar(LCD32Dev_t *Dev, Dim_t r, Dim_t c, char Ch, const GFXfont *Font, Color_t Color) {
    if (IsNull(Dev) || IsNull(Font)) return STAT_ERR_NULL;
    if (Ch < Font->first || Ch > Font->last) return STAT_ERR_INVALID_ARG;

    GFXglyph *glyph = &Font->glyph[Ch - Font->first];
    uint8_t *bitmap = Font->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t  w  = glyph->width, h = glyph->height;
    int8_t   xo = glyph->xOffset, yo = glyph->yOffset;
    
    uint8_t bits = 0, bitCount = 0;

    for (Dim_t yy = 0; yy < h; yy++) {
        for (Dim_t xx = 0; xx < w; xx++) {
            if (bitCount == 0) {
                bits = bitmap[bo++];
                bitCount = 8;
            }
            if (bits & 0x80) {
                LCD32SetCanvasPixel(Dev, r + yo + yy, c + xo + xx, Color);
            }
            bits <<= 1;
            bitCount--;
        }
    }
    return STAT_OKE;
}

DefaultRet_t LCD32DrawText(LCD32Dev_t *Dev, Dim_t r, Dim_t c, const char *Str, const GFXfont *Font, Color_t Color) {
    if (IsNull(Dev) || IsNull(Str) || IsNull(Font)) return STAT_ERR_NULL;

    Dim_t cursor_r = r, cursor_c = c;
    while (*Str) {
        char ch = *Str++;
        if (ch == '\n') {
            cursor_c = c;
            cursor_r += Font->yAdvance;
            continue;
        }
        if (ch < Font->first || ch > Font->last) continue;

        GFXglyph *glyph = &Font->glyph[ch - Font->first];
        
        // Wrap text
        if (cursor_c + glyph->xAdvance >= Dev->Width) {
            cursor_c = c;
            cursor_r += Font->yAdvance;
        }

        LCD32DrawChar(Dev, cursor_r, cursor_c, ch, Font, Color);
        cursor_c += glyph->xAdvance;
    }
    return STAT_OKE;
}