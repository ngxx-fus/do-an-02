#include "All.h"
#include "esp_heap_caps.h" // For memory monitoring

LCD32Dev_t * lcd32 = NULL; 

/// @brief Statically allocated Look-Up Table for the main LCD to save HEAP memory.
static P16Lut_t lcd_lut;

/**
 * @brief Helper to generate random coordinates with an out-of-bounds margin.
 * @param max_val The maximum valid coordinate (e.g., width or height).
 * @param margin The margin to extend beyond the valid range.
 * @return A random coordinate between -margin and (max_val + margin).
 */
static Dim_t rand_coord(Dim_t max_val, int16_t margin) {
    return (esp_random() % (max_val + 2 * margin)) - margin;
}

/**
 * @brief Task to periodically log system resource usage (Heap, Stack, etc.).
 * @details This function can be run as a separate task to monitor the system's
 *          health, checking for memory leaks or stack overflows.
 */
void TaskSystemMonitor(void * pv) {
    SysEntry("TaskSystemMonitor(%p)", pv);
    
    // A buffer large enough to hold the task list and runtime stats.
    // 2048 bytes should be more than enough for a reasonable number of tasks.
    char task_stats_buffer[2048];
    
    while(1) {
        // --- Task List (like top) ---
        vTaskList(task_stats_buffer);
        // Use a critical section to prevent log output from being interleaved by other tasks.
        #if (SYSTEM_SAFE_THREAD_LOG_EN == 1)
            portENTER_CRITICAL(&__LogSpinLock);
        #endif
        ets_printf("[%lld] [log] [Monitor] --- Task States & Stack Usage (vTaskList) ---\n", esp_timer_get_time());
        ets_printf("Task Name\t\tStatus\tPrio\tHWM (bytes)\tTask#\n");
        ets_printf("****************************************************************\n");
        ets_printf("%s", task_stats_buffer);
        ets_printf("****************************************************************\n\n");
        #if (SYSTEM_SAFE_THREAD_LOG_EN == 1)
            portEXIT_CRITICAL(&__LogSpinLock);
        #endif

        // --- CPU Usage ---
        vTaskGetRunTimeStats(task_stats_buffer);
        #if (SYSTEM_SAFE_THREAD_LOG_EN == 1)
            portENTER_CRITICAL(&__LogSpinLock);
        #endif
        ets_printf("[%lld] [log] [Monitor] --- Task CPU Usage (vTaskGetRunTimeStats) ---\n", esp_timer_get_time());
        ets_printf("Task Name\t\tAbs Time (us)\t\t%%CPU\n");
        ets_printf("****************************************************************\n");
        ets_printf("%s", task_stats_buffer);
        ets_printf("****************************************************************\n\n");
        #if (SYSTEM_SAFE_THREAD_LOG_EN == 1)
            portEXIT_CRITICAL(&__LogSpinLock);
        #endif

        // --- Heap Memory ---
        size_t total_free_heap = esp_get_free_heap_size();
        size_t internal_free_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        size_t psram_free_heap = 0;
        #if (LCD32_CANVAS_IN_PSRAM_EN == 1)
            psram_free_heap = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        #endif
        SysLog("[Monitor] Free Heap: Total=%u, Internal=%u, PSRAM=%u", (uint32_t)total_free_heap, (uint32_t)internal_free_heap, (uint32_t)psram_free_heap);

        // Add a final line for this monitor task's own HWM for easy checking
        SysLog("[Monitor] Monitor Task Stack HWM: %u bytes", uxTaskGetStackHighWaterMark(NULL));

        DelayMs(10000); // Log every 10 seconds
    }
}

