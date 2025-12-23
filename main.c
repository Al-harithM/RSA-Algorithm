#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// --- Montgomery Functions ---
uint32_t get_invN(uint32_t N) {
    uint32_t x = N;
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
    // Constant time addition to prevent timing attacks
    x += N & -(t1 < t2); 
    return x;
}

uint32_t get_R_squared(uint32_t N) {
    uint64_t R = (1ULL << 32);
    uint32_t r_mod_n = (uint32_t)(R % N);
    uint64_t r2 = (uint64_t)r_mod_n * r_mod_n;
    return (uint32_t)(r2 % N);
}

uint32_t rsa_montgomery(uint32_t data, uint32_t key, uint32_t N, uint32_t invN, uint32_t R2) {
    uint32_t d_bar = REDC((uint64_t)data * R2, N, invN);
    uint32_t res_bar = REDC((uint64_t)1 * R2, N, invN);
    while (key > 0) {
        if (key % 2 == 1) res_bar = REDC((uint64_t)res_bar * d_bar, N, invN);
        d_bar = REDC((uint64_t)d_bar * d_bar, N, invN);
        key /= 2;
    }
    return REDC((uint64_t)res_bar, N, invN);
}

// --- Math & Prime Helpers ---
uint32_t gcd(uint32_t a, uint32_t b) {
    uint32_t temp;
    while (b != 0) { temp = b; b = a % b; a = temp; }
    return a;
}

int is_prime(uint32_t n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (uint32_t i = 3; i * i <= n; i += 2) if (n % i == 0) return 0;
    return 1;
}

uint32_t generate_prime(uint32_t min, uint32_t max) {
    uint32_t candidate;
    do {
        candidate = min + rand() % (max - min + 1);
        if (candidate % 2 == 0) candidate++;
    } while (!is_prime(candidate));
    return candidate;
}

int64_t extended_gcd(int64_t a, int64_t b, int64_t *x, int64_t *y) {
    if (b == 0) { *x = 1; *y = 0; return a; }
    int64_t x1, y1;
    int64_t d = extended_gcd(b, a % b, &x1, &y1);
    *x = y1; *y = x1 - (a / b) * y1;
    return d;
}

uint32_t mod_inv(uint32_t a, uint64_t m) {
    int64_t x, y;
    extended_gcd((int64_t)a, (int64_t)m, &x, &y);
    return (uint32_t)((x % (int64_t)m + (int64_t)m) % (int64_t)m);
}

// --- Main Execution ---
int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    if (argc < 2) return 1;
    char mode = argv[1][0];

    if (mode == 'g') {
        uint32_t e = 0x101, p, q, N;
        uint32_t min_p = 40000, max_p = 65000;
        
        do {
            p = generate_prime(min_p, max_p);
            q = generate_prime(min_p, max_p);
        } while (p == q || gcd(e, (uint32_t)((uint64_t)(p-1)*(q-1))) != 1);

        N = p * q;
        uint64_t phi = (uint64_t)(p - 1) * (uint64_t)(q - 1);
        uint32_t d = mod_inv(e, phi);

        FILE *f_pub = fopen(argv[2], "w");
        FILE *f_priv = fopen(argv[3], "w");
        if(f_pub && f_priv) {
            fprintf(f_pub, "%08X %08X", e, N);
            fprintf(f_priv, "%08X %08X", d, N);
            fclose(f_pub); fclose(f_priv);
        }
    } 
    else if (mode == 'e' || mode == 'd') {
        uint32_t key, n, data;
        FILE *f_key = fopen(argv[2], "r");
        FILE *f_in = fopen(argv[3], "r");
        if (!f_key || !f_in) return 1;

        if(fscanf(f_key, "%x %x", &key, &n) != 2) return 1;
        if(fscanf(f_in, "%x", &data) != 1) return 1;
        fclose(f_key); fclose(f_in);

        uint32_t invN = get_invN(n);
        uint32_t R2 = get_R_squared(n);
        uint32_t result = rsa_montgomery(data, key, n, invN, R2);

        FILE *f_out = fopen(argv[4], "w");
        if(f_out) {
            fprintf(f_out, "%08X", result);
            fclose(f_out);
        }
    }
    return 0;
}