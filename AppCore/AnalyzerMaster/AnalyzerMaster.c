#include "All.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#define ANALYZER_MASTER_LOCAL_UTILS /// Do not deleted!

#include "driver/spi_master.h"
#include "driver/gpio.h"    /// Added for manual GPIO control
#include "rom/ets_sys.h"    /// Added for ets_delay_us

/// @brief Current configured size of the RX buffer.
const HalfWord_t AnalyzerMasterRxSize = ANALYZER_MASTER_RX_SIZE;
/// @brief Current configured size of the TX buffer.
const HalfWord_t AnalyzerMasterTxSize = ANALYZER_MASTER_TX_SIZE;
/// @brief Pointer to the RX data buffer.
uint16_t * AnalyzerMasterRx     = NULL ;
/// @brief Pointer to the TX data buffer.
uint16_t * AnalyzerMasterTx     = NULL ;
/// @brief CBuff for RX
CBuff_t * AnalyzerMasterCBuff  = NULL;
/// @brief LCD32
LCD32Dev_t * lcd32 = NULL; 

#ifdef ANALYZER_MASTER_LOCAL_UTILS

/// @brief Statically allocated Look-Up Table for the main LCD to save HEAP memory.
static P16Lut_t lcd_lut;

static portMUX_TYPE __AnalyzerReaderComLock = portMUX_INITIALIZER_UNLOCKED;

/**
 * @brief Helper to generate random coordinates with an out-of-bounds margin.
 * @param max_val The maximum valid coordinate (e.g., width or height).
 * @param margin The margin to extend beyond the valid range.
 * @return A random coordinate between -margin and (max_val + margin).
 */
static Dim_t RandCoordinate(Dim_t max_val, int16_t margin) {
    return (esp_random() % (max_val + 2 * margin)) - margin;
}

