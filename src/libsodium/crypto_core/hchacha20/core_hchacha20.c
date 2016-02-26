
#include <stdint.h>
#include <stdlib.h>

#include "core_hchacha20.h"
#include "crypto_core_hchacha20.h"

static const unsigned char sigma[16] = {
    'e', 'x', 'p', 'a', 'n', 'd', ' ', '3', '2', '-', 'b', 'y', 't', 'e', ' ', 'k'
};

int
crypto_core_hchacha20(unsigned char *out, const unsigned char *in,
                      const unsigned char *k, const unsigned char *c)
{
    int      i;
    uint32_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint32_t x8, x9, x10, x11, x12, x13, x14, x15;

    if (c == NULL) {
        c = sigma;
    }
    x0  = U8TO32_LITTLE(c +  0);
    x1  = U8TO32_LITTLE(c +  4);
    x2  = U8TO32_LITTLE(c +  8);
    x3  = U8TO32_LITTLE(c + 12);
    x4  = U8TO32_LITTLE(k +  0);
    x5  = U8TO32_LITTLE(k +  4);
    x6  = U8TO32_LITTLE(k +  8);
    x7  = U8TO32_LITTLE(k + 12);
    x8  = U8TO32_LITTLE(k + 16);
    x9  = U8TO32_LITTLE(k + 20);
    x10 = U8TO32_LITTLE(k + 24);
    x11 = U8TO32_LITTLE(k + 28);
    x12 = U8TO32_LITTLE(in +  0);
    x13 = U8TO32_LITTLE(in +  4);
    x14 = U8TO32_LITTLE(in +  8);
    x15 = U8TO32_LITTLE(in + 12);

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

    U32TO8_LITTLE(out +  0, x0);
    U32TO8_LITTLE(out +  4, x1);
    U32TO8_LITTLE(out +  8, x2);
    U32TO8_LITTLE(out + 12, x3);
    U32TO8_LITTLE(out + 16, x12);
    U32TO8_LITTLE(out + 20, x13);
    U32TO8_LITTLE(out + 24, x14);
    U32TO8_LITTLE(out + 28, x15);

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
