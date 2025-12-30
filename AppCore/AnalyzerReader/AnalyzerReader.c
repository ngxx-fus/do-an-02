#include "All.h"
#include "ESPFreeRTOSWrapper.h"
#include "SystemStage.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

#include "driver/spi_slave.h"
#include "driver/spi_master.h"
#include <string.h>

// --- Data Buffers (Using the shared Union) ---
// AnalyzerDE_t aligns exactly with ANALYZER_PKT_SIZE_BYTE
static AnalyzerDE_t* PktTx = NULL; 
static AnalyzerDE_t* PktRx = NULL;

volatile Word_t ScreenTouchX = 0;
volatile Word_t ScreenTouchY = 0;
static volatile bool g_is_touch_detected = false;
static spi_device_handle_t touch_spi_handle = NULL;

// Variable to store the command received in the previous transaction
// Initialized to NOP so the first response is just an ACK/Default
static Word_t next_response_cmd = AM_CMD_NOP;

/// @brief  Transmit cmd and receive 12-bit data from Touch Controller.
static uint16_t ScreenTouchGetValue(uint8_t cmd) {
    uint8_t tx_data[3] = {cmd, 0x00, 0x00};
    uint8_t rx_data[3] = {0};
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.length = 24; // 3 bytes
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;
    
    esp_err_t ret = spi_device_polling_transmit(touch_spi_handle, &t);
    if (ret != ESP_OK) return 0;

    // Parse XPT2046 format
    uint16_t result = ((uint16_t)rx_data[1] << 8) | rx_data[2];
    result >>= 3; // Shift right to get 12-bit value
    
    return result & 0x0FFF;
}

/// @brief Interrupt handler triggered when PEN pin goes LOW.
void IRAM_ATTR ScreenTouchSPIHandler(void* arg) {
    g_is_touch_detected = true;
}

