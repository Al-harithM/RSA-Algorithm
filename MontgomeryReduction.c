#include <stdint.h>
#include <stdio.h>

uint32_t get_invN(uint32_t N) {
    uint32_t x = N; // Initial guess

    for (int i = 0; i < 5; i++) {
        x = x * (2 - N * x);
    }
    return x;
}

uint32_t REDC(uint64_t T, uint32_t N, uint32_t invN) {
    uint64_t t1 = T;

    uint32_t m = (uint32_t)T * invN;
    
    uint64_t t2 = (uint64_t)m * N;

    uint32_t x = (uint32_t)((t1 - t2) >> 32);

    if (t1 < t2) {
        x += N;
    }
    
    return x;
}

uint32_t get_R_squared(uint32_t N) {
    // 1. Calculate R mod N
    // R = 2^32. In standard math, 2^32 % N is equivalent to (2^32 - N) if N > 2^31.
    // Since we can't store 2^32 in uint32_t, we use a 64-bit calculation.
    uint64_t R = (1ULL << 32);
    uint32_t r_mod_n = (uint32_t)(R % N);

    uint64_t r2 = (uint64_t)r_mod_n * r_mod_n;
    
    return (uint32_t)(r2 % N);
}