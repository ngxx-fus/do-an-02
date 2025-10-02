#ifndef __ESP_CUSTOM_LOG_H__
#define __ESP_CUSTOM_LOG_H__

#include <stdio.h>              /// for printf-like formatting
#include <stdarg.h>             /// for variadic macros
#include "esp_timer.h"          /// for esp_timer_get_time()
#include "esp_system.h"         /// for basic ESP system APIs (optional, common include)


#ifndef __entry
    /// Print entry log with timestamp and "-->"
    #define __entry(fmt, ...)   ets_printf("[%lld] [-->] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
#endif

#ifndef __exit
    /// Print exit log with timestamp and "<--"
    #define __exit(fmt, ...)    ets_printf("[%lld] [<--] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
#endif

#ifndef __log
    /// Print general log with timestamp and "log"
    #define __log(fmt, ...)     ets_printf("[%lld] [log] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
#endif

#ifndef __tag_log
    /// Print log with timestamp and custom tag
    #define __tag_log(tag, fmt, ...)     ets_printf("[%lld] [%s] " fmt "\n", esp_timer_get_time(), tag, ##__VA_ARGS__)
#endif

#ifndef __err
    /// Print error log with timestamp and "err"
    #define __err(fmt, ...)     ets_printf("[%lld] [err] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
#endif


#endif 