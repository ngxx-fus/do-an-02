#ifndef __ESP_CUSTOM_LOG_H__
#define __ESP_CUSTOM_LOG_H__

#include <stdio.h>              /// for printf-like formatting
#include <stdarg.h>             /// for variadic macros
#include "esp_timer.h"          /// for esp_timer_get_time()
#include "esp_system.h"         /// for basic ESP system APIs (optional, common include)

#include "../config/generalConfig.h"

#ifndef SYSTEM_LOG_LEVEL
    #define SYSTEM_LOG_LEVEL 1
#endif

#ifndef __sys_err
    #if SYSTEM_LOG_LEVEL >= 1
        /// Print error log with timestamp and "err"
        #define __sys_err(fmt, ...)     ets_printf("[%lld] [err] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else
        #define __sys_err(fmt, ...)
    #endif
#endif

#ifndef __sys_warn
    #if SYSTEM_LOG_LEVEL >= 2
        /// Print entry warn with timestamp
        #define __sys_warn(fmt, ...)   ets_printf("[%lld] [W] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __sys_warn(fmt, ...)
    #endif
#endif

#ifndef __entry
    #if SYSTEM_LOG_LEVEL >= 2
        /// Print entry log with timestamp and "-->"
        #define __entry(fmt, ...)   ets_printf("[%lld] [-->] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __entry(fmt, ...)
    #endif
#endif

#ifndef __exit
    #if SYSTEM_LOG_LEVEL >= 2
        /// Print exit log with timestamp and "<--"
        #define __exit(fmt, ...)    ets_printf("[%lld] [<--] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __exit(fmt, ...)
    #endif
#endif

#ifndef __sys_info
    #if SYSTEM_LOG_LEVEL >= 2
        /// Print entry warn with timestamp
        #define __sys_info(fmt, ...)   ets_printf("[%lld] [Info] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else 
        #define __sys_info(fmt, ...)
    #endif
#endif

#ifndef __sys_log
    #if SYSTEM_LOG_LEVEL >= 3
        /// Print general log with timestamp and "log"
        #define __sys_log(fmt, ...)     ets_printf("[%lld] [log] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else
        #define __sys_log(fmt, ...)
    #endif
#endif

#ifndef __tag_log
    #if SYSTEM_LOG_LEVEL >= 3
        /// Print log with timestamp and custom tag
        #define __tag_log(tag, fmt, ...)     ets_printf("[%lld] [log] [%s] " fmt "\n", esp_timer_get_time(), tag, ##__VA_ARGS__)
    #else
        #define __tag_log(tag, fmt, ...)
    #endif
#endif

#ifndef __sys_log1
    #if SYSTEM_LOG_LEVEL >= 4
        /// Print general log with timestamp and "log" (for log more details)
        #define __sys_log1(fmt, ...)     ets_printf("[%lld] [log] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #else
        /// Print general log with timestamp and "log" (for log more details)
        #define __sys_log1(fmt, ...)
    #endif
#endif

#ifndef __tag_log1
    #if SYSTEM_LOG_LEVEL >= 4
        /// Print log with timestamp and custom tag
        /// Print general log with timestamp and "log" (for log more details)
        #define __tag_log(tag, fmt, ...)     ets_printf("[%lld] [%s] " fmt "\n", esp_timer_get_time(), tag, ##__VA_ARGS__)
    #else
        /// Print general log with timestamp and "log" (for log more details)
        #define __tag_log1(tag, fmt, ...)
    #endif
#endif

#endif 