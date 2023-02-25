
#include <stdint.h>
#include <stdlib.h>

#include "crypto_core_hforro14.h"
#include "private/common.h"

#define U32C(v) (v##U)
#define U32V(v) ((uint32_t)(v)&U32C(0xFFFFFFFF))

#define ROTATE(v, c) (ROTL32(v, c))
#define XOR(v, w) ((v) ^ (w))
#define PLUS(v, w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v), 1))

#define QUARTERROUND(a, b, c, d, e) \
    d = PLUS(d, e);                 \
    c = XOR(c, d);                  \
    b = ROTATE(PLUS(b, c), 10);     \
    a = PLUS(a, b);                 \
    e = XOR(e, a);                  \
    d = ROTATE(PLUS(d, e), 27);     \
    c = PLUS(c, d);                 \
    b = XOR(b, c);                  \
    a = ROTATE(PLUS(a, b), 8);

int crypto_core_hforro14(unsigned char *out, const unsigned char *in,
                         const unsigned char *k, const unsigned char *c)
{
    int i;
    uint32_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint32_t x8, x9, x10, x11, x12, x13, x14, x15;

    if (c == NULL)
    {
        x6 = 0x746C6F76;
        x7 = 0x61616461;
        x14 = 0x72626173;
        x15 = 0x61636E61;
    }
    else
    {
        x6 = LOAD32_LE(c + 0);
        x7 = LOAD32_LE(c + 4);
        x14 = LOAD32_LE(c + 8);
        x15 = LOAD32_LE(c + 12);
    }
    x0 = LOAD32_LE(k + 0);
    x1 = LOAD32_LE(k + 4);
    x2 = LOAD32_LE(k + 8);
    x3 = LOAD32_LE(k + 12);
    x8 = LOAD32_LE(k + 16);
    x9 = LOAD32_LE(k + 20);
    x10 = LOAD32_LE(k + 24);
    x11 = LOAD32_LE(k + 28);
    x12 = LOAD32_LE(in + 0);
    x13 = LOAD32_LE(in + 4);
    x14 = LOAD32_LE(in + 8);
    x15 = LOAD32_LE(in + 12);

    for (i = 0; i < 10; i++)
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
