#include "general.h"

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

void GPIOConfig(uint64_t pin_bit_mask, gpio_mode_t mode, gpio_pullup_t pull_up_en, gpio_pulldown_t pull_down_en, gpio_int_type_t intr_type){
    if(pull_up_en < 0)      pull_up_en = GPIO_PULLUP_DISABLE;
    if(pull_down_en < 0)    pull_down_en = GPIO_PULLUP_DISABLE;
    if(intr_type < 0)        pull_down_en = GPIO_PULLUP_DISABLE;
    gpio_config_t pin = {
        .pin_bit_mask = pin_bit_mask,
        .mode = mode,
        .intr_type = intr_type,
        .pull_up_en = pull_up_en,
        .pull_down_en = pull_down_en,
    };
    gpio_config(&pin);
}

void GPIOConfigAsOutput(uint64_t pin_bit_mask){
    gpio_config_t pin = {
        .pin_bit_mask = pin_bit_mask,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_PULLUP_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&pin);
}

void GPIOConfigAsInput(uint64_t pin_bit_mask){
    gpio_config_t pin = {
        .pin_bit_mask = pin_bit_mask,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_PULLUP_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&pin);
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

/// @brief Atomically set a bit (Thread-safe)
/// @param f Pointer to the safeFlag_t variable
/// @param i Bit position (0-31)
void __safeFlagSet(safeFlag_t *f, uint32_t i) {
    atomic_fetch_or(f, (1UL << i));
}

/// @brief Atomically clear a bit (Thread-safe)
/// @param f Pointer to the safeFlag_t variable
/// @param i Bit position (0-31)
void __safeFlagClear(safeFlag_t *f, uint32_t i) {
    atomic_fetch_and(f, ~(1UL << i));
}

/// @brief Atomically toggle a bit (Thread-safe)
/// @param f Pointer to the safeFlag_t variable
/// @param i Bit position (0-31)
void __safeFlagToggle(safeFlag_t *f, uint32_t i) {
    atomic_fetch_xor(f, (1UL << i));
}

/// @brief Atomically check if a bit is set (Thread-safe)
/// @param f The safeFlag_t variable (passed by value is fine for read, but ptr is consistent)
/// @param i Bit position (0-31)
/// @return true if set, false otherwise
bool __safeFlagHas(safeFlag_t *f, uint32_t i) {
    return (atomic_load(f) & (1UL << i)) != 0U;
}