void PerformScreenTest(LCD32Dev_t * lcd32) {
    if (!lcd32) return;

    // --- Test 0: Performance Measurement with LUT optimization ---
    {
        Color_t random_color = (Color_t)esp_random();
        int64_t start_time, fill_end_time, flush_end_time;
        uint32_t fill_duration_ms, flush_duration_ms;

        // --- Measure LCD32FlushCanvas (which now uses LUT) ---
        SysLog("[TaskScreen] Testing: Performance of Fill + Flush (with LUT)");
        start_time = esp_timer_get_time();
        
        LCD32FillCanvas(lcd32, random_color);
        fill_end_time = esp_timer_get_time();
        
        LCD32FlushCanvas(lcd32);
        flush_end_time = esp_timer_get_time();
        
        fill_duration_ms = (uint32_t)((fill_end_time - start_time) / 1000);
        flush_duration_ms = (uint32_t)((flush_end_time - fill_end_time) / 1000);
        
        SysLog("[TaskScreen] LUT Optimized -> Fill: %u ms, Flush: %u ms", fill_duration_ms, flush_duration_ms);
        DelayMs(1000);
    }

    // --- Test 1: LCD32SetCanvasPixel ---
    SysLog("[TaskScreen] Testing: LCD32SetCanvasPixel");
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    for (int i = 0; i < 3000; i++) {
        Dim_t r = RandCoordinate(lcd32->Height, 50);
        Dim_t c = RandCoordinate(lcd32->Width, 50);
        LCD32SetCanvasPixel(lcd32, r, c, (Color_t)esp_random());
    }
    LCD32FlushCanvas(lcd32);
    DelayMs(500);

    // --- Test 2: LCD32DirectlyWritePixel ---
    SysLog("[TaskScreen] Testing: LCD32DirectlyWritePixel");
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    LCD32FlushCanvas(lcd32); // Flush background first
    for (int i = 0; i < 3000; i++) {
        Dim_t r = RandCoordinate(lcd32->Height, 50);
        Dim_t c = RandCoordinate(lcd32->Width, 50);
        LCD32DirectlyWritePixel(lcd32, r, c, (Color_t)esp_random());
    }
    DelayMs(500);

    // --- Test 3: LCD32DrawLine ---
    SysLog("[TaskScreen] Testing: LCD32DrawLine");
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    for (int i = 0; i < 20; i++) {
        Dim_t r0 = RandCoordinate(lcd32->Height, 50);
        Dim_t c0 = RandCoordinate(lcd32->Width, 50);
        Dim_t r1 = RandCoordinate(lcd32->Height, 50);
        Dim_t c1 = RandCoordinate(lcd32->Width, 50);
        LCD32DrawLine(lcd32, r0, c0, r1, c1, (Color_t)esp_random());
    }
    LCD32FlushCanvas(lcd32);
    DelayMs(500);

    // --- Test 4: LCD32DrawThickLine ---
    SysLog("[TaskScreen] Testing: LCD32DrawThickLine");
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    for (int i = 0; i < 10; i++) {
        Dim_t r0 = RandCoordinate(lcd32->Height, 50);
        Dim_t c0 = RandCoordinate(lcd32->Width, 50);
        Dim_t r1 = RandCoordinate(lcd32->Height, 50);
        Dim_t c1 = RandCoordinate(lcd32->Width, 50);
        Dim_t thickness = (esp_random() % 15) + 1;
        LCD32DrawThickLine(lcd32, r0, c0, r1, c1, (Color_t)esp_random(), thickness);
    }
    LCD32FlushCanvas(lcd32);
    DelayMs(500);

    // --- Test 5: LCD32DrawEmptyRect ---
    SysLog("[TaskScreen] Testing: LCD32DrawEmptyRect");
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    for (int i = 0; i < 10; i++) {
        Dim_t r0 = RandCoordinate(lcd32->Height, 50);
        Dim_t c0 = RandCoordinate(lcd32->Width, 50);
        Dim_t r1 = RandCoordinate(lcd32->Height, 50);
        Dim_t c1 = RandCoordinate(lcd32->Width, 50);
        Dim_t edge = (esp_random() % 10) + 2;
        if (r0 > r1) { Dim_t tmp = r0; r0 = r1; r1 = tmp; }
        if (c0 > c1) { Dim_t tmp = c0; c0 = c1; c1 = tmp; }
        LCD32DrawEmptyRect(lcd32, r0, c0, r1, c1, edge, (Color_t)esp_random());
    }
    LCD32FlushCanvas(lcd32);
    DelayMs(500);

    // --- Test 6: LCD32DrawPolygon (Outline) ---
    SysLog("[TaskScreen] Testing: LCD32DrawPolygon");
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    {
        size_t num_points = (esp_random() % 5) + 3; // 3 to 7 points
        LCDPoint_t points[8];
        for(size_t i = 0; i < num_points; i++) {
            points[i].row = RandCoordinate(lcd32->Height, 50);
            points[i].col = RandCoordinate(lcd32->Width, 50);
        }
        LCD32DrawPolygon(lcd32, points, num_points, (Color_t)esp_random());
    }
    LCD32FlushCanvas(lcd32);
    DelayMs(500);

    // --- Test 7: LCD32DrawFilledPolygon ---
    SysLog("[TaskScreen] Testing: LCD32DrawFilledPolygon");
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    {
        size_t num_points = (esp_random() % 5) + 3; // 3 to 7 points
        LCDPoint_t points[8];
        for(size_t i = 0; i < num_points; i++) {
            points[i].row = RandCoordinate(lcd32->Height, 50);
            points[i].col = RandCoordinate(lcd32->Width, 50);
        }
        LCD32DrawFilledPolygon(lcd32, points, num_points, (Color_t)esp_random());
    }
    LCD32FlushCanvas(lcd32);
    DelayMs(500);

    // --- Font Tests (Iterate via SystemFont Union Array) ---
    // Order in union/struct: Title, Body, Heading01, Heading02, Heading03, Note
    const char* font_names[] = {
        "Title (24pt)", 
        "Body (9pt)", 
        "Heading01 (18pt)", 
        "Heading02 (12pt)", 
        "Heading03 (9pt)", 
        "Note (Picopixel)"
    };
    
    // Calculate number of fonts based on the union array size
    size_t num_fonts = sizeof(SystemFont.arr) / sizeof(SystemFont.arr[0]);

    for (size_t f = 0; f < num_fonts; f++) {
        // Access font via the union array (already pointers, so no & needed)
        const GFXfont* current_font = SystemFont.arr[f];
        const char* current_font_name = font_names[f];

        if (current_font == NULL) continue; // Skip if null

        // --- Test 8.1: LCD32DrawChar with current font ---
        SysLog("[TaskScreen] Testing: LCD32DrawChar with %s", current_font_name);
        
        Color_t bg_color = (Color_t)esp_random();
        Color_t fg_color = (Color_t)esp_random();
        
        LCD32FillCanvas(lcd32, bg_color);
        
        // Use fewer characters for larger fonts to avoid clutter
        int char_count = (current_font->yAdvance > 20) ? 20 : 50;
        
        for (int i = 0; i < char_count; i++) {
            Dim_t r = RandCoordinate(lcd32->Height, 20);
            Dim_t c = RandCoordinate(lcd32->Width, 20);
            char ch = (esp_random() % (126 - 32)) + 32; // Printable ASCII
            LCD32DrawChar(lcd32, r, c, ch, current_font, fg_color);
        }
        LCD32FlushCanvas(lcd32);
        DelayMs(500);

        // --- Test 8.2: LCD32DrawText Normal ---
        SysLog("[TaskScreen] Testing: LCD32DrawText with %s", current_font_name);
        LCD32FillCanvas(lcd32, (Color_t)esp_random());
        
        const char* test_strings[] = { 
            "Hello World!", "ESP32 Test", "LCD32 Driver", 
            "Random Text\nNew Line", "Boundary Check" 
        };
        const char* str_to_draw = test_strings[esp_random() % 5];
        
        Dim_t r = RandCoordinate(lcd32->Height, 20);
        Dim_t c = RandCoordinate(lcd32->Width, 20);
        
        LCD32DrawText(lcd32, r, c, str_to_draw, current_font, (Color_t)esp_random());
        LCD32FlushCanvas(lcd32);
        DelayMs(500);

        // --- Test 8.3: STRESS TEST - Long Text (>300 chars) ---
        SysLog("[TaskScreen] Testing: Pangram Stress Test (>300 chars) with %s", current_font_name);
        LCD32FillCanvas(lcd32, (Color_t)esp_random());
        
        // "The quick brown fox jumps over the lazy dog." is 44 chars.
        // Repeating it 7 times = 308 chars.
        // This tests buffer handling and rendering stability.
        const char * long_pangram = 
            "The quick brown fox jumps over the lazy dog. "
            "The quick brown fox jumps over the lazy dog. "
            "The quick brown fox jumps over the lazy dog. "
            "The quick brown fox jumps over the lazy dog. "
            "The quick brown fox jumps over the lazy dog. "
            "The quick brown fox jumps over the lazy dog. "
            "The quick brown fox jumps over the lazy dog.";

        // Draw near top-left to maximize visible area
        LCD32DrawText(lcd32, 10, 5, long_pangram, current_font, (Color_t)esp_random());
        
        LCD32FlushCanvas(lcd32);
        DelayMs(1500); // Give extra time to inspect
    }
}

