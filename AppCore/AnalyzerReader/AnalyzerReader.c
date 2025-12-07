#include "AnalyzerReader.h"
#include "All.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

#include "driver/spi_slave.h"
#include <string.h>
#include <stdint.h>

// Static buffers for SPI transactions
static uint16_t* AnalyzerReaderTx = NULL;
static uint16_t* AnalyzerReaderRx = NULL;

// Response data for AM_CMD_REQ_ID
const static uint16_t reader_id = ANALYZER_READER_ID;

/**
 * @brief Task to monitor system resources, like free heap.
 */
void TaskMonitor(void * pv) {
    AREntry("TaskMonitor(%p)", pv);
    uint32_t last_heap = 0;
    while(1) {
        // Using the SystemMonitor component functions for consistency
        uint32_t current_heap = xPortGetFreeHeapSize();
        if (current_heap != last_heap) {
            ARLog("[Monitor] Free Heap: %u bytes", current_heap);
            last_heap = current_heap;
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Match system monitor interval
    }
}

/**
 * @brief Main task to handle SPI slave communication with the Analyzer Master.
 * @details This implementation uses the standard SPI slave driver. The protocol
 *          requires the slave to prepare the response for the *next* transaction
 *          based on the command received in the *current* transaction.
 */
void TaskAnalyzerReader(void * pv) {
    AREntry("TaskAnalyzerReader(%p)", pv);
    esp_err_t ret;

    /// Static transaction structure to persist configuration across loops.
    /// No need to re-allocate or memset every iteration.
    static spi_slave_transaction_t trans;

    // --- Buffer Allocation ---
    // Allocate DMA-capable memory for SPI buffers.
    AnalyzerReaderTx = (uint16_t *)heap_caps_malloc(ANALYZER_READER_TX_SIZE * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (IsNull(AnalyzerReaderTx)) {
        ARErr("Failed to allocate SPI TX buffer.");
        goto cleanup;
    }
    ARLog("Allocated %d bytes for TX buffer.", ANALYZER_READER_TX_SIZE * sizeof(uint16_t));

    AnalyzerReaderRx = (uint16_t *)heap_caps_malloc(ANALYZER_READER_RX_SIZE * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (IsNull(AnalyzerReaderRx)) {
        ARErr("Failed to allocate SPI RX buffer.");
        goto cleanup;
    }
    ARLog("Allocated %d bytes for RX buffer.", ANALYZER_READER_RX_SIZE * sizeof(uint16_t));

    // --- GPIO Configuration ---
    // Configure READY pin as output.
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
        .max_transfer_sz = ANALYZER_READER_RX_SIZE * sizeof(uint16_t),
    };

    // Configuration for the SPI slave device
    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = ANALYZER_READER_SPI_CS,
        .mode = 0, // SPI mode 0
        .queue_size = 1, // We queue one transaction at a time
    };

    // Initialize SPI slave interface
    ret = spi_slave_initialize(ANALYZER_READER_SPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ARErr("spi_slave_initialize failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ARLog("SPI Slave initialized and configured. Waiting for master.");

    // --- Transaction Setup (One-time) ---
    /// Initialize the transaction structure once.
    memset(&trans, 0, sizeof(trans));
    
    /// Set pointers to the DMA-capable buffers.
    trans.tx_buffer = AnalyzerReaderTx;
    trans.rx_buffer = AnalyzerReaderRx;
    
    /// Configure maximum data length in bits.
    trans.length = ANALYZER_READER_RX_SIZE * sizeof(uint16_t) * 8;

    // --- Main Loop ---
    // Pre-fill the first TX buffer with some default data (e.g., an idle/ready state)
    memset(AnalyzerReaderTx, 0, ANALYZER_READER_TX_SIZE * sizeof(uint16_t));

    // Signal to the master that we are ready for a command (i.e., the TX buffer is loaded)
    #if (ANALYZER_READER_PIN_READY != -1)
        gpio_set_level(ANALYZER_READER_PIN_READY, 1);
    #endif

    while(1) {
        ARLog("[TaskAnalyzerReader] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
                AnalyzerReaderRx[0], AnalyzerReaderRx[1], AnalyzerReaderRx[2], AnalyzerReaderRx[3]);
        ARLog("[TaskAnalyzerReader] TX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
                AnalyzerReaderTx[0], AnalyzerReaderTx[1], AnalyzerReaderTx[2], AnalyzerReaderTx[3]);

        /// Wait for the master to initiate a transaction. This is a blocking call.
        /// We pass the address of our static 'trans' struct.
        ret = spi_slave_queue_trans(ANALYZER_READER_SPI_HOST, &trans, portMAX_DELAY);

        // Lower the READY signal once the transaction has started/completed
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 0);
        #endif

        if (ret != ESP_OK) {
            ARErr("spi_slave_queue_trans failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(1000)); // Wait before retrying
            continue;
        }

        // Transaction is complete. Process received data and prepare the *next* response.
        // Protocol assumption: Master sends 16-bit CMD, then 16-bit ARG/ADDR.
        uint16_t received_cmd = AnalyzerReaderRx[0];
        uint16_t received_arg = AnalyzerReaderRx[1];

        ARLog("Transferred %d bytes. RX CMD: 0x%04X, ARG: 0x%04X", trans.trans_len / 8, received_cmd, received_arg);

        /// Prepare the response for the NEXT transaction in AnalyzerReaderTx.
        /// Since 'trans.tx_buffer' already points to 'AnalyzerReaderTx', 
        /// modifying the buffer content here is sufficient for the next call.
        switch (received_cmd) {
            case AM_CMD_NOP:
                ARLog("Master just sent NOP to get data!", reader_id);
                AnalyzerReaderTx[0] = ANALYZER_READER_ACK;
                break;
            case AM_CMD_REQ_ID:
                ARLog("Master requested ID. Preparing response 0x%04X.", reader_id);
                AnalyzerReaderTx[0] = ANALYZER_READER_ACK;
                AnalyzerReaderTx[1] = ANALYZER_READER_ID;
                break;
            case AM_CMD_REQ_TEST:
                ARLog("Master requested test data. Preparing random data.");
                esp_fill_random(AnalyzerReaderTx, ANALYZER_READER_TX_SIZE * sizeof(uint16_t));
                AnalyzerReaderTx[0] = ANALYZER_READER_ACK;
                break;
            default:
                ARLog("Master sent unknown command: 0x%04X", received_cmd);
                // Prepare an error response or default state
                AnalyzerReaderTx[0] = ANALYZER_READER_NACK; // Example error code
                break;
        }
        
        // Signal to the master that we are ready for a command (i.e., the TX buffer is loaded)
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 1);
        #endif

        /// A small delay is good practice, though strictly speaking slave is driven by master clock.
        vTaskDelay(pdMS_TO_TICKS(10));
    }

cleanup:
    ARLog("Cleaning up TaskAnalyzerReader...");
    if (AnalyzerReaderTx) {
        heap_caps_free(AnalyzerReaderTx);
        AnalyzerReaderTx = NULL;
    }
    if (AnalyzerReaderRx) {
        heap_caps_free(AnalyzerReaderRx);
        AnalyzerReaderRx = NULL;
    }
    spi_slave_free(ANALYZER_READER_SPI_HOST);

    ARExit("TaskAnalyzerReader exiting.");
    vTaskDelete(NULL);
}


#endif // (FIRMWARE_TYPE == TYPE_ANALYZER_READER)
