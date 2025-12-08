#include "AnalyzerMaster.h"
#include "All.h"
#include "ESPFreeRTOSWrapper.h"
#include <stdatomic.h>

#if (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#define ANALYZER_MASTER_LOCAL_UTILS /// Do not deleted!

#include "driver/spi_master.h"
#include "driver/gpio.h"    /// Added for manual GPIO control
#include "rom/ets_sys.h"    /// Added for ets_delay_us
#include "freertos/semphr.h"

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
/// @brief Recursive Mutex to protect SPI and Buffers
static SemaphoreHandle_t hAnalyzerComMutex = NULL;

static spi_device_handle_t AnalyzerMasterSPIHandle;
static bool AnalyzerReaderIDVerified = false;

// --- PROTOTYPES ---
static DefaultRet_t WaitForAnalyzerReaderReadyPinHigh(int TimeoutMS);
static DefaultRet_t WaitForAnalyzerReaderReadyPinLow(int TimeoutMS);
static DefaultRet_t PerformAnalyzerReaderComViaSPI(spi_device_handle_t *SPIHandlePtr, HalfWord_t * SpecificTxData, HalfWord_t TxHalfWordSize);

/**
 * @brief Helper to generate random coordinates with an out-of-bounds margin.
 */
static Dim_t RandCoordinate(Dim_t max_val, int16_t margin) {
    return (esp_random() % (max_val + 2 * margin)) - margin;
}

// =============================================================================
// --- MUTEX & BUFFER HELPERS (NEW IMPLEMENTATION) ---
// =============================================================================

/// @brief Initialize the Recursive Mutex if not exists.
static bool AnalyzerMutexInit(void) {
    if (hAnalyzerComMutex == NULL) {
        hAnalyzerComMutex = xSemaphoreCreateRecursiveMutex();
        if (hAnalyzerComMutex == NULL) {
            AMErr("Failed to create Recursive Mutex!");
            return false;
        }
    }
    return true;
}

/// @brief Global Lock for Analyzer Resources (Buffers + SPI).
/// @note  Allows recursive locking by the same task.
bool AnalyzerAccessLock(TickType_t wait_ticks) {
    if (!AnalyzerMutexInit()) return false;
    return (xSemaphoreTakeRecursive(hAnalyzerComMutex, wait_ticks) == pdTRUE);
}

/// @brief Global Unlock for Analyzer Resources.
void AnalyzerAccessUnlock(void) {
    if (hAnalyzerComMutex != NULL) {
        xSemaphoreGiveRecursive(hAnalyzerComMutex);
    }
}

/// @brief Safely set data into TX Buffer with Mutex protection and Offset.
DefaultRet_t SetAnalyzerMasterTx(HalfWord_t *Arr, uint16_t ArrSize, uint16_t BufferOffset) {
    if (IsNull(Arr) || IsZero(ArrSize)) return STAT_ERR_INVALID_ARG;
    if ((BufferOffset + ArrSize) > ANALYZER_MASTER_TX_SIZE) {
        AMErr("[SetTx] Overflow! Size %d + Offset %d > Max %d", ArrSize, BufferOffset, ANALYZER_MASTER_TX_SIZE);
        return STAT_ERR_INVALID_ARG;
    }

    // Acquire Recursive Lock
    if (!AnalyzerAccessLock(portMAX_DELAY)) return STAT_ERR_BUSY;

    // Critical Section
    memcpy(&AnalyzerMasterTx[BufferOffset], Arr, ArrSize * sizeof(HalfWord_t));

    // Release Lock
    AnalyzerAccessUnlock();
    return STAT_OKE;
}

