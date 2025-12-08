#include "All.h"
#include "ESPFreeRTOSWrapper.h"
#include "SystemStage.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

#include "driver/spi_slave.h"
#include "driver/spi_master.h"
#include <string.h>
#include <stdint.h>

// Static buffers for SPI transactions
static uint16_t* AnalyzerReaderTx = NULL;
// Static buffers for SPI transactions
static uint16_t* AnalyzerReaderRx = NULL;

volatile uint16_t ScreenTouchX = 0;
volatile uint16_t ScreenTouchY = 0;
static volatile bool g_is_touch_detected = false;
static spi_device_handle_t touch_spi_handle = NULL;

// Response data for AM_CMD_REQ_ID
// const static uint16_t reader_id = ANALYZER_READER_ID;

// Variable to store the command received in the previous transaction
// Initialized to NOP so the first response is just an ACK/Default
static uint16_t next_response_cmd = AM_CMD_NOP;

/// @brief  Transmit cmd and receive 12-bit data from Touch Controller.
/// @param  cmd Command byte (0xD0 for X, 0x90 for Y).
/// @return 12-bit coordinate value or 0 if failed.
static uint16_t ScreenTouchGetValue(uint8_t cmd) {
    uint8_t tx_data[3] = {cmd, 0x00, 0x00};
    uint8_t rx_data[3] = {0};
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.length = 24; // 3 bytes transaction (Command + High Byte + Low Byte)
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;
    
    // Polling because we are already in a task and data is small
    esp_err_t ret = spi_device_polling_transmit(touch_spi_handle, &t);

    if (ret != ESP_OK) return 0;

    // Parse XPT2046 format: result is in bits 14..3 of the received 24 bits
    // But since we send 8 bits cmd first, the useful data comes in byte 1 and 2.
    // ((Byte1 << 8) | Byte2) >> 3
    uint16_t result = ((uint16_t)rx_data[1] << 8) | rx_data[2];
    result >>= 3; // Shift right to get 12-bit value
    
    return result & 0x0FFF; // Mask to 12 bits
}

/// @brief Interrupt handler triggered when PEN pin goes LOW.
/// @note  Do NOT perform SPI transactions here. Just signal the task.
void IRAM_ATTR ScreenTouchSPIHandler(void* arg) {
    // Notify that a touch is detected
    g_is_touch_detected = true;
    // Optional: You could use xTaskNotifyFromISR here to wake up the task immediately
}

/// @brief Task to initialize SPI Master for Touch and poll data periodically.
void TaskScreenTouchSPICom(void * pv) {
    while(SYSTEM_STAGE < SYSTEM_INIT_N(1)) DelayMs(50);
    AREntry("TaskScreenTouchSPICom(%p)", pv);
    esp_err_t ret;

    // 1. GPIO Config for PEN (Interrupt)
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE, // Falling edge (Touch = Low)
        .pin_bit_mask = (1ULL << ANALYZER_READER_LCD_PEN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1, // XPT2046 PEN needs pull-up
        .pull_down_en = 0
    };
    gpio_config(&io_conf);

    // Install ISR Service
    // Note: If 'gpio_install_isr_service' was already called elsewhere, don't call again.
    // Assuming it might not be called yet:
    gpio_install_isr_service(0); 
    gpio_isr_handler_add(ANALYZER_READER_LCD_PEN, ScreenTouchSPIHandler, NULL);
    ARLog("Touch PEN Interrupt configured on Pin %d", ANALYZER_READER_LCD_PEN);

    // 2. SPI Bus Config (Master)
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_READER_LCD_MISO,
        .mosi_io_num = ANALYZER_READER_LCD_MOSI,
        .sclk_io_num = ANALYZER_READER_LCD_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32, // We only send short commands
    };

    // Initialize SPI3 (VSPI)
    ret = spi_bus_initialize(TOUCH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ARErr("Failed to init SPI3 bus for Touch: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
    }

    // 3. Add Device (Touch Controller)
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,   // 1MHz is safe for XPT2046
        .mode = 0,                   // SPI Mode 0 (CPOL=0, CPHA=0)
        .spics_io_num = ANALYZER_READER_LCD_T_CS,
        .queue_size = 1,
        // .pre_cb = NULL,
    };

    ret = spi_bus_add_device(TOUCH_SPI_HOST, &devcfg, &touch_spi_handle);
    if (ret != ESP_OK) {
        ARErr("Failed to add Touch device to SPI3: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
    }
    
    ARLog("SPI Touch Master initialized. Starting loop...");

    while (1) {
        // Run periodically every 300ms as requested
        DelayMs(100);

        // Check pin level directly for robustness (Debounce simple logic)
        // Or check the flag set by ISR
        if (gpio_get_level(ANALYZER_READER_LCD_PEN) == 0 || g_is_touch_detected) {
            
            // Read X (Command 0xD0)
            uint16_t raw_x = ScreenTouchGetValue(0xD0);
            
            // Read Y (Command 0x90)
            uint16_t raw_y = ScreenTouchGetValue(0x90);

            // Simple validation to reject noise
            if (raw_x > 0 && raw_y > 0) {
                ScreenTouchX = raw_x;
                ScreenTouchY = raw_y;
                ARLog("[TOUCH] Detected! X: %d, Y: %d", ScreenTouchX, ScreenTouchY);
            }
            
            // Reset interrupt flag if we used it logic
            g_is_touch_detected = false;
        } else {
            // ARLog("[TOUCH] Idle - No touch detected.");
        }
    }
}

