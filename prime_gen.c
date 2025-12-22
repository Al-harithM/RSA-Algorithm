#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/* GCD using Euclidean algorithm */
uint32_t gcd(uint32_t a, uint32_t b) {
    uint32_t temp;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* Simple primality test for small numbers */
int is_prime(uint32_t n) {
    uint32_t i;
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    
    for (i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

/* Generate random prime in range */
uint32_t generate_prime(uint32_t min, uint32_t max) {
    uint32_t candidate;
    do {
        candidate = min + rand() % (max - min + 1);
        if (candidate % 2 == 0) candidate++;
    } while (!is_prime(candidate));
    return candidate;
}

/* Generate p and q such that gcd(e, phi(N)) = 1 */
int generate_valid_primes(uint32_t e, uint32_t *p, uint32_t *q, uint32_t *N) {
    uint32_t min_prime = 40000;
    uint32_t max_prime = 65000;
    uint64_t N_check;
    uint64_t phi;
    int attempts = 0;
    
    do {
        *p = generate_prime(min_prime, max_prime);
        *q = generate_prime(min_prime, max_prime);
        
        /* Check p != q */
        if (*p == *q) continue;
        
        N_check = (uint64_t)(*p) * (uint64_t)(*q);
        
        /* Check N fits in 32 bits */
        if (N_check > 0xFFFFFFFFULL) continue;
        
        /* Calculate phi(N) = (p-1)(q-1) */
        phi = (uint64_t)(*p - 1) * (uint64_t)(*q - 1);
        
        /* Check gcd(e, phi(N)) = 1 */
        if (gcd(e, (uint32_t)phi) == 1) {
            *N = (uint32_t)N_check;
            return 1; /* Success */
        }
        
        attempts++;
        if (attempts % 10 == 0) {
            printf("Trying... (attempt %d)\n", attempts);
        }
        
    } while (attempts < 1000); /* Max attempts */
    
    return 0; /* Failed */
}

int main(void) {
    uint32_t e, p, q, N;
    uint64_t phi;
    
    srand((unsigned int)time(NULL));
    
    /* Set e = 65537 (0x10001) - standard RSA public exponent */
    e = 0x00000101;
    
    printf("\nSearching for primes p and q where gcd(e, phi(N)) = 1...\n");
    
    if (!generate_valid_primes(e, &p, &q, &N)) {
        printf("Failed to find valid primes after many attempts.\n");
        return 1;
    }
    
    /* Calculate phi(N) */
    phi = (uint64_t)(p - 1) * (uint64_t)(q - 1);
    
    printf("\n=== Results ===\n");
    printf("Public exponent e = %u (0x%08X)\n", e, e);
    printf("Prime p = %u (0x%08X)\n", p, p);
    printf("Prime q = %u (0x%08X)\n", q, q);
    printf("N = p*q = %u (0x%08X)\n", N, N);
    printf("phi(N) = %llu (0x%08X)\n", (unsigned long long)phi, (uint32_t)phi);
    printf("\ngcd(e, phi(N)) = %u\n", gcd(e, (uint32_t)phi));
    printf("Valid RSA parameters generated!\n");
    
    return 0;
}