#include "All.h"

#if (SYSTEM_MON_EN == 1)

/// @brief Gets the total free heap size available in the system.
/// @return Total free heap size in bytes.
size_t SysMonGetFreeHeapSize(void) {
    return esp_get_free_heap_size();
}

/// @brief Gets the free heap size available in internal RAM.
/// @return Free internal heap size in bytes.
size_t SysMonGetInternalFreeHeapSize(void) {
    return heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
}

/// @brief Gets the free heap size available in external PSRAM.
/// @return Free PSRAM size in bytes. Returns 0 if PSRAM is not available or disabled.
size_t SysMonGetPsramFreeHeapSize(void) {
    #if (CONFIG_SPIRAM_USE_MALLOC || CONFIG_SPIRAM)
        return heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    #else
        return 0;
    #endif
}

/// @brief Gets the stack high water mark for a specific task.
/// @details The high water mark is the minimum amount of free stack space a task has had.
/// @param taskHandle Handle to the task. Use NULL for the currently running task.
/// @return Stack high water mark in bytes.
UBaseType_t SysMonGetTaskStackHWM(TaskHandle_t taskHandle) {
    return uxTaskGetStackHighWaterMark(taskHandle);
}

/// @brief Prints a formatted table of all running tasks and their states.
/// @note This is a heavy, debug-only function. It causes high CPU load on IPC tasks.
void SysMonPrintTaskList(void) {
    // A buffer large enough to hold the task list.
    // Using static to avoid placing a large buffer on the stack.
    static char task_list_buffer[2048];

    // Get the task list.
    vTaskList(task_list_buffer);

    // Use a critical section to prevent log output from being interleaved.
    // This is still risky if the UART is slow. A mutex would be safer.
    portENTER_CRITICAL(&__LogSpinLock);
    ets_printf("[%lld] [log] [Monitor] --- Task States & Stack Usage (vTaskList) ---\n", esp_timer_get_time());
    ets_printf("Task Name\t\tStatus\tPrio\tHWM (bytes)\tTask#\n");
    ets_printf("****************************************************************\n");
    ets_printf("%s", task_list_buffer);
    ets_printf("****************************************************************\n\n");
    portEXIT_CRITICAL(&__LogSpinLock);
}

/// @brief Prints a formatted table of task runtime statistics (CPU usage).
/// @note This is a heavy, debug-only function. It causes high CPU load on IPC tasks.
void SysMonPrintRuntimeStats(void) {
    // A buffer large enough to hold the runtime stats.
    // Using static to avoid placing a large buffer on the stack.
    static char runtime_stats_buffer[2048];

    // Get the runtime stats.
    vTaskGetRunTimeStats(runtime_stats_buffer);

    // Use a critical section to prevent log output from being interleaved.
    portENTER_CRITICAL(&__LogSpinLock);
    ets_printf("[%lld] [log] [Monitor] --- Task CPU Usage (vTaskGetRunTimeStats) ---\n", esp_timer_get_time());
    ets_printf("Task Name\t\tAbs Time (us)\t\t%%CPU\n");
    ets_printf("****************************************************************\n");
    ets_printf("%s", runtime_stats_buffer);
    ets_printf("****************************************************************\n\n");
    portEXIT_CRITICAL(&__LogSpinLock);
}

/// @brief Task to periodically log system resource usage (Heap, Stack, etc.).
void TaskSystemMonitor(void * pv) {
    /// Waiting for essential init
    while(SYSTEM_STAGE < SYSTEM_INIT_N(1)) vTaskDelay(1);
    /// Start the task
    SysMonEntry("TaskSystemMonitor(%p)", pv);
    
    while(1) {
        // --- Heap Memory ---
        size_t total_free = SysMonFreeHeapSize();
        size_t internal_free = SysMonFreeInternalHeapSize();
        size_t psram_free = SysMonFreePsramHeapSize();

        SysMonLog("[Monitor] Free Heap: Total=%u, Internal=%u, PSRAM=%u", 
                  (uint32_t)total_free, (uint32_t)internal_free, (uint32_t)psram_free);

        // --- Stack Memory for this task ---
        // Add a final line for this monitor task's own HWM for easy checking
        SysMonLog("[Monitor] Monitor Task Stack HWM: %u bytes", TaskMonFreeStackSize());

        // --- Full System Stats (DEBUG ONLY) ---
        // The functions below are very resource-intensive and can cause high CPU load
        // on IPC tasks, potentially destabilizing the system.
        // They are disabled by default. Uncomment them only for temporary debugging.
        SysMonPrintTaskList();
        SysMonPrintRuntimeStats();

        DelayMs(SYS_MON_INTERVAL);
    }
}

#endif /// (SYSTEM_MON_EN == 1)