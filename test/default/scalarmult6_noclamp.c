
#define TEST_NAME "scalarmult6_noclamp"
#include "cmptest.h"

static unsigned char bobsk_[crypto_scalarmult_SCALARBYTES] = {
    0x35, 0x4a, 0x67, 0x27, 0x0b, 0x01, 0xff, 0x9e, 0x25, 0x9c, 0x23,
    0x50, 0x56, 0x95, 0x9e, 0x00, 0x8b, 0x14, 0xbe, 0x2d, 0x05, 0x60,
    0xdd, 0x88, 0xd6, 0x6f, 0xc6, 0x2d, 0x68, 0xf2, 0x7d, 0x35
};

static unsigned char alicepk_[crypto_scalarmult_SCALARBYTES] = {
    0x7a, 0x5c, 0x96, 0x10, 0x06, 0xd5, 0xa1, 0x7c, 0xa1, 0xe4, 0xaf,
    0x55, 0x99, 0x1a, 0x8d, 0x16, 0x33, 0x67, 0x56, 0xd6, 0x6b, 0x48,
    0x36, 0x80, 0xae, 0x6a, 0xa7, 0xb4, 0xd1, 0x3e, 0x16, 0x2c
};

int
main(void)
{
    unsigned char *k;
    unsigned char *bobsk;
    unsigned char *alicepk;
    int            i;
    int            ret;

    k       = (unsigned char *) sodium_malloc(crypto_scalarmult_BYTES);
    bobsk   = (unsigned char *) sodium_malloc(crypto_scalarmult_SCALARBYTES);
    alicepk = (unsigned char *) sodium_malloc(crypto_scalarmult_SCALARBYTES);
    assert(k != NULL && bobsk != NULL && alicepk != NULL);

    memcpy(bobsk, bobsk_, crypto_scalarmult_SCALARBYTES);
    memcpy(alicepk, alicepk_, crypto_scalarmult_SCALARBYTES);

    ret = crypto_scalarmult_noclamp(k, bobsk, alicepk);
    assert(ret == 0);

    sodium_free(alicepk);
    sodium_free(bobsk);

    for (i = 0; i < 32; ++i) {
        if (i > 0) {
            printf(",");
        } else {
            printf(" ");
        }
        printf("0x%02x", (unsigned int) k[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    sodium_free(k);

    return 0;
}