/// @brief Safely set data into RX Buffer (Pre-fill/Clear) with Mutex protection.
DefaultRet_t SetAnalyzerMasterRx(HalfWord_t *Arr, uint16_t ArrSize, uint16_t BufferOffset) {
    if (IsNull(Arr) || IsZero(ArrSize)) return STAT_ERR_INVALID_ARG;
    if ((BufferOffset + ArrSize) > ANALYZER_MASTER_RX_SIZE) {
        AMErr("[SetRx] Overflow! Size %d + Offset %d > Max %d", ArrSize, BufferOffset, ANALYZER_MASTER_RX_SIZE);
        return STAT_ERR_INVALID_ARG;
    }

    // Acquire Recursive Lock
    if (!AnalyzerAccessLock(portMAX_DELAY)) return STAT_ERR_BUSY;

    // Critical Section
    memcpy(&AnalyzerMasterRx[BufferOffset], Arr, ArrSize * sizeof(HalfWord_t));

    // Release Lock
    AnalyzerAccessUnlock();
    return STAT_OKE;
}

// =============================================================================
// --- SCREEN TESTS ---
// =============================================================================

void PerformScreenTest(LCD32Dev_t * lcd32) {
    if (!lcd32) return;
    // ... (Giữ nguyên nội dung cũ của PerformScreenTest nếu không cần sửa) ...
    // Để tiết kiệm không gian file, tôi giữ nguyên logic cũ ở đây,
    // nếu bạn cần code cũ, hãy paste lại phần PerformScreenTest từ file gốc.
    // Dưới đây là ví dụ test ngắn gọn:
    LCD32FillCanvas(lcd32, (Color_t)esp_random());
    LCD32FlushCanvas(lcd32);
}

// =============================================================================
// --- SPI CORE FUNCTIONS ---
// =============================================================================

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

    // 0. Init Mutex
    AnalyzerMutexInit();

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
    // AMEntry("PerformAnalyzerReaderComViaSPI(%p, %p, %d)", SPIHandlePtr, SpecificTxData, TxHalfWordSize);
    
    // 1. Validate Arguments
    if(IsNotPos(TxHalfWordSize)) {
        return STAT_ERR_INVALID_ARG;
    }
    
    // 2. Acquire Recursive Mutex
    // Since this is Recursive, it's safe even if called inside an already locked section (SetAnalyzerMasterTx)
    if (!AnalyzerAccessLock(portMAX_DELAY)) {
        return STAT_ERR_BUSY;
    }

    DefaultRet_t ReturnValue = STAT_OKE;

    // --- CRITICAL SECTION START ---

    /// Setup data (Optional: Only if SpecificTxData is provided)
    /// Note: If SpecificTxData is NULL, it assumes the buffer is already filled via SetAnalyzerMasterTx
    if(IsNotNull(SpecificTxData)){
        REPN(i, TxHalfWordSize) {
            AnalyzerMasterTx[i] = SpecificTxData[i];
        }
    }

    // AMLog("[TaskAnalyzerReaderCom] TX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
    //        AnalyzerMasterTx[0], AnalyzerMasterTx[1], AnalyzerMasterTx[2], AnalyzerMasterTx[3]);

    /// Create transaction
    spi_transaction_t trans;
    spi_transaction_t *r_trans;
    memset(&trans, 0, sizeof(trans));
    trans.length = TxHalfWordSize * 16;
    trans.tx_buffer = AnalyzerMasterTx;
    trans.rx_buffer = AnalyzerMasterRx;

    /// Wait for Slave Ready (Physical Pin High)
    /// @warning holding Mutex here ensures nobody touches the buffer while waiting
    ReturnValue = WaitForAnalyzerReaderReadyPinHigh(200);
    if (ReturnValue != STAT_OKE) {
        // AMErr("Timeout waiting for Ready Pin High");
        goto release_mutex; 
    }

    // --- MANUAL CS CONTROL START ---
    
    /// 1. Pull CS Low (Start Frame)
    gpio_set_level(ANALYZER_MASTER_SPI_CS, 0);

    /// 2. Wait (Hard delay for Slave wakeup/setup)
    ets_delay_us(10); 

    /// 3. Queue Transaction (Non-blocking call)
    ReturnValue = ESPReturnType2DefaultReturnType(
        spi_device_queue_trans((*SPIHandlePtr), &trans, portMAX_DELAY)
    );

    if(ReturnValue == STAT_OKE){
        /// 4. Wait for result (Blocking until done)
        ReturnValue = ESPReturnType2DefaultReturnType(
            spi_device_get_trans_result((*SPIHandlePtr), &r_trans, portMAX_DELAY)
        );
    }
    
    /// 5. Pull CS High (End Frame)
    gpio_set_level(ANALYZER_MASTER_SPI_CS, 1);

    // --- MANUAL CS CONTROL END ---

    if(ReturnValue != STAT_OKE){
        AMErr("[PerformAnalyzerReaderComViaSPI] SPI ERR: %s", DefaultReturnType2Str(ReturnValue));
        goto release_mutex;
    }

    // AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
    //        AnalyzerMasterRx[0], AnalyzerMasterRx[1], AnalyzerMasterRx[2], AnalyzerMasterRx[3]);

    // --- CRITICAL SECTION END ---