/**
 * @brief Main task to handle SPI slave communication with the Analyzer Master.
 * @details This implementation uses the split transaction mechanism (Queue -> Get Result)
 * to ensure data is loaded into DMA before the Master starts clocking.
 */
void TaskAnalyzerMasterCom(void * pv) {
    while(SYSTEM_STAGE < SYSTEM_INIT_N(4)) DelayMs(50);
    AREntry("TaskAnalyzerMasterCom(%p)", pv);
    esp_err_t ret;

    /// Transaction structure
    spi_slave_transaction_t trans;
    /// Pointer to the result transaction (required by spi_slave_get_trans_result)
    spi_slave_transaction_t *ret_trans;

    // --- Buffer Allocation ---
    /// Allocate DMA-capable memory for SPI buffers.
    AnalyzerReaderTx = (uint16_t *)heap_caps_malloc(ANALYZER_READER_TX_SIZE * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (IsNull(AnalyzerReaderTx)) {
        ARErr("Failed to allocate SPI TX buffer.");
        goto cleanup;
    }
    ARLog("Allocated %d bytes for TX buffer.", ANALYZER_READER_TX_SIZE * sizeof(uint16_t));

    AnalyzerReaderRx = (uint16_t *)heap_caps_calloc(ANALYZER_READER_RX_SIZE, sizeof(uint16_t), MALLOC_CAP_DMA);
    if (IsNull(AnalyzerReaderRx)) {
        ARErr("Failed to allocate SPI RX buffer.");
        goto cleanup;
    }
    ARLog("Allocated %d bytes for RX buffer.", ANALYZER_READER_RX_SIZE * sizeof(uint16_t));

    // --- GPIO Configuration ---
    /// Configure READY pin as output.
    #if (ANALYZER_READER_PIN_READY != -1)
        IOConfigAsOutput(1ULL << ANALYZER_READER_PIN_READY, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE);
        gpio_set_level(ANALYZER_READER_PIN_READY, 0); // Initially not ready
        ARLog("Configured READY pin (%d) as output.", ANALYZER_READER_PIN_READY);
    #else
        ARLog("READY pin is disabled.");
    #endif

    // --- SPI Slave Configuration ---
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_READER_SPI_MISO,
        .mosi_io_num = ANALYZER_READER_SPI_MOSI,
        .sclk_io_num = ANALYZER_READER_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // Ensure max_transfer_sz covers the full buffer size in bytes
        .max_transfer_sz = ANALYZER_READER_RX_SIZE * sizeof(uint16_t),
    };

    // Configuration for the SPI slave device
    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = ANALYZER_READER_SPI_CS,
        .mode = 3, // SPI mode 3 as per reference code
        .queue_size = 1, // We queue one transaction at a time
        .flags = 0,
    };

    // Initialize SPI slave interface
    ret = spi_slave_initialize(ANALYZER_READER_SPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ARErr("spi_slave_initialize failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ARLog("SPI Slave initialized and configured. Waiting for master.");
    
    // Default response for the very first loop
    next_response_cmd = AM_CMD_NOP;

    // --- Main Loop ---
    while(1) {
        // --- 1. Prepare Data for Transmission ---
        /// Based on the command received in the PREVIOUS transaction (next_response_cmd),
        /// prepare the TX buffer for THIS transaction.
        
        // Clear buffer first to avoid stale data (optional but safer)
        memset(AnalyzerReaderTx, 0, ANALYZER_READER_TX_SIZE * sizeof(uint16_t));

        switch (next_response_cmd) {
            case AM_CMD_NOP:
                AnalyzerReaderTx[0] = ANALYZER_READER_ACK;
                ARLog("Master requested data (NOP CMD).");
                break;
            case AM_CMD_REQ_ID:
                AnalyzerReaderTx[0] = ANALYZER_READER_ACK;
                AnalyzerReaderTx[1] = ANALYZER_READER_ID;
                ARLog("Master requested ID. Preparing response 0x%04X.", ANALYZER_READER_ID);
                break;
            case AM_CMD_REQ_TOUCH:
                AnalyzerReaderTx[0] = ANALYZER_READER_ACK;
                AnalyzerReaderTx[1] = ScreenTouchX;
                AnalyzerReaderTx[2] = ScreenTouchY;
                AnalyzerReaderTx[3] = AnalyzerReaderRx[1];
                ARLog("Master requested TouchData, Preparing response <0x%04X, 0x%04X.>", ScreenTouchX, ScreenTouchY);
                break;            
            case AM_CMD_REQ_TEST:
                ARLog("Master requested test data. Preparing random data.");
                AnalyzerReaderTx[0] = ANALYZER_READER_ACK;
                // Fill remaining with random for test
                for(int i = 1; i < ANALYZER_READER_TX_SIZE; i++) {
                     AnalyzerReaderTx[i] = (uint16_t)rand();
                }
                break;
            default:
                // Unknown command handling
                AnalyzerReaderTx[0] = ANALYZER_READER_NACK; 
                break;
        }

        // --- 2. Setup Transaction ---
        memset(&trans, 0, sizeof(trans));
        trans.length = ANALYZER_READER_RX_SIZE * sizeof(uint16_t) * 8; // Bit length
        trans.tx_buffer = AnalyzerReaderTx;
        trans.rx_buffer = AnalyzerReaderRx;

        // --- 3. Queue Transaction (Load DMA) ---
        /// This call places the transaction in the driver queue. 
        /// Since we use portMAX_DELAY, it waits if queue is full (queue_size=1).
        ret = spi_slave_queue_trans(ANALYZER_READER_SPI_HOST, &trans, portMAX_DELAY);
        if (ret != ESP_OK) {
            ARErr("spi_slave_queue_trans failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // Signal to Master: We are READY (DMA is loaded)
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 1);
        #endif

        // --- 4. Wait for Completion ---
        /// Block here until the Master asserts CS and clocks out the data.
        ret = spi_slave_get_trans_result(ANALYZER_READER_SPI_HOST, &ret_trans, portMAX_DELAY);

        // Transaction done (or failed), lower READY signal immediately
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 0);
        #endif

        if (ret == ESP_OK) {
            // --- 5. Process Received Data ---
            /// Log data exactly as requested
            ARLog("[TaskAnalyzerMasterCom] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
                    AnalyzerReaderRx[0], AnalyzerReaderRx[1], AnalyzerReaderRx[2], AnalyzerReaderRx[3]);
            ARLog("[TaskAnalyzerMasterCom] TX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
                    AnalyzerReaderTx[0], AnalyzerReaderTx[1], AnalyzerReaderTx[2], AnalyzerReaderTx[3]);
            
            uint16_t received_cmd = AnalyzerReaderRx[0];
            uint16_t received_arg = AnalyzerReaderRx[1];

            ARLog("Transferred %d bytes. RX CMD: 0x%04X, ARG: 0x%04X", (trans.trans_len / 8), received_cmd, received_arg);
            
            // Store the command to determine what to send in the NEXT loop
            next_response_cmd = received_cmd;
            
            ARLog("\n");
        } else {
            ARErr("spi_slave_get_trans_result failed: %s", esp_err_to_name(ret));
        }
    }

cleanup:
    ARLog("Cleaning up TaskAnalyzerMasterCom...");
    if (AnalyzerReaderTx) {
        heap_caps_free(AnalyzerReaderTx);
        AnalyzerReaderTx = NULL;
    }
    if (AnalyzerReaderRx) {
        heap_caps_free(AnalyzerReaderRx);
        AnalyzerReaderRx = NULL;
    }
    spi_slave_free(ANALYZER_READER_SPI_HOST);

    ARExit("TaskAnalyzerMasterCom exiting.");
    vTaskDelete(NULL);
}

#endif // (FIRMWARE_TYPE == TYPE_ANALYZER_READER)