void TaskScreen(void * pv){
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
    
    // 5. Main loop: Test all drawing functions cyclically
    while (1){
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
            Dim_t r = rand_coord(lcd32->Height, 50);
            Dim_t c = rand_coord(lcd32->Width, 50);
            LCD32SetCanvasPixel(lcd32, r, c, (Color_t)esp_random());
        }
        LCD32FlushCanvas(lcd32);
        DelayMs(500);

        // --- Test 2: LCD32DirectlyWritePixel ---
        SysLog("[TaskScreen] Testing: LCD32DirectlyWritePixel");
        LCD32FillCanvas(lcd32, (Color_t)esp_random());
        LCD32FlushCanvas(lcd32); // Flush background first
        for (int i = 0; i < 3000; i++) {
            Dim_t r = rand_coord(lcd32->Height, 50);
            Dim_t c = rand_coord(lcd32->Width, 50);
            LCD32DirectlyWritePixel(lcd32, r, c, (Color_t)esp_random());
        }
        DelayMs(500);

        // --- Test 3: LCD32DrawLine ---
        SysLog("[TaskScreen] Testing: LCD32DrawLine");
        LCD32FillCanvas(lcd32, (Color_t)esp_random());
        for (int i = 0; i < 20; i++) {
            Dim_t r0 = rand_coord(lcd32->Height, 50);
            Dim_t c0 = rand_coord(lcd32->Width, 50);
            Dim_t r1 = rand_coord(lcd32->Height, 50);
            Dim_t c1 = rand_coord(lcd32->Width, 50);
            LCD32DrawLine(lcd32, r0, c0, r1, c1, (Color_t)esp_random());
        }
        LCD32FlushCanvas(lcd32);
        DelayMs(500);

        // --- Test 4: LCD32DrawThickLine ---
        SysLog("[TaskScreen] Testing: LCD32DrawThickLine");
        LCD32FillCanvas(lcd32, (Color_t)esp_random());
        for (int i = 0; i < 10; i++) {
            Dim_t r0 = rand_coord(lcd32->Height, 50);
            Dim_t c0 = rand_coord(lcd32->Width, 50);
            Dim_t r1 = rand_coord(lcd32->Height, 50);
            Dim_t c1 = rand_coord(lcd32->Width, 50);
            Dim_t thickness = (esp_random() % 15) + 1;
            LCD32DrawThickLine(lcd32, r0, c0, r1, c1, (Color_t)esp_random(), thickness);
        }
        LCD32FlushCanvas(lcd32);
        DelayMs(500);

        // --- Test 5: LCD32DrawEmptyRect ---
        SysLog("[TaskScreen] Testing: LCD32DrawEmptyRect");
        LCD32FillCanvas(lcd32, (Color_t)esp_random());
        for (int i = 0; i < 10; i++) {
            Dim_t r0 = rand_coord(lcd32->Height, 50);
            Dim_t c0 = rand_coord(lcd32->Width, 50);
            Dim_t r1 = rand_coord(lcd32->Height, 50);
            Dim_t c1 = rand_coord(lcd32->Width, 50);
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
                points[i].row = rand_coord(lcd32->Height, 50);
                points[i].col = rand_coord(lcd32->Width, 50);
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
                points[i].row = rand_coord(lcd32->Height, 50);
                points[i].col = rand_coord(lcd32->Width, 50);
            }
            LCD32DrawFilledPolygon(lcd32, points, num_points, (Color_t)esp_random());
        }
        LCD32FlushCanvas(lcd32);
        DelayMs(500);

        // --- Font Tests (DrawChar and DrawText for each font) ---
        const GFXfont* fonts_to_test[] = {
            &fontTitle, &fontHeading01, &fontHeading02, &fontHeading03, &mainFont, &fontBody, &fontNote
        };
        const char* font_names[] = {
            "fontTitle (24pt)", "fontHeading01 (18pt)", "fontHeading02 (12pt)", 
            "fontHeading03 (9pt)", "mainFont (9pt)", "fontBody (9pt)", "fontNote (Picopixel)"
        };
        size_t num_fonts = sizeof(fonts_to_test) / sizeof(fonts_to_test[0]);

        for (size_t f = 0; f < num_fonts; f++) {
            const GFXfont* current_font = fonts_to_test[f];
            const char* current_font_name = font_names[f];

            // --- Test: LCD32DrawChar with current font ---
            SysLog("[TaskScreen] Testing: LCD32DrawChar with %s", current_font_name);
            Color_t bg_color = (Color_t)esp_random();
            Color_t fg_color = (Color_t)esp_random();
            LCD32FillCanvas(lcd32, bg_color);
            // Use fewer characters for larger fonts to avoid clutter
            int char_count = (current_font->yAdvance > 20) ? 20 : 50;
            for (int i = 0; i < char_count; i++) {
                Dim_t r = rand_coord(lcd32->Height, 20);
                Dim_t c = rand_coord(lcd32->Width, 20);
                char ch = (esp_random() % (126 - 32)) + 32; // Printable ASCII
                LCD32DrawChar(lcd32, r, c, ch, current_font, fg_color);
            }
            LCD32FlushCanvas(lcd32);
            DelayMs(500);

            // --- Test: LCD32DrawText with current font ---
            SysLog("[TaskScreen] Testing: LCD32DrawText with %s", current_font_name);
            LCD32FillCanvas(lcd32, (Color_t)esp_random());
            const char* test_strings[] = { "Hello World!", "ESP32 Test", "LCD32 Driver", "Random Text\nNew Line", "Boundary Check" };
            const char* str_to_draw = test_strings[esp_random() % 5];
            Dim_t r = rand_coord(lcd32->Height, 20);
            Dim_t c = rand_coord(lcd32->Width, 20);
            LCD32DrawText(lcd32, r, c, str_to_draw, current_font, (Color_t)esp_random());
            LCD32FlushCanvas(lcd32);
            DelayMs(500);
        }
    }
}
