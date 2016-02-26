
#include <stdint.h>
#include <stdlib.h>

#include "core_hchacha20.h"
#include "crypto_core_hchacha20.h"

int
crypto_core_hchacha20(unsigned char *out, const unsigned char *in,
                      const unsigned char *k, const unsigned char *c)
{
    int      i;
    uint32_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint32_t x8, x9, x10, x11, x12, x13, x14, x15;

    if (c == NULL) {
        x0 = U32C(0x61707865);
        x1 = U32C(0x3320646e);
        x2 = U32C(0x79622d32);
        x3 = U32C(0x6b206574);
    } else {
        x0 = load32(c +  0);
        x1 = load32(c +  4);
        x2 = load32(c +  8);
        x3 = load32(c + 12);
    }
    x4  = load32(k +  0);
    x5  = load32(k +  4);
    x6  = load32(k +  8);
    x7  = load32(k + 12);
    x8  = load32(k + 16);
    x9  = load32(k + 20);
    x10 = load32(k + 24);
    x11 = load32(k + 28);
    x12 = load32(in +  0);
    x13 = load32(in +  4);
    x14 = load32(in +  8);
    x15 = load32(in + 12);

    for (i = 0; i < 10; i++) {
        QUARTERROUND(x0, x4,  x8, x12);
        QUARTERROUND(x1, x5,  x9, x13);
        QUARTERROUND(x2, x6, x10, x14);
        QUARTERROUND(x3, x7, x11, x15);
        QUARTERROUND(x0, x5, x10, x15);
        QUARTERROUND(x1, x6, x11, x12);
        QUARTERROUND(x2, x7,  x8, x13);
        QUARTERROUND(x3, x4,  x9, x14);
    }

    store32(out +  0, x0);
    store32(out +  4, x1);
    store32(out +  8, x2);
    store32(out + 12, x3);
    store32(out + 16, x12);
    store32(out + 20, x13);
    store32(out + 24, x14);
    store32(out + 28, x15);

    return 0;
}

size_t
crypto_core_hchacha20_outputbytes(void)
{
    return crypto_core_hchacha20_OUTPUTBYTES;
}

size_t
crypto_core_hchacha20_inputbytes(void)
{
    return crypto_core_hchacha20_INPUTBYTES;
}

size_t
crypto_core_hchacha20_keybytes(void)
{
    return crypto_core_hchacha20_KEYBYTES;
}

size_t
crypto_core_hchacha20_constbytes(void)
{
    return crypto_core_hchacha20_CONSTBYTES;
}
