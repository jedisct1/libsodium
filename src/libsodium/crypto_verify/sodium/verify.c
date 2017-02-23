
#include <stddef.h>
#include <stdint.h>

#include "crypto_verify_16.h"
#include "crypto_verify_32.h"
#include "crypto_verify_64.h"

int
crypto_verify_16(const unsigned char *x, const unsigned char *y)
{
    uint_fast16_t d = 0U;
    int           i;

    for (i = 0; i < 16; i++) {
        d |= x[i] ^ y[i];
    }
    return (1 & ((d - 1) >> 8)) - 1;
}

size_t
crypto_verify_16_bytes(void)
{
    return crypto_verify_16_BYTES;
}

int
crypto_verify_32(const unsigned char *x, const unsigned char *y)
{
    uint_fast16_t d = 0U;
    int           i;

    for (i = 0; i < 32; i++) {
        d |= x[i] ^ y[i];
    }
    return (1 & ((d - 1) >> 8)) - 1;
}

size_t
crypto_verify_32_bytes(void)
{
    return crypto_verify_32_BYTES;
}

int
crypto_verify_64(const unsigned char *x, const unsigned char *y)
{
    uint_fast16_t d = 0U;
    int           i;

    for (i = 0; i < 64; i++) {
        d |= x[i] ^ y[i];
    }
    return (1 & ((d - 1) >> 8)) - 1;
}

size_t
crypto_verify_64_bytes(void)
{
    return crypto_verify_64_BYTES;
}
