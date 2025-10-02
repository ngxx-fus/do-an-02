#ifndef __ESP_MULTITASK_WRAPPER__
#define __ESP_MULTITASK_WRAPPER__

#include "freertos/FreeRTOS.h"   /// core FreeRTOS definitions (config, types)
#include "freertos/task.h"       /// task management (xTaskCreate, vTaskDelay, vTaskDelete)
#include "freertos/portmacro.h"  /// architecture-specific macros (portTICK_PERIOD_MS, critical sections)
#include "freertos/semphr.h"     /// synchronization (xSemaphoreCreate, xSemaphoreTake, xSemaphoreGive)

#ifndef __enterCriticalSection
    /// Enter critical section with given mutex (disable interrupts for this region)
    #define __enterCriticalSection(mutex_ptr) taskENTER_CRITICAL(mutex_ptr)
#endif

#ifndef __exitCriticalSection
    /// Exit critical section with given mutex (restore interrupts after region)
    #define __exitCriticalSection(mutex_ptr) taskEXIT_CRITICAL(mutex_ptr)
#endif

#endif 