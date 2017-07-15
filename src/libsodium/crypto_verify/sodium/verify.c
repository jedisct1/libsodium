
#include <stddef.h>
#include <stdint.h>

#include "crypto_verify_16.h"
#include "crypto_verify_32.h"
#include "crypto_verify_64.h"

size_t
crypto_verify_16_bytes(void)
{
    return crypto_verify_16_BYTES;
}

size_t
crypto_verify_32_bytes(void)
{
    return crypto_verify_32_BYTES;
}

size_t
crypto_verify_64_bytes(void)
{
    return crypto_verify_64_BYTES;
}

static inline int
crypto_verify_n(const unsigned char *x_, const unsigned char *y_,
                const size_t n)
{
    const volatile unsigned char *volatile x =
        (const volatile unsigned char *volatile) x_;
    const volatile unsigned char *volatile y =
        (const volatile unsigned char *volatile) y_;
    volatile uint_fast16_t d = 0U;
    int i;

    for (i = 0; i < n; i++) {
        d |= x[i] ^ y[i];
    }
    return (1 & ((d - 1) >> 8)) - 1;
}

int
crypto_verify_16(const unsigned char *x, const unsigned char *y)
{
    return crypto_verify_n(x, y, crypto_verify_16_BYTES);
}

int
crypto_verify_32(const unsigned char *x, const unsigned char *y)
{
    return crypto_verify_n(x, y, crypto_verify_32_BYTES);
}

int
crypto_verify_64(const unsigned char *x, const unsigned char *y)
{
    return crypto_verify_n(x, y, crypto_verify_64_BYTES);
}
