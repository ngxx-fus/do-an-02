#include "helper.h"
#include "stdint.h"
#include "return.h"

int generateRandom(int seed_input) {
    // --- 1. Persistent state ---
    static uint32_t last_known_random_state = 104729; // initialized with a prime
    static int last_known_seed_input = 0;

    uint32_t x;

    // --- 2. Select initial seed ---
    if (seed_input < 0 || last_known_seed_input == seed_input) {
        // Reuse previous valid random state
        x = last_known_random_state;
    } else {
        // Start from new seed
        x = (uint32_t)seed_input;
    }
    
    last_known_seed_input = seed_input;

    // --- 3. Prime constants for nonlinear mixing ---
    const uint32_t P1 = 7919;     // prime multiplier
    const uint32_t P2 = 104729;   // large prime
    const uint32_t P3 = 65537;    // Fermat prime
    const uint32_t P4 = 439;      // small prime
    const uint32_t P5 = 1223;     // extra prime for chaos

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
    last_known_random_state = (x ^ (x >> 16) ^ (x << 7)) + P2;

    // --- 8. Constrain result to [1, 9999] (avoid 0)
    int result = (int)(last_known_random_state % 10000);
    return result ? result : 1;
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