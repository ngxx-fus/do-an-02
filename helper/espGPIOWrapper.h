#ifndef __ESP_GPIO_WRAPPER_H__
#define __ESP_GPIO_WRAPPER_H__

#include <stdint.h>             /// fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdlib.h>             /// fixed-width integer types (uint8_t, int32_t, etc.)
#include <stdbool.h>            /// boolean type (true/false)
#include <stdarg.h>             /// variable arguments (va_list, va_start, va_end)

#include "driver/gpio.h"        /// high-level GPIO driver (gpio_set_level, gpio_get_level, gpio_config)
#include "esp_log.h"            /// logging utilities (ESP_LOGI, ESP_LOGE, ESP_LOGD, etc.)
#include "esp_timer.h"          /// system timer API (esp_timer_get_time, esp_timer_start_once, esp_tim

#include "rom/ets_sys.h"        /// ROM functions (ets_printf, ets_delay_us)
#include "hal/gpio_ll.h"        /// low-level GPIO control (gpio_ll_set_level, gpio_ll_get_level)
#include "soc/gpio_struct.h"    /// direct register access: GPIO register structure
#include "soc/gpio_reg.h"       /// GPIO register addresses and bit fields

typedef void IRAM_ATTR (isrFunc_t)(void *pv);

void setupISR(){
    static int8_t isSetup = 0;
    if(isSetup > 0) return ; 
    isSetup = 1;
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
}

void gpioSetOutpin(uint64_t gpioPinMask){
    gpio_config_t outPin = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = gpioPinMask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&outPin);
}

void gpioSetInput(uint64_t gpioPinMask){
    gpio_config_t inPin = {
        .intr_type      = GPIO_INTR_POSEDGE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   = gpioPinMask,
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_DISABLE
    };
    gpio_config(&inPin);
}

void gpioSetISR(pin_t pin, isrFunc_t *isr){
    setupISR();
    gpio_isr_handler_add(pin, isr, (void*) pin);
}

void gpioSetState(pin_t pin, level_t level){
    if (level)
        GPIO.out_w1ts = (1 << pin);
    else
        GPIO.out_w1tc = (1 << pin);
}

def gpioGetState(pin_t pin){
    return (GPIO.out & __mask32(pin)) ? 0x1 : 0x0;
}

#endif 