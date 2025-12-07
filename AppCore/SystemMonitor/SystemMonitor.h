#ifndef __SYSTEM_MONITOR_H__
#define __SYSTEM_MONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppCore/SystemMonitor/SystemMonitor.h")
#endif

#include "../../AppConfig/All.h"
#include "../../AppUtils/All.h"
#include "../../AppESPWrap/All.h"

#ifndef SYSTEM_MON_EN
    /// @brief Enable/Disable the entire System Monitor component.
    #define SYSTEM_MON_EN       1
#endif


#ifndef SYS_MON_INTERVAL
    /// @brief Delay between two calls of System Monitor component.
    #define SYS_MON_INTERVAL        10000   /// ms
#endif


#if (SYSTEM_MON_EN == 1)

/* --- LOGGING CONFIGURATION --- */
#define SYSTEM_MON_LOG_SECTION

/// @brief Enable standard logging for this module
#define SYSTEM_MON_LOG_EN           1
/// @brief Enable error logging
#define SYSTEM_MON_ERR_EN           1
/// @brief Enable function entry tracing
#define SYSTEM_MON_LOG_ENTRY        1
/// @brief Enable function exit tracing
#define SYSTEM_MON_LOG_EXIT         1

/* --- UTILITY FUNCTIONS --- */

/// @brief Gets the total free heap size available in the system.
/// @return Total free heap size in bytes.
size_t SysMonGetFreeHeapSize(void);

/// @brief Gets the free heap size available in internal RAM.
/// @return Free internal heap size in bytes.
size_t SysMonGetInternalFreeHeapSize(void);

/// @brief Gets the free heap size available in external PSRAM.
/// @return Free PSRAM size in bytes. Returns 0 if PSRAM is not available or disabled.
size_t SysMonGetPsramFreeHeapSize(void);

/// @brief Gets the stack high water mark for a specific task.
/// @details The high water mark is the minimum amount of free stack space a task has had.
/// @param taskHandle Handle to the task. Use NULL for the currently running task.
/// @return Stack high water mark in bytes.
UBaseType_t SysMonGetTaskStackHWM(TaskHandle_t taskHandle);

/// @brief Prints a formatted table of all running tasks and their states.
/// @note This is a heavy, debug-only function. It causes high CPU load on IPC tasks.
void SysMonPrintTaskList(void);

/// @brief Prints a formatted table of task runtime statistics (CPU usage).
/// @note This is a heavy, debug-only function. It causes high CPU load on IPC tasks.
void SysMonPrintRuntimeStats(void);


/* --- WRAPPER MACROS --- */

/// @brief Macro to get the total free heap size.
#define SysMonFreeHeapSize()            SysMonGetFreeHeapSize()

/// @brief Macro to get the free internal heap size.
#define SysMonFreeInternalHeapSize()    SysMonGetInternalFreeHeapSize()

/// @brief Macro to get the free PSRAM heap size.
#define SysMonFreePsramHeapSize()       SysMonGetPsramFreeHeapSize()

/// @brief Macro to get the stack high water mark of the current task.
#define TaskMonFreeStackSize()          SysMonGetTaskStackHWM(NULL)

/* --- CORE TASK --- */

/// @brief Task to periodically log system resource usage (Heap, Stack, etc.).
/// @details This function runs as a separate task to monitor the system's
///          health, checking for memory leaks or stack overflows.
void TaskSystemMonitor(void * pv);

/* --- LOGGING MACROS --- */
#ifdef SYSTEM_MON_LOG_SECTION

    #if (SYSTEM_MON_LOG_EN == 1)
        /// @brief Log standard info message
        #define SysMonLog(...)                 SysLog(__VA_ARGS__)
    #else 
        #define SysMonLog(...)
    #endif 

    #if (SYSTEM_MON_ERR_EN == 1)
        /// @brief Log error message
        #define SysMonErr(...)                 SysErr(__VA_ARGS__)
    #else
        #define SysMonErr(...)
    #endif 

    #if (SYSTEM_MON_LOG_EXIT == 1)
        /// @brief Log function exit
        #define SysMonExit(...)                SysExit(__VA_ARGS__)
    #else
        #define SysMonExit(...)
    #endif 

    #if (SYSTEM_MON_LOG_ENTRY == 1)
        /// @brief Log function entry
        #define SysMonEntry(...)               SysEntry(__VA_ARGS__)
    #else 
        #define SysMonEntry(...)
    #endif 

#endif /// SYSTEM_MON_LOG_SECTION

#endif /// (SYSTEM_MON_EN == 1)

#ifdef __cplusplus
}
#endif

#endif /// __SYSTEM_MONITOR_H__