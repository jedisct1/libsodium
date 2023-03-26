
#include <stdint.h>
#include <stdlib.h>

#include "crypto_core_hforro14.h"
#include "private/common.h"

#define ROUNDS 7
#define ROTL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QUARTERROUND(a, b, c, d, e) (        \
    d += e, c ^= d, b += c, b = ROTL(b, 10), \
    a += b, e ^= a, d += e, d = ROTL(d, 27), \
    c += d, b ^= c, a += b, a = ROTL(a, 8))

int crypto_core_hforro14(unsigned char *out, const unsigned char *in,
                         const unsigned char *k)
{
    int i;
    uint32_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint32_t x8, x9, x10, x11, x12, x13, x14, x15;

    x0 = LOAD32_LE(k + 0);
    x1 = LOAD32_LE(k + 4);
    x2 = LOAD32_LE(k + 8);
    x3 = LOAD32_LE(k + 12);
    x4 = LOAD32_LE(in + 0);
    x5 = LOAD32_LE(in + 4);
    x6 = 0x746C6F76;
    x7 = 0x61616461;
    x8 = LOAD32_LE(k + 16);
    x9 = LOAD32_LE(k + 20);
    x10 = LOAD32_LE(k + 24);
    x11 = LOAD32_LE(k + 28);
    x12 = LOAD32_LE(in + 8);
    x13 = LOAD32_LE(in + 12);
    x14 = 0x72626173;
    x15 = 0x61636E61;

    for (i = 0; i < ROUNDS; i++)
    {
        QUARTERROUND(x0, x4, x8, x12, x3);
        QUARTERROUND(x1, x5, x9, x13, x0);
        QUARTERROUND(x2, x6, x10, x14, x1);
        QUARTERROUND(x3, x7, x11, x15, x2);
        QUARTERROUND(x0, x5, x10, x15, x3);
        QUARTERROUND(x1, x6, x11, x12, x0);
        QUARTERROUND(x2, x7, x8, x13, x1);
        QUARTERROUND(x3, x4, x9, x14, x2);
    }

    STORE32_LE(out + 0, x4);
    STORE32_LE(out + 4, x5);
    STORE32_LE(out + 8, x6);
    STORE32_LE(out + 12, x7);
    STORE32_LE(out + 16, x12);
    STORE32_LE(out + 20, x13);
    STORE32_LE(out + 24, x14);
    STORE32_LE(out + 28, x15);

    return 0;
}

size_t
crypto_core_hforro14_outputbytes(void)
{
    return crypto_core_hforro14_OUTPUTBYTES;
}

size_t
crypto_core_hforro14_inputbytes(void)
{
    return crypto_core_hforro14_INPUTBYTES;
}

size_t
crypto_core_hforro14_keybytes(void)
{
    return crypto_core_hforro14_KEYBYTES;
}

size_t
crypto_core_hforro14_constbytes(void)
{
    return crypto_core_hforro14_CONSTBYTES;
}