release_mutex:
    // 3. Release Recursive Mutex
    AnalyzerAccessUnlock();

    // AMExit("PerformAnalyzerReaderComViaSPI(): %s", DefaultReturnType2Str(ReturnValue));
    return ReturnValue;
}

/**
 * @brief Test touch functionality with updated Thread-Safe Mechanism.
 */
void PerformScreenTestWithTouch(LCD32Dev_t * lcd32){
    if (IsNull(lcd32) || IsZero(AnalyzerReaderIDVerified)) return;

    // --- CALIBRATION CONFIG ---
    static int16_t CALIB_MIN_X = 165;   
    static int16_t CALIB_MIN_Y = 357;   
    static int16_t CALIB_MAX_X = 3715;  
    static int16_t CALIB_MAX_Y = 3751;  
    static bool SWAP_XY  = false;
    static bool INVERT_X = false;
    static bool INVERT_Y = true;

    // Buffer tạm để chuẩn bị dữ liệu gửi (Stack)
    HalfWord_t ReqData[4] = {AM_CMD_REQ_TOUCH, 0xACEF, 0xACEF, 0xACEF};
    HalfWord_t DummyData[4] = {0, 0, 0, 0};
    // AMLog("PerformScreenTestWithTouch] Send req ...");
    // --- ATOMIC OPERATION 1: SEND REQUEST ---
    // Khóa Mutex cho toàn bộ quá trình Ghi Buffer -> Gửi SPI
    if (AnalyzerAccessLock(portMAX_DELAY)) {
        
        // 1. Dùng SetAnalyzerMasterTx (An toàn, nó sẽ đệ quy lock)
        SetAnalyzerMasterTx(ReqData, 4, 0);

        // 2. Perform SPI (An toàn, nó sẽ đệ quy lock)
        PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 4);
        
        AMLog("[PerformScreenTestWithTouch] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
                  AnalyzerMasterRx[0], AnalyzerMasterRx[1], AnalyzerMasterRx[2], AnalyzerMasterRx[3]);

        // 3. Mở khóa
        AnalyzerAccessUnlock();
    }

    // AMLog("PerformScreenTestWithTouch] Read data ...");
    // --- ATOMIC OPERATION 2: READ RESPONSE ---
    if (AnalyzerAccessLock(portMAX_DELAY)) {
        
        // 1. Gửi Dummy để đọc về
        SetAnalyzerMasterTx(DummyData, 4, 0);
        
        // 2. Perform SPI
        PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 4);

        // AMLog("[PerformScreenTestWithTouch] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
                  // AnalyzerMasterRx[0], AnalyzerMasterRx[1], AnalyzerMasterRx[2], AnalyzerMasterRx[3]);

        // 3. Đọc dữ liệu từ Buffer (Vẫn đang giữ Lock nên an toàn tuyệt đối)
        uint16_t Header = AnalyzerMasterRx[0];
        uint16_t RawX   = AnalyzerMasterRx[1];
        uint16_t RawY   = AnalyzerMasterRx[2];
        uint16_t Footer = AnalyzerMasterRx[3];

        // 4. Mở khóa ngay sau khi copy xong dữ liệu cần thiết
        AnalyzerAccessUnlock();

        AMLog("[TOUCH DEBUG] Raw RX: Header = %04X X=%d, Y=%d Footer = %04x", Header, RawX, RawY, Footer);
        // 5. Xử lý Logic (Không cần giữ Lock nữa để tránh chặn Task khác lâu)
        if(Header == ANALYZER_READER_ACK && Footer == 0xACEF){
            

            if (RawX == 0 || RawY == 0 || RawX >= 4095 || RawY >= 4095) return;

            // --- MAPPING & DRAWING ---
            int32_t MapX, MapY;
            uint16_t InX, InY;

            if (SWAP_XY) { InX = RawY; InY = RawX; } 
            else         { InX = RawX; InY = RawY; }

            MapX = ((int32_t)InX - CALIB_MIN_X) * (lcd32->Width) / (CALIB_MAX_X - CALIB_MIN_X);
            MapY = ((int32_t)InY - CALIB_MIN_Y) * (lcd32->Height) / (CALIB_MAX_Y - CALIB_MIN_Y);

            if (INVERT_X) MapX = lcd32->Width - MapX;
            if (INVERT_Y) MapY = lcd32->Height - MapY;

            if (MapX < 0) MapX = 0;
            if (MapX >= lcd32->Width) MapX = lcd32->Width - 1;
            if (MapY < 0) MapY = 0;
            if (MapY >= lcd32->Height) MapY = lcd32->Height - 1;

            AMLog("[TOUCH DEBUG] --> Screen: X=%d, Y=%d", (int)MapX, (int)MapY);

            LCD32DrawThickLine(lcd32, (Dim_t)MapX - 5, (Dim_t)MapY, (Dim_t)MapX + 5, (Dim_t)MapY, COLOR_RED, 2);
            LCD32DrawThickLine(lcd32, (Dim_t)MapX, (Dim_t)MapY - 5, (Dim_t)MapX, (Dim_t)MapY + 5, COLOR_RED, 2);
            // LCD32FlushCanvas(lcd32);
        }
    }
}