static DefaultRet_t WaitForAnalyzerReaderReadyPinHigh(int TimeoutMS){
    if(!IsValidPin(ANALYZER_READER_PIN_READY)) return STAT_ERR_IO;
    int64_t TimeoutUS = 1000L * TimeoutMS;
    int64_t EntryTime = esp_timer_get_time();
    while(esp_timer_get_time() - EntryTime <= TimeoutUS){
        if((IOGet() & Mask64(ANALYZER_READER_PIN_READY)) != 0) return STAT_OKE;
        DelayMs(5);
    }
    return STAT_ERR_TIMEOUT;
}

static DefaultRet_t WaitForAnalyzerReaderReadyPinLow(int TimeoutMS){
    if(!IsValidPin(ANALYZER_READER_PIN_READY)) return STAT_ERR_IO;
    int64_t TimeoutUS = 1000L * TimeoutMS;
    int64_t EntryTime = esp_timer_get_time();
    while(esp_timer_get_time() - EntryTime <= TimeoutUS){
        DelayMs(5);
        if((IOGet() & Mask64(ANALYZER_READER_PIN_READY)) == 0) return STAT_OKE;
    }
    return STAT_ERR_TIMEOUT;
}

/// @brief Init SPI with Manual CS configuration.
static DefaultRet_t AnalyzerReaderComInit(spi_device_handle_t * AnalyzerMasterSPIHandlePtr){
    AMEntry("AnalyzerReaderComInit(...)");

    DefaultRet_t ReturnValue = 0;
    bool AnalyzerMasterSPIReady = false;

    /// Allocate DMA-capable memory for RX/TX buffers.
    AnalyzerMasterRx = (uint16_t *)heap_caps_malloc(ANALYZER_MASTER_RX_SIZE * sizeof(HalfWord_t), MALLOC_CAP_DMA);
    AnalyzerMasterTx = (uint16_t *)heap_caps_malloc(ANALYZER_MASTER_TX_SIZE * sizeof(HalfWord_t), MALLOC_CAP_DMA);

    if (IsNull(AnalyzerMasterRx) || IsNull(AnalyzerMasterTx)) {
        AMErr("[AnalyzerReaderComInit] Failed to allocate DMA buffers!");
        goto cleanup;
    }
    
    /// Zero-initialize buffers.
    memset(AnalyzerMasterRx, 0, ANALYZER_MASTER_RX_SIZE * sizeof(HalfWord_t));
    memset(AnalyzerMasterTx, 0, ANALYZER_MASTER_TX_SIZE * sizeof(HalfWord_t));

    AMLog("[AnalyzerReaderComInit] Buffers allocated directly for DMA usage.");

    // --- GPIO Configuration (Ready Pin) ---
    #if (ANALYZER_READER_PIN_READY != -1)
        /// Configure the handshake pin as input.
        IOConfigAsInput(1ULL << ANALYZER_READER_PIN_READY, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_ENABLE);
    #endif

    // --- GPIO Configuration (Manual CS) ---
    /// Reset and config CS pin as Output for manual control
    gpio_reset_pin(ANALYZER_MASTER_SPI_CS);
    gpio_set_direction(ANALYZER_MASTER_SPI_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(ANALYZER_MASTER_SPI_CS, 1); /// Default High (Inactive)

    // --- SPI Configuration ---
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_MASTER_SPI_MISO,
        .mosi_io_num = ANALYZER_MASTER_SPI_MOSI,
        .sclk_io_num = ANALYZER_MASTER_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = ANALYZER_MASTER_RX_SIZE * sizeof(HalfWord_t),
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = ANALYZER_READER_SPI_FREQ,
        .mode = 3,               // SPI mode 3 (CPOL=1, CPHA=1)
        .spics_io_num = -1,      /// Disable Hardware CS to control manually
        .queue_size = 1,         /// Queue size > 0 for queue_trans
        .command_bits = 0,       
        .address_bits = 0,       
        .dummy_bits = 0,
        .cs_ena_pretrans = 0,    /// Not used in manual mode
    };

    /// Initialize the SPI bus with DMA enabled (Auto channel).
    ReturnValue = spi_bus_initialize(ANALYZER_READER_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ReturnValue != ESP_OK) goto cleanup;
    AnalyzerMasterSPIReady = true;

    /// Attach the Reader device to the SPI bus.
    ReturnValue = spi_bus_add_device(ANALYZER_READER_SPI_HOST, &devcfg, &(*AnalyzerMasterSPIHandlePtr));
    if (ReturnValue != ESP_OK) goto cleanup;

    AMLog("[AnalyzerReaderComInit] SPI Initialized in Manual CS Mode.");

    AMExit("AnalyzerReaderComInit() : STAT_OKE");
    return STAT_OKE;

    cleanup:
    /// Release resources before exiting.
    if ((*AnalyzerMasterSPIHandlePtr)) spi_bus_remove_device(*AnalyzerMasterSPIHandlePtr);
    if (AnalyzerMasterSPIReady) spi_bus_free(ANALYZER_READER_SPI_HOST);
    if (AnalyzerMasterTx) heap_caps_free(AnalyzerMasterTx);
    if (AnalyzerMasterRx) heap_caps_free(AnalyzerMasterRx);

    ReturnValue = ESPReturnType2DefaultReturnType(ReturnValue);
    AMExit("AnalyzerReaderComInit() : %s", DefaultReturnType2Str(ReturnValue));
    return ReturnValue;
}