/// @brief Task to initialize SPI Master for Touch and poll data periodically.
void TaskScreenTouchSPICom(void * pv) {
    while(SYSTEM_STAGE < SYSTEM_INIT_N(1)) DelayMs(50);
    AREntry("TaskScreenTouchSPICom(%p)", pv);
    esp_err_t ret;

    // 1. GPIO Config for PEN (Interrupt)
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE, 
        .pin_bit_mask = (1ULL << ANALYZER_READER_LCD_PEN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1, 
        .pull_down_en = 0
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0); 
    gpio_isr_handler_add(ANALYZER_READER_LCD_PEN, ScreenTouchSPIHandler, NULL);

    // 2. SPI Bus Config (Master)
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_READER_LCD_MISO,
        .mosi_io_num = ANALYZER_READER_LCD_MOSI,
        .sclk_io_num = ANALYZER_READER_LCD_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    ret = spi_bus_initialize(TOUCH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ARErr("Failed to init SPI3 bus for Touch: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
    }

    // 3. Add Device
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,   // 1MHz
        .mode = 0,
        .spics_io_num = ANALYZER_READER_LCD_T_CS,
        .queue_size = 1,
    };

    ret = spi_bus_add_device(TOUCH_SPI_HOST, &devcfg, &touch_spi_handle);
    if (ret != ESP_OK) {
        ARErr("Failed to add Touch device: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
    }
    
    ARLog("SPI Touch Master initialized.");

    while (1) {
        DelayMs(100);
        if (gpio_get_level(ANALYZER_READER_LCD_PEN) == 0 || g_is_touch_detected) {
            uint16_t raw_x = ScreenTouchGetValue(0xD0);
            uint16_t raw_y = ScreenTouchGetValue(0x90);
            if (raw_x > 0 && raw_y > 0) {
                ScreenTouchX = (Word_t)raw_x;
                ScreenTouchY = (Word_t)raw_y;
                ARLog("[TOUCH] Detected! X: %d, Y: %d", ScreenTouchX, ScreenTouchY);
            }
            g_is_touch_detected = false;
        }
    }
}

/**
 * @brief Main task to handle SPI slave communication with the Analyzer Master.
 */
void TaskAnalyzerMasterCom(void * pv) {
    while(SYSTEM_STAGE < SYSTEM_INIT_N(4)) DelayMs(50);
    AREntry("TaskAnalyzerMasterCom(%p)", pv);
    esp_err_t ret;

    spi_slave_transaction_t trans;
    spi_slave_transaction_t *ret_trans;

    // --- Buffer Allocation (Size = ANALYZER_PKT_SIZE_BYTE) ---
    PktTx = (AnalyzerDE_t *)heap_caps_malloc(ANALYZER_PKT_SIZE_BYTE, MALLOC_CAP_DMA);
    if (IsNull(PktTx)) {
        ARErr("Failed to allocate SPI TX buffer.");
        goto cleanup;
    }

    PktRx = (AnalyzerDE_t *)heap_caps_calloc(1, ANALYZER_PKT_SIZE_BYTE, MALLOC_CAP_DMA);
    if (IsNull(PktRx)) {
        ARErr("Failed to allocate SPI RX buffer.");
        goto cleanup;
    }
    ARLog("Buffers allocated: %d bytes each.", ANALYZER_PKT_SIZE_BYTE);

    // --- GPIO Configuration (Ready Pin) ---
    #if (ANALYZER_READER_PIN_READY != -1)
        IOConfigAsOutput(1ULL << ANALYZER_READER_PIN_READY, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE);
        gpio_set_level(ANALYZER_READER_PIN_READY, 0);
    #endif

    // --- SPI Slave Configuration ---
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_READER_SPI_MISO,
        .mosi_io_num = ANALYZER_READER_SPI_MOSI,
        .sclk_io_num = ANALYZER_READER_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // Allow full packet transfer
        .max_transfer_sz = ANALYZER_PKT_SIZE_BYTE,
    };

    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = ANALYZER_READER_SPI_CS,
        .mode = 3,
        .queue_size = 1,
        .flags = 0,
    };

    ret = spi_slave_initialize(ANALYZER_READER_SPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ARErr("spi_slave_initialize failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ARLog("SPI Slave initialized.");
    next_response_cmd = AM_CMD_NOP;

    // --- Main Loop ---
    while(1) {
        // --- 1. Prepare TX Data ---
        // Clean buffer
        memset(PktTx, 0, ANALYZER_PKT_SIZE_BYTE);

        // Fill data based on what Master asked LAST time
        switch (next_response_cmd) {
            case AM_CMD_REQ_TOUCH:
                // Format response as RxTouch
                PktTx->RxTouch.Response = ANALYZER_READER_ACK;
                PktTx->RxTouch.SequenceNumber = 0; // TODO: Add sequence counter
                PktTx->RxTouch.TouchSize = 2;      // X + Y = 2 Words
                // Fill Payload
                PktTx->RxTouch.Touch[0] = ScreenTouchX;
                PktTx->RxTouch.Touch[1] = ScreenTouchY;
                ARLog("Resp: TOUCH <X:%d, Y:%d>", ScreenTouchX, ScreenTouchY);
                break;
            
            case AM_CMD_REQ_ID:
                // Format response as RxData
                PktTx->RxData.Response = ANALYZER_READER_ACK;
                PktTx->RxData.DataSize = 1;
                PktTx->RxData.Data[0] = ANALYZER_READER_ID;
                ARLog("Resp: ID <0x%04X>", ANALYZER_READER_ID);
                break;

            case AM_CMD_REQ_TEST:
                PktTx->RxData.Response = ANALYZER_READER_ACK;
                PktTx->RxData.DataSize = TOUCH_PAYLOAD_WORDS; // Fill max
                for(int i = 0; i < 10; i++) {
                     PktTx->RxData.Data[i] = (Word_t)rand();
                }
                ARLog("Resp: TEST DATA");
                break;

            default: // NOP or Unknown
                PktTx->RxData.Response = ANALYZER_READER_ACK; 
                break;
        }

        // --- 2. Setup Transaction ---
        memset(&trans, 0, sizeof(trans));
        trans.length = ANALYZER_PKT_SIZE_BYTE * 8; // Bits
        trans.tx_buffer = PktTx; // Pointer to union
        trans.rx_buffer = PktRx; // Pointer to union

        // --- 3. Queue & Signal Ready ---
        ret = spi_slave_queue_trans(ANALYZER_READER_SPI_HOST, &trans, portMAX_DELAY);
        if (ret != ESP_OK) {
            ARErr("Queue failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 1);
        #endif

        // --- 4. Wait for Transfer ---
        ret = spi_slave_get_trans_result(ANALYZER_READER_SPI_HOST, &ret_trans, portMAX_DELAY);

        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 0);
        #endif

        if (ret == ESP_OK) {
            // --- 5. Process Received Data (From Master) ---
            // Master sends data in "Tx" format (Command, Args)
            Word_t rcv_cmd = PktRx->Tx.Command;
            Word_t rcv_seq = PktRx->Tx.SequenceNumber;
            Word_t rcv_arg = PktRx->Tx.Argument[0]; // Example arg

            ARLog("RX: CMD=0x%04X, SEQ=%d, ARG[0]=0x%04X", rcv_cmd, rcv_seq, rcv_arg);
            
            // Update state for next loop
            next_response_cmd = rcv_cmd;
            
        } else {
            ARErr("Transfer failed: %s", esp_err_to_name(ret));
        }
    }

cleanup:
    if (PktTx) heap_caps_free(PktTx);
    if (PktRx) heap_caps_free(PktRx);
    spi_slave_free(ANALYZER_READER_SPI_HOST);
    ARExit("TaskAnalyzerMasterCom exiting.");
    vTaskDelete(NULL);
}

#endif // (FIRMWARE_TYPE == TYPE_ANALYZER_READER)