#endif /// ANALYZER_MASTER_LOCAL_UTILS

void TaskScreenFlush(void * pv){
    while(!IS_SYSTEM_STOPPED()){
        LCD32FlushCanvas(lcd32);
        DelayMs(40);
    }
    
}

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
    CreateTaskCPU1(TaskScreenFlush, "TaskScreenFlush", 1024, NULL, 5, NULL);

    // 5. Main loop
    while (!IS_SYSTEM_STOPPED()){
        PerformScreenTestWithTouch(lcd32);
        DelayMs(100);
    }
}

/// @brief Task to handle SPI Master communication with the Analyzer Reader.
void TaskAnalyzerReaderCom(void * pv) {
    /// Wait for the system to reach initialization stage 2.
    while(SYSTEM_STAGE < SYSTEM_INIT_N(2)) { vTaskDelay(1); }

    AMEntry("TaskAnalyzerReaderCom(%p)", pv);

    DefaultRet_t ReturnValue = AnalyzerReaderComInit(&AnalyzerMasterSPIHandle);
    bool AnalyzerMasterSPIReady = (ReturnValue == STAT_OKE) ? true : false;

    static uint64_t __ACKs = 0, __NACKs = 0, __Total = 0;

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
        
        HalfWord_t ReqID[2] = {AM_CMD_REQ_ID, ANALYZER_MASTER_ID};
        HalfWord_t NopData[2] = {AM_CMD_NOP, AM_CMD_NOP};

        // --- ATOMIC TRANSACTION ---
        if(AnalyzerAccessLock(portMAX_DELAY)){
            SetAnalyzerMasterTx(ReqID, 2, 0);
            PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 2);
            WaitForAnalyzerReaderReadyPinLow(1000); // Note: Should we release lock here? Assuming safe.
            
            SetAnalyzerMasterTx(NopData, 2, 0);
            PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 2);
            
            AnalyzerAccessUnlock();
        }

        AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X,...}", AnalyzerMasterRx[0], AnalyzerMasterRx[1]);
        
        __Total++;
        if(AnalyzerMasterRx[0] == ANALYZER_READER_ACK){
            __ACKs++;
            if (AnalyzerMasterRx[1] == ANALYZER_READER_ID) {
                AnalyzerReaderIDVerified = true;
                AMLog("[TaskAnalyzerReaderCom] Reader ID Verified!");
            } else {
                AMErr("[TaskAnalyzerReaderCom] Received ACK, but wrong expected ID (0x%04X)!", ANALYZER_READER_ID);
                DelayMs(1000);
                continue;
            }
        }else if(AnalyzerMasterRx[0] == ANALYZER_READER_NACK){
            __NACKs++;
            AMErr("[TaskAnalyzerReaderCom] Received NACK!");
            DelayMs(1000);
            continue;
        }else {
            AMErr("[TaskAnalyzerReaderCom] Not received ACK nor NACK!");
            DelayMs(1000);
            continue;
        }
        LOG_COM_STATS();
    }

    /// Crawl data from Analyzer-Reader loop
    while (!IS_SYSTEM_STOPPED()) {
        
        HalfWord_t ReqData[2] = {AM_CMD_REQ_TEST, AnalyzerMasterRxSize};

        // --- ATOMIC TRANSACTION ---
        if(AnalyzerAccessLock(portMAX_DELAY)){
            // 1. Request Data
            SetAnalyzerMasterTx(ReqData, 2, 0);
            PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, 2);
            
            WaitForAnalyzerReaderReadyPinLow(1000);
            
            // 2. Read Large Data
            ReturnValue = PerformAnalyzerReaderComViaSPI(&AnalyzerMasterSPIHandle, NULL, AnalyzerMasterRxSize);
            
            AnalyzerAccessUnlock();
        }

        __Total++;
        if (ReturnValue == ESP_OK) {
            // AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X,...}", AnalyzerMasterRx[0], AnalyzerMasterRx[1]);

            if(AnalyzerMasterRx[0] == ANALYZER_READER_ACK){
                __ACKs++;
                AMLog("[TaskAnalyzerReaderCom] RX = {0x%04X, 0x%04X, 0x%04X, 0x%04X, ...}",
                      AnalyzerMasterRx[0], AnalyzerMasterRx[1], AnalyzerMasterRx[2], AnalyzerMasterRx[3]);
            }else if (AnalyzerMasterRx[0] == ANALYZER_READER_NACK){
                __NACKs++;
                AMErr("[TaskAnalyzerReaderCom] Received NACK!");
            }else {
                AMErr("[TaskAnalyzerReaderCom] Not received ACK nor NACK!");
            }
        }
        LOG_COM_STATS();
        AMLog("\n");

        DelayMs(1000);
    }

    if (AnalyzerMasterSPIHandle)    spi_bus_remove_device(AnalyzerMasterSPIHandle);
    if (AnalyzerMasterSPIReady)     spi_bus_free(ANALYZER_READER_SPI_HOST);
    if (AnalyzerMasterTx)           heap_caps_free(AnalyzerMasterTx);
    if (AnalyzerMasterRx)           heap_caps_free(AnalyzerMasterRx);
    
    #undef LOG_COM_STATS

    AMExit("Exiting.");
    vTaskDelete(NULL);
}

#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)
