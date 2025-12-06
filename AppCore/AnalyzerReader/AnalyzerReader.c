#include "All.h"
#include "driver/spi_slave.h"

void TaskMonitor(void * pv) {
    AREntry("TaskMonitor(%p)", pv);
    uint32_t last_heap = 0;
    while(1) {
        uint32_t current_heap = esp_get_free_heap_size();
        if (current_heap != last_heap) {
            ARLog("[Monitor] Free Heap: %u bytes", current_heap);
            last_heap = current_heap;
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void TaskAnalyzerReader(void * pv) {
    AREntry("TaskAnalyzerReader(%p)", pv);

    // --- Buffer Configuration ---
    // This must match the master's read size.
    const size_t buffer_size_bytes = 4096 * sizeof(uint16_t);

    // The slave only needs a transmit buffer for this protocol.
    uint8_t *spi_tx_buffer = NULL;

    // --- Buffer Allocation ---
    // Allocate DMA-capable memory for SPI buffer
    spi_tx_buffer = (uint8_t *)heap_caps_malloc(buffer_size_bytes, MALLOC_CAP_DMA);

    if (IsNull(spi_tx_buffer)) {
        ARErr("[TaskAnalyzerReader] Failed to allocate SPI TX buffer.");
        vTaskDelete(NULL);
        return;
    }
    ARLog("[TaskAnalyzerReader] Allocated %d-byte TX buffer.", buffer_size_bytes);

    // --- GPIO Configuration ---
    // Configure READY pin as output.
    #if (ANALYZER_READER_PIN_READY != -1)
        IOConfigAsOutput(1ULL << ANALYZER_READER_PIN_READY, -1, -1);
        gpio_set_level(ANALYZER_READER_PIN_READY, 0); // Initially not ready
        ARLog("[TaskAnalyzerReader] Configured READY pin (%d) as output.", ANALYZER_READER_PIN_READY);
    #else
        ARLog("[TaskAnalyzerReader] READY pin is disabled.");
    #endif

    // --- SPI Configuration ---
    spi_bus_config_t buscfg = {
        .miso_io_num = ANALYZER_READER_SPI_MISO,
        .mosi_io_num = ANALYZER_READER_SPI_MOSI,
        .sclk_io_num = ANALYZER_READER_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = ANALYZER_READER_SPI_CS,
        .flags = 0,
        .queue_size = 3,
        .mode = 0, // SPI mode 0
    };

    // Initialize SPI slave interface
    esp_err_t ret = spi_slave_initialize(ANALYZER_READER_SPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ARErr("[TaskAnalyzerReader] spi_slave_initialize failed: %s", esp_err_to_name(ret));
        heap_caps_free(spi_tx_buffer);
        vTaskDelete(NULL);
        return;
    }
    ARLog("[TaskAnalyzerReader] SPI Slave initialized successfully.");

    // --- Main Loop ---
    while(1) {
        ARLog("[TaskAnalyzerReader] Preparing data and waiting for master...");

        // 1. Prepare the data to be sent.
        esp_fill_random(spi_tx_buffer, buffer_size_bytes);
        ARLog("[TaskAnalyzerReader] Generated %d random bytes for transmission.", buffer_size_bytes);

        // 2. Prepare the transaction structure.
        spi_slave_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = buffer_size_bytes * 8; // Length in bits
        t.tx_buffer = spi_tx_buffer;
        t.rx_buffer = NULL; // We don't need to receive data in this protocol.

        // 3. Signal the master that we are ready.
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 1);
            ARLog("[TaskAnalyzerReader] READY signal set to HIGH.");
        #endif

        // 4. Wait for the master to initiate and complete the transaction.
        // This function blocks until the transaction is finished.
        ret = spi_slave_transmit(ANALYZER_READER_SPI_HOST, &t, portMAX_DELAY);

        // 5. Lower the READY signal after the transaction.
        #if (ANALYZER_READER_PIN_READY != -1)
            gpio_set_level(ANALYZER_READER_PIN_READY, 0);
            ARLog("[TaskAnalyzerReader] READY signal set to LOW.");
        #endif

        // 6. Check the result.
        if (ret == ESP_OK) {
            // We cannot check the command sent by the master due to ESP-IDF driver limitations.
            // We just assume the transaction was successful if it completed.
            ARLog("[TaskAnalyzerReader] Transaction completed. Sent %d bytes.", t.trans_len / 8);
        } else {
            ARErr("[TaskAnalyzerReader] SPI slave transmit failed: %s", esp_err_to_name(ret));
        }

        // Wait before preparing the next batch of data.
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}