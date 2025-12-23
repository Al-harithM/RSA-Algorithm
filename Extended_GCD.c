#include <stdio.h>
#include <stdint.h>

int64_t extended_gcd(int64_t a, int64_t b, int64_t *x, int64_t *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    int64_t x1, y1;
    int64_t gcd = extended_gcd(b, a % b, &x1, &y1);

    *x = y1;
    *y = x1 - (a / b) * y1;

    return gcd;
}


int64_t mod_inv(uint32_t a, uint64_t m) {
    int64_t x, y;
    int64_t gcd = extended_gcd((int64_t)a, (int64_t)m, &x, &y);
    if (gcd != 1) {
        return -1; 
    } else {
        // Correctly handle negative results from extended_gcd
        int64_t res = x % (int64_t)m;
        if (res < 0) res += m;
        return res;
    }
}

void find_e_d(uint64_t phi, uint32_t *e, uint32_t *d) {
    *e = 0x00000101; // matches your prime generator

    int64_t result = mod_inv(*e, phi);
    if (result == -1) {
        printf("Failed to find modular inverse!\n");
        *d = 0;
    } else {
        *d = (uint32_t)result;
    }
}
