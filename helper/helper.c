#include "general.h"

#pragma message("helper.c: Compling helper.c")

/// @brief Configure GPIO pins as output based on a 64-bit mask.
def GPIOConfigOutputMask(uint64_t mask) {
    __entry("GPIOConfigOutputMask(%p)", mask);
    
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = mask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    def ret = gpio_config(&io_conf);
    __exit("GPIOConfigOutputMask() : %s", getDefRetStat_Str(ret)); 
    return ret;
}

/// @brief Configure GPIO pins as input based on a 64-bit mask.
def GPIOConfigInputMask(uint64_t mask) {
    __sys_log1("GPIOConfigInputMask(%p)", mask);

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = mask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    def ret = gpio_config(&io_conf);
    __exit("GPIOConfigOutputMask() : %s", getDefRetStat_Str(ret)); 
    return ret;
}

/// @brief [Fast] Configure GPIO pins as input (disable output) based on a 64-bit mask.
void GPIOFastConfigInputMask(uint64_t mask) {
    __sys_log1("GPIOFastConfigInputMask(%p)", mask);

    uint32_t low = (uint32_t)(mask & 0xFFFFFFFFULL);
    uint32_t high = (uint32_t)(mask >> 32);

    if (low)  REG_WRITE(GPIO_ENABLE_W1TC_REG, low);
    if (high) REG_WRITE(GPIO_ENABLE1_W1TC_REG, high);
}

/// @brief [Fast] Configure GPIO pins as output (enable output) based on a 64-bit mask.
void GPIOFastConfigOutputMask(uint64_t mask) {
    __sys_log1("GPIOFastConfigOutputMask(%p)", mask);

    uint32_t low = (uint32_t)(mask & 0xFFFFFFFFULL);
    uint32_t high = (uint32_t)(mask >> 32);

    if (low)  REG_WRITE(GPIO_ENABLE_W1TS_REG, low);
    if (high) REG_WRITE(GPIO_ENABLE1_W1TS_REG, high);
}


unsigned int genRandNum(unsigned int seed_input) {
    // --- 1. Persistent state ---
    static uint32_t last_known_random_state = 104729u; // initialized with a prime
    static int last_known_seed_input = 0;

    uint32_t x;

    // --- 2. Select initial seed ---
    if (last_known_seed_input == seed_input) {
        x = last_known_random_state;
    } else {
        // start from new seed, add prime noise
        x = (uint32_t)seed_input ^ 0xA5A5A5A5u;
    }
    last_known_seed_input = seed_input;

    // --- 3. Prime constants for nonlinear mixing ---
    const uint32_t P1 = 7919u;
    const uint32_t P2 = 104729u;
    const uint32_t P3 = 65537u;
    const uint32_t P4 = 439u;
    const uint32_t P5 = 1223u;

    // --- 4. Chaotic transformations ---
    x = (x * P1 + P3) ^ (x >> 11);
    x = (x << 7) - (x >> 5) + P4;
    x ^= (x * P2);
    x = (x ^ (x >> 17)) + (x << 13);

    // --- 5. Differential and derivative noise ---
    uint32_t dx = x - ((x >> 1) | (x << 31));
    uint32_t ddx = dx ^ (dx >> 3) ^ (dx << 5);

    // --- 6. Amplify chaos using primes ---
    x ^= (ddx * P5);
    x ^= (dx << 9) | (dx >> 23);

    // --- 7. Update persistent state ---
    last_known_random_state = x ^ (x >> 16) ^ (x << 7) ^ P2;

    // --- 8. Return full 32-bit random number ---
    return last_known_random_state;
}

const char * getDefRetStat_Str(enum DEFAULT_RETURN_STATUS ret) {
    switch (ret) {
        case OKE:                 return STR_OKE;
        case ERR:                 return STR_ERR;
        case ERR_NULL:            return STR_ERR_NULL;
        case ERR_MALLOC_FAILED:   return STR_ERR_MALLOC_FAILED;
        case ERR_TIMEOUT:         return STR_ERR_TIMEOUT;
        case ERR_BUSY:            return STR_ERR_BUSY;
        case ERR_INVALID_ARG:     return STR_ERR_INVALID_ARG;
        case ERR_OVERFLOW:        return STR_ERR_OVERFLOW;
        case ERR_UNDERFLOW:       return STR_ERR_UNDERFLOW;
        case ERR_NOT_FOUND:       return STR_ERR_NOT_FOUND;
        case ERR_ALREADY_EXISTS:  return STR_ERR_ALREADY_EXISTS;
        case ERR_NOT_IMPLEMENTED: return STR_ERR_NOT_IMPLEMENTED;
        case ERR_UNSUPPORTED:     return STR_ERR_UNSUPPORTED;
        case ERR_IO:              return STR_ERR_IO;
        case ERR_PERMISSION:      return STR_ERR_PERMISSION;
        case ERR_CRC:             return STR_ERR_CRC;
        case ERR_INIT_FAILED:     return STR_ERR_INIT_FAILED;
        
        default:                  return "UNKNOWN_RETURN_CODE";
    }
}