static DefaultRet_t PerformAnalyzerReaderComViaSPI(spi_device_handle_t *SPIHandlePtr, HalfWord_t * SpecificTxData, HalfWord_t TxHalfWordSize){
    AMEntry("PerformAnalyzerReaderComViaSPI(%p, %p, %d)", SPIHandlePtr, SpecificTxData, TxHalfWordSize);
    if(IsNotPos(TxHalfWordSize)) {
        return STAT_ERR_INVALID_ARG;
    }
    
    /// Setup data
    if(IsNotNull(SpecificTxData)){
        REPN(i, TxHalfWordSize) {
            AnalyzerMasterTx[i] = SpecificTxData[i];
        }
    }

    AMLog("[TaskAnalyzerReaderCom] TX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X,...}",
            AnalyzerMasterTx[0], AnalyzerMasterTx[1], AnalyzerMasterTx[2], AnalyzerMasterTx[3],
            AnalyzerMasterTx[4], AnalyzerMasterTx[5], AnalyzerMasterTx[6], AnalyzerMasterRx[7]);

    /// Create transaction
    spi_transaction_t trans;
    spi_transaction_t *r_trans;
    memset(&trans, 0, sizeof(trans));
    trans.length = TxHalfWordSize * 16;
    trans.tx_buffer = AnalyzerMasterTx;
    trans.rx_buffer = AnalyzerMasterRx;

    /// Wait for Slave Ready (Physical Pin High)
    WaitForAnalyzerReaderReadyPinHigh(200);

    // --- MANUAL CS CONTROL START ---
    
    /// 1. Pull CS Low (Start Frame)
    gpio_set_level(ANALYZER_MASTER_SPI_CS, 0);

    /// 2. Wait 1ms (Hard delay for Slave wakeup/setup)
    ets_delay_us(1000); 

    /// 3. Queue Transaction (Non-blocking call)
    DefaultRet_t ReturnValue = spi_device_queue_trans((*SPIHandlePtr), &trans, portMAX_DELAY);
    ReturnValue = ESPReturnType2DefaultReturnType(ReturnValue);

    if(ReturnValue == STAT_OKE){
        /// 4. Wait for result (Blocking until done)
        ReturnValue = spi_device_get_trans_result((*SPIHandlePtr), &r_trans, portMAX_DELAY);
        ReturnValue = ESPReturnType2DefaultReturnType(ReturnValue);
    }
    
    /// 5. Pull CS High (End Frame)
    gpio_set_level(ANALYZER_MASTER_SPI_CS, 1);

    // --- MANUAL CS CONTROL END ---

    if(ReturnValue != STAT_OKE){
        AMErr("[PerformAnalyzerReaderComViaSPI] ERR: %s", DefaultReturnType2Str(ReturnValue));
        return ReturnValue;
    }

    AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X,...}",
            AnalyzerMasterRx[0], AnalyzerMasterRx[1], AnalyzerMasterRx[2], AnalyzerMasterRx[3],
            AnalyzerMasterRx[4], AnalyzerMasterRx[5], AnalyzerMasterRx[6], AnalyzerMasterRx[7]);

    AMExit("PerformAnalyzerReaderComViaSPI(): %s", DefaultReturnType2Str(ReturnValue));
    return ReturnValue;
}

