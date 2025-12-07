#include "All.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#define ANALYZER_MASTER_LOCAL_UTILS /// Do not deleted!

#include "driver/spi_master.h"

/// @brief Current configured size of the RX buffer.
const uint32_t  AnalyzerMasterRxSize = ANALYZER_MASTER_RX_SIZE;
/// @brief Current configured size of the TX buffer.
const uint32_t  AnalyzerMasterTxSize = ANALYZER_MASTER_TX_SIZE;
/// @brief Pointer to the RX data buffer.
uint16_t *      AnalyzerMasterRx     = NULL ;
/// @brief Pointer to the TX data buffer.
uint16_t *      AnalyzerMasterTx     = NULL ;
/// @brief CBuff for RX
CBuff_t *       AnalyzerMasterCBuff  = NULL;
/// @brief LCD32
LCD32Dev_t *    lcd32 = NULL; 

#ifdef ANALYZER_MASTER_LOCAL_UTILS

/// @brief Statically allocated Look-Up Table for the main LCD to save HEAP memory.
static P16Lut_t lcd_lut;

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

void TaskAnalyzerReaderCom(void * pv) {
    /// Wait for essential init
    while(SYSTEM_STAGE < SYSTEM_INIT_N(2)) { vTaskDelay(1); }

    AMEntry("TaskAnalyzerReaderCom(%p)", pv);

    esp_err_t ret;
    spi_device_handle_t spi_handle = NULL;
    bool spi_bus_initialized = false;

    /// Allocate SPI RX buffer
    AnalyzerMasterRx = (uint16_t *)heap_caps_malloc(ANALYZER_MASTER_RX_SIZE * sizeof(HalfWord_t), MALLOC_CAP_SPIRAM);
    if(IsNull(AnalyzerMasterRx)){
        AMErr("[TaskAnalyzerReaderCom] Cannot allocate memory for SPI RX buffer!");
        goto cleanup;
    }
    AMLog("[TaskAnalyzerReaderCom] Allocated %d bytes for RX buffer in PSRAM.", ANALYZER_MASTER_RX_SIZE * sizeof(HalfWord_t));

    /// Allocate SPI TX buffer
    AnalyzerMasterTx = (uint16_t *)heap_caps_malloc(ANALYZER_MASTER_TX_SIZE * sizeof(HalfWord_t), MALLOC_CAP_SPIRAM);
    if(IsNull(AnalyzerMasterTx)){
        AMErr("[TaskAnalyzerReaderCom] Cannot allocate memory for SPI TX buffer!");
        goto cleanup;
    }
    AMLog("[TaskAnalyzerReaderCom] Allocated %d bytes for TX buffer in PSRAM.", ANALYZER_MASTER_TX_SIZE * sizeof(HalfWord_t));

    // --- GPIO Configuration ---
    #if (ANALYZER_READER_PIN_READY != -1)
        IOConfigAsInput(1ULL << ANALYZER_READER_PIN_READY, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_ENABLE);
        AMLog("[TaskAnalyzerReaderCom] Configured READY pin (%d) as input.", ANALYZER_READER_PIN_READY);
    #else
        AMLog("[TaskAnalyzerReaderCom] READY pin is disabled, will not wait for signal.");
    #endif

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
        .mode = 0, // SPI mode 0
        .spics_io_num = ANALYZER_MASTER_SPI_CS,
        .queue_size = 7,
        .command_bits = 16, // Protocol: 16-bit command
        .address_bits = 16, // Protocol: 16-bit argument
        .flags = SPI_DEVICE_HALFDUPLEX, // Explicitly set half-duplex mode
    };

    // Initialize the SPI bus
    ret = spi_bus_initialize(ANALYZER_READER_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        AMErr("[TaskAnalyzerReaderCom] spi_bus_initialize failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }
    spi_bus_initialized = true;

    // Attach the slave device to the SPI bus
    ret = spi_bus_add_device(ANALYZER_READER_SPI_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        AMErr("[TaskAnalyzerReaderCom] spi_bus_add_device failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    AMLog("[TaskAnalyzerReaderCom] SPI Master initialized successfully.");

    bool reader_verified = false;

    while(!IS_SYSTEM_STOPPED()){
        if (!reader_verified) {
            AMLog("[TaskAnalyzerReaderCom] Sending ID request to reader...");

            spi_transaction_t id_trans;
            memset(&id_trans, 0, sizeof(id_trans));
            id_trans.flags       = SPI_TRANS_USE_RXDATA; // Use internal buffer for small reception
            id_trans.cmd         = AM_CMD_REQ_ID;
            id_trans.addr        = ANALYZER_MASTER_ID;
            id_trans.rxlength    = sizeof(HalfWord_t) * 8; // Expect 1 half-word (16 bits) back

            ret = spi_device_polling_transmit(spi_handle, &id_trans);

            if (ret != ESP_OK) {
                AMErr("[TaskAnalyzerReaderCom] ID request transaction failed: %s", esp_err_to_name(ret));
            } else {
                // SPI data is received MSB first. On a little-endian CPU, we need to swap bytes.
                uint16_t received_id = SPI_SWAP_DATA_RX(*(uint16_t*)id_trans.rx_data, 16);
                AMLog("[TaskAnalyzerReaderCom] Received ID: 0x%04X", received_id);

                if (received_id == ANALYZER_READER_ID) {
                    AMLog("[TaskAnalyzerReaderCom] Reader ID verified successfully!");
                    reader_verified = true;
                } else {
                    AMErr("[TaskAnalyzerReaderCom] Reader ID mismatch! Expected 0x%04X, got 0x%04X.", (uint16_t)ANALYZER_READER_ID, received_id);
                }
            }

            // If verification fails, wait and retry in the next loop iteration
            if (!reader_verified) {
                DelayMs(2000);
                continue; // Skip to next loop iteration
            }
        }

        // --- If verified, proceed with other commands ---
        {
            bool proceed_with_transaction = false;
            #if (ANALYZER_READER_PIN_READY != -1)
                AMLog("[TaskAnalyzerReaderCom] Waiting for READY signal...");
                int64_t start_time = esp_timer_get_time();
                while(gpio_get_level(ANALYZER_READER_PIN_READY) == 0) {
                    if ((esp_timer_get_time() - start_time) / 1000 > 2000) { // 2s timeout
                        AMLog("[TaskAnalyzerReaderCom] Timeout waiting for READY signal. Retrying...");
                        break;
                    }
                    vTaskDelay(pdMS_TO_TICKS(1)); // Yield
                }

                if (gpio_get_level(ANALYZER_READER_PIN_READY) == 1) {
                    proceed_with_transaction = true;
                }
            #else
                proceed_with_transaction = true;
            #endif

            if (proceed_with_transaction) {
                AMLog("[TaskAnalyzerReaderCom] READY signal detected/bypassed. Requesting test data.");

                const uint16_t data_req_size_hw = 5; // Request 5 half-words

                spi_transaction_t t;
                memset(&t, 0, sizeof(t));
                t.cmd        = AM_CMD_REQ_TEST;
                t.addr       = data_req_size_hw;
                t.rxlength   = data_req_size_hw * sizeof(HalfWord_t) * 8;
                t.rx_buffer  = AnalyzerMasterRx;

                ret = spi_device_polling_transmit(spi_handle, &t);
                if (ret == ESP_OK) {
                    AMLog("[TaskAnalyzerReaderCom] SPI transaction successful. Read %d bytes.", t.rxlength / 8);

                    // The SPI peripheral transfers data MSB-first. On a little-endian CPU like ESP32,
                    // we need to swap the bytes of each half-word to get the correct value,
                    // ensuring consistency with the ID check.
                    for (int i = 0; i < data_req_size_hw; i++) {
                        AnalyzerMasterRx[i] = __builtin_bswap16(AnalyzerMasterRx[i]);
                    }

                    AMLog("[TaskAnalyzerReaderCom] Data sample: 0x%04X 0x%04X 0x%04X 0x%04X 0x%04X",
                          AnalyzerMasterRx[0], AnalyzerMasterRx[1], AnalyzerMasterRx[2], AnalyzerMasterRx[3], AnalyzerMasterRx[4]);
                } else {
                    AMErr("[TaskAnalyzerReaderCom] SPI transaction failed: %s", esp_err_to_name(ret));
                }
            }
        }
        DelayMs(5000); // Wait 5s before next cycle
    }

cleanup:
    if (spi_handle) {
        spi_bus_remove_device(spi_handle);
    }
    if (spi_bus_initialized) {
        spi_bus_free(ANALYZER_READER_SPI_HOST);
    }
    if (AnalyzerMasterTx) {
        heap_caps_free(AnalyzerMasterTx);
        AnalyzerMasterTx = NULL;
    }
    if (AnalyzerMasterRx) {
        heap_caps_free(AnalyzerMasterRx);
        AnalyzerMasterRx = NULL;
    }
    AMExit("TaskAnalyzerReaderCom() exiting.");
    vTaskDelete(NULL);
}

void Deprecated_TaskAnalyzerReaderCom(void * pv) {
    AMEntry("TaskAnalyzerReaderCom(%p)", pv);

    // --- Constants ---
    #define ANALYZER_CMD_READ_DATA      0x0A
    #define READY_WAIT_TIMEOUT_MS       20000
    #define MAIN_LOOP_DELAY_MS          10000

    // --- Buffer Configuration ---
    const size_t buffer_size_words = 4096;
    const size_t buffer_size_bytes = buffer_size_words * sizeof(uint16_t);
    
    // This is a temporary linear buffer to receive data from a single SPI transaction.
    // The data is then copied to the main circular buffer.
    uint16_t *spi_rx_buffer = NULL;
    // This is the main circular buffer to store incoming data stream.
    CBuff_t *data_cbuff = NULL;

    // --- Buffer Allocation ---
    #if (LCD32_CANVAS_IN_PSRAM_EN == 1)
        spi_rx_buffer = (uint16_t *)heap_caps_malloc(buffer_size_bytes, MALLOC_CAP_SPIRAM);
        AMLog("[TaskAnalyzerReaderCom] Allocating %d-byte temporary SPI RX buffer in PSRAM.", buffer_size_bytes);
    #else
        spi_rx_buffer = (uint16_t *)malloc(buffer_size_bytes);
        AMLog("[TaskAnalyzerReaderCom] Allocating %d-byte temporary SPI RX buffer in Internal RAM.", buffer_size_bytes);
    #endif

    if (IsNull(spi_rx_buffer)) {
        AMErr("[TaskAnalyzerReaderCom] Failed to allocate SPI buffer.");
        vTaskDelete(NULL);
        return;
    }

    // Create the main circular buffer, preferably in PSRAM.
    data_cbuff = CBuffCreate(buffer_size_bytes);
    if (IsNull(data_cbuff)) {
        AMErr("[TaskAnalyzerReaderCom] Failed to create circular buffer.");
        heap_caps_free(spi_rx_buffer);
        vTaskDelete(NULL);
        return;
    }
    AMLog("[TaskAnalyzerReaderCom] Circular buffer created successfully (size: %d bytes).", buffer_size_bytes);

    // --- GPIO Configuration ---
    // Configure READY pin as input with pull-down, if it's enabled.
    #if (ANALYZER_READER_PIN_READY != -1)
        IOConfigAsInput(1ULL << ANALYZER_READER_PIN_READY, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_ENABLE);
        AMLog("[TaskAnalyzerReaderCom] Configured READY pin (%d) as input.", ANALYZER_READER_PIN_READY);
    #else
        AMLog("[TaskAnalyzerReaderCom] READY pin is disabled, will not wait for signal.");
    #endif

    // --- SPI Configuration ---
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_MASTER_SPI_MISO,
        .mosi_io_num = ANALYZER_MASTER_SPI_MOSI,
        .sclk_io_num = ANALYZER_MASTER_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = buffer_size_bytes,
    };

    // Note: SPI clock speed should be chosen based on the slave device's capability.
    // Starting with a safe 10 MHz.
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0, // SPI mode 0
        .spics_io_num = ANALYZER_MASTER_SPI_CS,
        .queue_size = 7, // We want to be able to queue 7 transactions at a time
        .command_bits = 8, // Use 8-bit command phase
    };

    spi_device_handle_t spi_handle;

    // Initialize the SPI bus
    esp_err_t ret = spi_bus_initialize(ANALYZER_READER_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        AMErr("[TaskAnalyzerReaderCom] spi_bus_initialize failed: %s", esp_err_to_name(ret));
        heap_caps_free(spi_rx_buffer);
        vTaskDelete(NULL);
        return;
    }

    // Attach the slave device to the SPI bus
    ret = spi_bus_add_device(ANALYZER_READER_SPI_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        AMErr("[TaskAnalyzerReaderCom] spi_bus_add_device failed: %s", esp_err_to_name(ret));
        spi_bus_free(ANALYZER_READER_SPI_HOST);
        heap_caps_free(spi_rx_buffer);
        vTaskDelete(NULL);
        return;
    }

    AMLog("[TaskAnalyzerReaderCom] SPI Master initialized successfully.");

    // --- Main Loop ---
    while(1) {
        bool proceed_with_transaction = false;

        #if (ANALYZER_READER_PIN_READY != -1)
            // Wait for the slave to be ready (active high)
            AMLog("[TaskAnalyzerReaderCom] Waiting for READY signal...");
            int64_t start_time = esp_timer_get_time();
            while(gpio_get_level(ANALYZER_READER_PIN_READY) == 0) {
                if ((esp_timer_get_time() - start_time) / 1000 > READY_WAIT_TIMEOUT_MS) {
                    AMLog("[TaskAnalyzerReaderCom] Timeout waiting for READY signal. Retrying...");
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(1)); // Yield to other tasks
            }

            if (gpio_get_level(ANALYZER_READER_PIN_READY) == 1) {
                proceed_with_transaction = true;
            }
        #else
            // If READY pin is disabled, always proceed.
            proceed_with_transaction = true;
        #endif

        if (proceed_with_transaction) {
            AMLog("[TaskAnalyzerReaderCom] READY signal detected or bypassed. Starting transaction.");

            // A single transaction that sends a command and then reads data.
            // This is more efficient and ensures CS stays low during the whole process.
            spi_transaction_t t;
            memset(&t, 0, sizeof(t));
            t.cmd = ANALYZER_CMD_READ_DATA;      // Command to send
            t.length = buffer_size_bytes * 8;    // Total bits to READ
            t.rx_buffer = spi_rx_buffer;         // Buffer to store received data

            ret = spi_device_polling_transmit(spi_handle, &t);
            if (ret == ESP_OK) {
                // On success, t.rxlength is the number of bits read.
                size_t bytes_read = t.rxlength / 8;
                AMLog("[TaskAnalyzerReaderCom] Command 0x%02X sent, read %d bytes from SPI.", (uint8_t)t.cmd, bytes_read);

                    // --- Phase 3: Store data in Circular Buffer ---
                    size_t bytes_written = CBuffWrite(data_cbuff, spi_rx_buffer, bytes_read);
                    if (bytes_written < bytes_read) {
                        AMErr("[TaskAnalyzerReaderCom] Circular buffer is full! Discarded %d bytes.", bytes_read - bytes_written);
                    } else {
                        AMLog("[TaskAnalyzerReaderCom] Wrote %d bytes to circular buffer.", bytes_written);
                    }
                    AMLog("[TaskAnalyzerReaderCom] CBuff status: %u / %u bytes used.", CBuffGetDataCount(data_cbuff), data_cbuff->size);

                    AMLog("[TaskAnalyzerReaderCom] Data sample: 0x%04X 0x%04X 0x%04X 0x%04X ...",
                          spi_rx_buffer[0], spi_rx_buffer[1], spi_rx_buffer[2], spi_rx_buffer[3]);
            } else {
                AMErr("[TaskAnalyzerReaderCom] SPI transaction failed: %s", esp_err_to_name(ret));
            }
        }

        // Wait before next cycle
        vTaskDelay(pdMS_TO_TICKS(MAIN_LOOP_DELAY_MS));
    }
}


#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)