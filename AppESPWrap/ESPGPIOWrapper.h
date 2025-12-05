/// @file   espGPIOWrapper.h
/// @brief  GPIO wrapper for ESP32; provides config helpers and fast direct-register macros.

#ifndef __ESP_GPIO_WRAPPER_H__
#define __ESP_GPIO_WRAPPER_H__

#include <stdint.h>             /// Standard fixed-width integer types
#include <stdlib.h>             /// Standard library definitions (malloc, free, etc.)
#include <stdbool.h>            /// Boolean type definitions
#include <stdarg.h>             /// Variable argument handling

#include "driver/gpio.h"        /// ESP-IDF High-level GPIO driver
#include "esp_log.h"            /// ESP logging utilities
#include "esp_timer.h"          /// High-resolution timer API

#include "rom/ets_sys.h"        /// ROM functions (ets_printf, ets_delay_us)
#include "hal/gpio_ll.h"        /// Low-level GPIO hardware abstraction layer
#include "soc/gpio_struct.h"    /// GPIO hardware register structure
#include "soc/gpio_reg.h"       /// GPIO register addresses and bit masks

/// @brief Configure a GPIO pin with specific mode, pull-up/down settings, and interrupt type
/// @param pin_bit_mask Bitmask of the GPIO(s) to configure
/// @param mode         GPIO mode (e.g., GPIO_MODE_INPUT, GPIO_MODE_OUTPUT)
/// @param pull_up_en   Enable/Disable pull-up resistor
/// @param pull_down_en Enable/Disable pull-down resistor
/// @param intr_type    Interrupt type (e.g., GPIO_INTR_POSEDGE, GPIO_INTR_DISABLE)
void IOConfig(uint64_t pin_bit_mask, gpio_mode_t mode, gpio_pullup_t pull_up_en, gpio_pulldown_t pull_down_en, gpio_int_type_t intr_type);

/// @brief Configure GPIO(s) as output with no internal pull resistors and interrupts disabled
/// @param pin_bit_mask Bitmask of the GPIO(s) to configure
void void IOConfigAsOutput(uint64_t pin_bit_mask, gpio_pullup_t pull_up_en, gpio_pulldown_t pull_down_en);

/// @brief Configure GPIO(s) as input with no internal pull resistors and interrupts disabled
/// @param pin_bit_mask Bitmask of the GPIO(s) to configure
void IOConfigAsInput(uint64_t pin_bit_mask, gpio_pullup_t pull_up_en, gpio_pulldown_t pull_down_en);

/// @brief Fast set High for GPIO 0-31 (Write 1 to Set)
#define GPIOSetHigh(GPIO_MASK)  GPIO.out_w1ts = (GPIO_MASK)

/// @brief Fast set Low for GPIO 0-31 (Write 1 to Clear)
#define GPIOSetLow(GPIO_MASK)   GPIO.out_w1tc = (GPIO_MASK)

/// @brief Fast set High for GPIO 32-64 (Write 1 to Set - High Register)
#define GPIOSetHigh1(GPIO_MASK) GPIO.out1_w1ts = (GPIO_MASK)  

/// @brief Fast set Low for GPIO 32-64 (Write 1 to Clear - High Register)
#define GPIOSetLow1(GPIO_MASK)  GPIO.out1_w1tc = (GPIO_MASK)  

// typedef void IRAM_ATTR (isrFunc_t)(void *pv);

#endif