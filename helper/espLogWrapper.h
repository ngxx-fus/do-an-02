#ifndef __ESP_CUSTOM_LOG_H__
#define __ESP_CUSTOM_LOG_H__

#include <stdio.h>              /// Standard input/output definitions
#include <stdarg.h>             /// Macros for variable arguments
#include "esp_timer.h"          /// ESP32 timer functions (esp_timer_get_time)
#include "esp_system.h"         /// ESP32 system APIs

#include "../config/projectConfig.h"

/// Default system log level if not defined externally (1: Err, 2: Warn, 3: Log, 4: Verbose)
#ifndef SYSTEM_LOG_LEVEL
    #define SYSTEM_LOG_LEVEL 1
#endif

#ifndef __sys_err
    #if SYSTEM_LOG_LEVEL >= 1
        /// Log error message with timestamp and [err] tag
        #define __sys_err(fmt, ...)     ets_printf("[%lld] [err] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else
        #define __sys_err(fmt, ...)
    #endif
#endif

#ifndef __sys_warn
    #if SYSTEM_LOG_LEVEL >= 2
        /// Log warning message with timestamp and [W] tag
        #define __sys_warn(fmt, ...)    ets_printf("[%lld] [W] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __sys_warn(fmt, ...)
    #endif
#endif

#ifndef __entry
    #if SYSTEM_LOG_LEVEL >= 2
        /// Log function entry trace with timestamp and [-->] tag
        #define __entry(fmt, ...)       ets_printf("[%lld] [-->] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __entry(fmt, ...)
    #endif
#endif

#ifndef __exit
    #if SYSTEM_LOG_LEVEL >= 2
        /// Log function exit trace with timestamp and [<--] tag
        #define __exit(fmt, ...)        ets_printf("[%lld] [<--] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __exit(fmt, ...)
    #endif
#endif

#ifndef __sys_info
    #if SYSTEM_LOG_LEVEL >= 2
        /// Log informational message with timestamp and [Info] tag
        #define __sys_info(fmt, ...)    ets_printf("[%lld] [Info] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __sys_info(fmt, ...)
    #endif
#endif

#ifndef __sys_log
    #if SYSTEM_LOG_LEVEL >= 3
        /// Log general message with timestamp and [log] tag
        #define __sys_log(fmt, ...)     ets_printf("[%lld] [log] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else
        #define __sys_log(fmt, ...)
    #endif
#endif

#ifndef __tag_log
    #if SYSTEM_LOG_LEVEL >= 3
        /// Log message with timestamp, [log] tag, and custom sub-tag
        #define __tag_log(tag, fmt, ...)    ets_printf("[%lld] [log] [%s] " fmt "\n", esp_timer_get_time(), tag, ##__VA_ARGS__)
    #else
        #define __tag_log(tag, fmt, ...)
    #endif
#endif

#ifndef __sys_log1
    #if SYSTEM_LOG_LEVEL >= 4
        /// Log detailed verbose message with timestamp and [log] tag
        #define __sys_log1(fmt, ...)    ets_printf("[%lld] [log] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else
        #define __sys_log1(fmt, ...)
    #endif
#endif

#ifndef __tag_log1
    #if SYSTEM_LOG_LEVEL >= 4
        /// Log detailed verbose message with custom tag
        #define __tag_log1(tag, fmt, ...)   ets_printf("[%lld] [%s] " fmt "\n", esp_timer_get_time(), tag, ##__VA_ARGS__)
    #else
        #define __tag_log1(tag, fmt, ...)
    #endif
#endif

#endif