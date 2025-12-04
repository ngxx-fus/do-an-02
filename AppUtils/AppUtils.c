#include "./All.h"

unsigned int GenerateRandomNumber(unsigned int SeedInput) {
    // --- 1. Persistent state ---
    static uint32_t last_known_random_state = 104729u; // initialized with a prime
    static int last_known_seed_input = 0;

    uint32_t x;

    // --- 2. Select initial seed ---
    if (last_known_seed_input == SeedInput) {
        x = last_known_random_state;
    } else {
        // start from new seed, add prime noise
        x = (uint32_t)SeedInput ^ 0xA5A5A5A5u;
    }
    last_known_seed_input = SeedInput;

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

