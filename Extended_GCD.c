#include <stdio.h>
#include <stdint.h>

// Function to perform Extended Euclidean Algorithm
int extended_gcd(int a, int b, int *x, int *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a; // gcd is a
    }
    int x1, y1;
    int gcd = extended_gcd(b, a % b, &x1, &y1);

    *x = y1;
    *y = x1 - (a / b) * y1;

    return gcd; // return gcd
}

// Function to find modular inverse
int mod_inv(int a, int m) {
    int x, y;
    int gcd = extended_gcd(a, m, &x, &y);
    if (gcd != 1) {
        return -1; // Modular inverse doesn't exist
    } else {
        return (x % m + m) % m; // Ensure positive result
    }
}

// Function to find e and d for RSA
void find_e_d(uint32_t phi, uint32_t *e, uint32_t *d) {
    // Common choice for e is 65537 (0x10001)
    *e = 0x00000101;

    // Calculate d
    *d = mod_inv(*e, phi);
    if (*d == -1) {
        printf("Failed to find modular inverse!\n");
    }
}
