#include "All.h"

void IOConfig(uint64_t pin_bit_mask, gpio_mode_t mode, gpio_pullup_t pull_up_en, gpio_pulldown_t pull_down_en, gpio_int_type_t intr_type){
    if(pull_up_en <= 0)          pull_up_en     = GPIO_PULLUP_DISABLE;
    if(pull_down_en <= 0)        pull_down_en   = GPIO_PULLDOWN_DISABLE;
    if(intr_type <= 0)           intr_type      = GPIO_INTR_DISABLE;
    gpio_config_t pin = {
        .pin_bit_mask = pin_bit_mask,
        .mode = mode,
        .intr_type = intr_type,
        .pull_up_en = pull_up_en,
        .pull_down_en = pull_down_en,
    };
    gpio_config(&pin);
}

void IOConfigAsOutput(uint64_t pin_bit_mask, gpio_pullup_t pull_up_en, gpio_pulldown_t pull_down_en){
    if(pull_up_en <= 0)          pull_up_en     = GPIO_PULLUP_DISABLE;
    if(pull_down_en <= 0)        pull_down_en   = GPIO_PULLDOWN_DISABLE;
    gpio_config_t pin = {
        .pin_bit_mask = pin_bit_mask,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = pull_up_en,
        .pull_down_en = pull_down_en,
    };
    gpio_config(&pin);
}

void IOConfigAsInput(uint64_t pin_bit_mask, gpio_pullup_t pull_up_en, gpio_pulldown_t pull_down_en){
    if(pull_up_en <= 0)          pull_up_en     = GPIO_PULLUP_DISABLE;
    if(pull_down_en <= 0)        pull_down_en   = GPIO_PULLDOWN_DISABLE;
    gpio_config_t pin = {
        .pin_bit_mask = pin_bit_mask,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = pull_up_en,
        .pull_down_en = pull_down_en,
    };
    gpio_config(&pin);
}
