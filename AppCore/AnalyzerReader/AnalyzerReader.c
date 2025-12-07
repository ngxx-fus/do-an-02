#include "All.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

#include "driver/spi_slave_hd.h"
#include <string.h>

// Static buffers for SPI transactions
static HalfWord_t* spi_tx_buffer = NULL;
static HalfWord_t* spi_rx_buffer = NULL;

// Response data for AM_CMD_REQ_ID
const static HalfWord_t reader_id = ANALYZER_READER_ID;

/**
 * @brief Task to monitor system resources, like free heap.
 */
void TaskMonitor(void * pv) {
    AREntry("TaskMonitor(%p)", pv);
    uint32_t last_heap = 0;
    while(1) {
        // Using the SystemMonitor component functions for consistency
        uint32_t current_heap = SysMonGetFreeHeapSize();
        if (current_heap != last_heap) {
            ARLog("[Monitor] Free Heap: %u bytes", current_heap);
            last_heap = current_heap;
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Match system monitor interval
    }
}

/**
 * @brief Main task to handle SPI slave communication with the Analyzer Master.
 * @details This implementation uses the Half-Duplex (HD) SPI slave driver to
 *          correctly handle the Master's command-response protocol. It receives
 *          a command and argument, then sends the appropriate data back in the
 *          same transaction.
 */
void TaskAnalyzerReader(void * pv) {
    AREntry("TaskAnalyzerReader(%p)", pv);
    esp_err_t ret;

    // --- Buffer Allocation ---
    // Allocate DMA-capable memory for SPI buffers.
    spi_tx_buffer = (HalfWord_t *)heap_caps_malloc(ANALYZER_READER_TX_SIZE * sizeof(HalfWord_t), MALLOC_CAP_DMA);
    if (IsNull(spi_tx_buffer)) {
        ARErr("Failed to allocate SPI TX buffer.");
        goto cleanup;
    }
    ARLog("Allocated %d bytes for TX buffer.", ANALYZER_READER_TX_SIZE * sizeof(HalfWord_t));

    spi_rx_buffer = (HalfWord_t *)heap_caps_malloc(ANALYZER_READER_RX_SIZE * sizeof(HalfWord_t), MALLOC_CAP_DMA);
    if (IsNull(spi_rx_buffer)) {
        ARErr("Failed to allocate SPI RX buffer.");
        goto cleanup;
    }
    ARLog("Allocated %d bytes for RX buffer.", ANALYZER_READER_RX_SIZE * sizeof(HalfWord_t));

    // --- GPIO Configuration ---
    // Configure READY pin as output.
    #if (ANALYZER_READER_PIN_READY != -1)
        IOConfigAsOutput(1ULL << ANALYZER_READER_PIN_READY, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE);
        gpio_set_level(ANALYZER_READER_PIN_READY, 0); // Initially not ready
        ARLog("Configured READY pin (%d) as output.", ANALYZER_READER_PIN_READY);
    #else
        ARLog("READY pin is disabled.");
    #endif

    // --- SPI HD Slave Configuration ---
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_READER_SPI_MISO,
        .mosi_io_num = ANALYZER_READER_SPI_MOSI,
        .sclk_io_num = ANALYZER_READER_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    // Configuration for the SPI slave device in Half-Duplex mode
    // This MUST match the master's command/address bit length
    spi_slave_hd_slot_config_t slvcfg = {
        .spics_io_num = ANALYZER_READER_SPI_CS,
        .flags = SPI_SLAVE_HD_DB_BIT_LEN_ACCURATE,
        .mode = 0, // SPI mode 0
        .command_bits = 16,
        .address_bits = 16,
        .dummy_bits = 0,
    };

    ret = spi_slave_hd_init(ANALYZER_READER_SPI_HOST, &buscfg, &slvcfg);
    if (ret != ESP_OK) {
        ARErr("spi_slave_hd_init failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    // --- Prepare Transaction Descriptors ---
    // These descriptors tell the driver what data to send when a specific command is received.

    // Descriptor for AM_CMD_REQ_ID
    spi_slave_hd_data_t id_trans_cfg = {
        .cmd = AM_CMD_REQ_ID,
        .data = (void*)&reader_id,
        .len = sizeof(reader_id),
    };

    // Descriptor for AM_CMD_REQ_TEST
    spi_slave_hd_data_t test_trans_cfg = {
        .cmd = AM_CMD_REQ_TEST,
        .data = spi_tx_buffer,
        .len = ANALYZER_READER_TX_SIZE * sizeof(HalfWord_t), // Max length
    };
    
    // Load the descriptors into the driver
    spi_slave_hd_data_t* trans_configs[] = { &id_trans_cfg, &test_trans_cfg };
    ret = spi_slave_hd_load_trans_config(ANALYZER_READER_SPI_HOST, trans_configs, 2);
    if (ret != ESP_OK) {
        ARErr("spi_slave_hd_load_trans_config failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ARLog("SPI Slave HD initialized and configured. Waiting for master.");

    // --- Main Loop ---
    while(1) {
        // Prepare data for the next potential transaction
        esp_fill_random(spi_tx_buffer, ANALYZER_READER_TX_SIZE * sizeof(HalfWord_t));

        // Signal to the master that we are ready for a command
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 1);
        #endif

        // Arm the driver and wait for a transaction to occur. This is a blocking call.
        spi_slave_hd_event_t event;
        ret = spi_slave_hd_trigger_trans_wait_ret(ANALYZER_READER_SPI_HOST, &event, portMAX_DELAY);

        // Lower the READY signal once the transaction is complete
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 0);
        #endif

        if (ret != ESP_OK) {
            ARErr("spi_slave_hd_trigger_trans_wait_ret failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(1000)); // Wait before retrying
            continue;
        }

        // Process the completed transaction
        if (event.trans_type == SPI_SLAVE_HD_TRANS_TYPE_WRITE) {
            // Master wrote data to us (e.g., AM_CMD_SET_DATA)
            // Not implemented in this example
            ARLog("Master wrote %d bytes.", event.len);

        } else if (event.trans_type == SPI_SLAVE_HD_TRANS_TYPE_READ) {
            // Master read data from us
            switch (event.cmd) {
                case AM_CMD_REQ_ID:
                    ARLog("Master requested ID. Sent 0x%04X. Master ID was 0x%04X.", reader_id, (uint16_t)event.addr);
                    break;
                case AM_CMD_REQ_TEST:
                    // The 'address' field from master contains the requested size in half-words
                    ARLog("Master requested test data. Sent %d bytes (requested %d half-words).", event.len, (uint16_t)event.addr);
                    break;
                default:
                    ARLog("Master read with unknown command: 0x%04X", event.cmd);
                    break;
            }
        }
        // A small delay to prevent busy-looping if something goes wrong
        vTaskDelay(pdMS_TO_TICKS(10));
    }

cleanup:
    ARLog("Cleaning up TaskAnalyzerReader...");
    if (spi_tx_buffer) {
        heap_caps_free(spi_tx_buffer);
        spi_tx_buffer = NULL;
    }
    if (spi_rx_buffer) {
        heap_caps_free(spi_rx_buffer);
        spi_rx_buffer = NULL;
    }
    spi_slave_hd_deinit(ANALYZER_READER_SPI_HOST);

    ARExit("TaskAnalyzerReader exiting.");
    vTaskDelete(NULL);
}

#endif // (FIRMWARE_TYPE == TYPE_ANALYZER_READER)