#endif /// ANALYZER_MASTER_LOCAL_UTILS

void TaskScreen(void * pv){
    /// Waiting for essential init
    while(SYSTEM_STAGE < SYSTEM_INIT_N(0)) vTaskDelay(1);
    /// Start the task
    SysEntry("TaskScreen(%p)", pv);

    // 1. Create a new LCD device instance
    lcd32 = LCD32New();
    if (IsNull(lcd32)) {
        SysErr("[TaskScreen] Failed to create LCD device object.");
        vTaskDelete(NULL); // Abort task if creation fails
        return;
    }

    // 2. Define pin configuration arrays
    const Pin_t ctl_pins[6] = {
        LCD32_RD, LCD32_WR, LCD32_CS, LCD32_RS, LCD32_RST, LCD32_BL
    };

    const Pin_t dat_pins[16] = {
        LCD32_DB0,  LCD32_DB1,  LCD32_DB2,  LCD32_DB3,
        LCD32_DB4,  LCD32_DB5,  LCD32_DB6,  LCD32_DB7,
        LCD32_DB8,  LCD32_DB9,  LCD32_DB10, LCD32_DB11,
        LCD32_DB12, LCD32_DB13, LCD32_DB14, LCD32_DB15
    };

    // 3. Configure the LCD driver with the specified pins
    if (LCD32Config(lcd32, ctl_pins, dat_pins, &lcd_lut) != STAT_OKE) {
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
    
    LCD32FillCanvas(lcd32, COLOR_WHITE);
    LCD32DrawText(lcd32, 20, 0, "Hello from ngxx.fus!", SystemFont.Heading02, COLOR_CYAN);
    LCD32FlushCanvas(lcd32);
    DelayMs(500);

    /// Set next state
    SET_SYSTEM_INIT_N(2);

    // 5. Main loop: Test all drawing functions cyclically
    while (!IS_SYSTEM_STOPPED()){
        PerformScreenTest(lcd32);
    }
}

/// @brief Task to handle SPI Master communication with the Analyzer Reader.
///        Implements a manual command-response protocol over Full-Duplex SPI.
/// @param pv Parameters passed to the task (unused).
void TaskAnalyzerReaderCom(void * pv) {
    /// Wait for the system to reach initialization stage 2.
    while(SYSTEM_STAGE < SYSTEM_INIT_N(2)) { vTaskDelay(1); }

    AMEntry("TaskAnalyzerReaderCom(%p)", pv);

    spi_device_handle_t AnalyzerMasterSPIHandle;
    DefaultRet_t ReturnValue = AnalyzerReaderComInit(&AnalyzerMasterSPIHandle);
    bool AnalyzerMasterSPIReady = (ReturnValue == STAT_OKE) ? true : false;
    bool AnalyzerReaderIDVerified = false;

    static uint64_t __ACKs = 0, __NACKs = 0, __Total = 0;

    // Helper macro to log communication statistics in a readable format.
    #define LOG_COM_STATS() \
        do { \
            if (__Total > 0) { \
                uint32_t ack_p = (uint32_t)((100LLU * __ACKs) / __Total); \
                uint32_t nack_p = (uint32_t)((100LLU * __NACKs) / __Total); \
                uint32_t other_p = 100 - ack_p - nack_p; \
                AMLog("[TaskAnalyzerReaderCom] Stats: Total=%llu | ACK: %u%%, NACK: %u%%, Other: %u%%", __Total, ack_p, nack_p, other_p); \
            } \
        } while(0)

    SYSTEM_STAGE = SYSTEM_RUNNING;

    /// Verify Analyzer-Reader-ID loop
    while((!IS_SYSTEM_STOPPED()) && (!AnalyzerReaderIDVerified)){
        AMLog("\n");
        AMLog("[TaskAnalyzerReaderCom] Try to verify Analyzer-Reader... ");
        
        AnalyzerMasterTx[0] = (AM_CMD_REQ_ID);      // HalfWord 0: CMD
        AnalyzerMasterTx[1] = (ANALYZER_MASTER_ID); // HalfWord 1: ARG/ADDR

        PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 2);
        WaitForAnalyzerReaderReadyPinLow(1000);

        AnalyzerMasterTx[0] = AM_CMD_NOP;   // HalfWord 0: NOP
        AnalyzerMasterTx[1] = AM_CMD_NOP;   // HalfWord 1: NOP

        PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 2);

        AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X,...}", AnalyzerMasterRx[0], AnalyzerMasterRx[1]);
        
        __Total++;
        if(AnalyzerMasterRx[0] == ANALYZER_READER_ACK){
            __ACKs++;
            if (AnalyzerMasterRx[1] == ANALYZER_READER_ID) {
                AnalyzerReaderIDVerified = true;
                AMLog("[TaskAnalyzerReaderCom] Reader ID Verified!");
            } else {
                AMErr("[TaskAnalyzerReaderCom] Received ACK, but wrong expected ID (0x%04X)! Check the the Analyzer-Reader!", ANALYZER_READER_ID);
                DelayMs(1000); // Retry after delay on failure
                continue;
            }
        }else if(AnalyzerMasterRx[0] == ANALYZER_READER_NACK){
            __NACKs++;
            AMErr("[TaskAnalyzerReaderCom] Received NACK! Check the command!");
            DelayMs(1000); // Retry after delay on failure
            continue;
        }else {
            AMErr("[TaskAnalyzerReaderCom] Not received ACK nor NACK (0x%04X)! Check the the Analyzer-Reader!", AnalyzerMasterRx[0]);
            DelayMs(1000); // Retry after delay on failure
            continue;
        }
        LOG_COM_STATS();
    }

    /// Crawl data from Analyzer-Reader loop
    while (!IS_SYSTEM_STOPPED()) {
        
        /// Populate TX buffer manually.
        AnalyzerMasterTx[0] = (AM_CMD_REQ_TEST);
        AnalyzerMasterTx[1] = AnalyzerMasterRxSize;

        PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 2);
        WaitForAnalyzerReaderReadyPinLow(1000);
        ReturnValue = PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, AnalyzerMasterRxSize);

        __Total++;
        if (ReturnValue == ESP_OK) {
            AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X,...}", AnalyzerMasterRx[0], AnalyzerMasterRx[1]);

            if(AnalyzerMasterRx[0] == ANALYZER_READER_ACK){
                __ACKs++;
                AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X,...}",
                      AnalyzerMasterRx[0], AnalyzerMasterRx[1], AnalyzerMasterRx[2], AnalyzerMasterRx[3],
                      AnalyzerMasterRx[4], AnalyzerMasterRx[5], AnalyzerMasterRx[6], AnalyzerMasterRx[7]);
                /// Do with the correct data!
            }else if (AnalyzerMasterRx[0] == ANALYZER_READER_NACK){
                __NACKs++;
                AMErr("[TaskAnalyzerReaderCom] Received NACK! Check the command!");
            }else {
                AMErr("[TaskAnalyzerReaderCom] Not received ACK nor NACK (0x%04X)! Check the the Analyzer-Reader!", AnalyzerMasterRx[0]);
            }
        }
        LOG_COM_STATS();
        AMLog("\n");

        DelayMs(1000);
    }

    /// Release resources before exiting.
    if (AnalyzerMasterSPIHandle)    spi_bus_remove_device(AnalyzerMasterSPIHandle);
    if (AnalyzerMasterSPIReady)     spi_bus_free(ANALYZER_READER_SPI_HOST);
    if (AnalyzerMasterTx)           heap_caps_free(AnalyzerMasterTx);
    if (AnalyzerMasterRx)           heap_caps_free(AnalyzerMasterRx);
    
    // Undefine the local macro to avoid polluting other scopes
    #undef LOG_COM_STATS

    AMExit("Exiting.");
    vTaskDelete(NULL);
}

#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)
