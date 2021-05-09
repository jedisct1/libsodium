
#define TEST_NAME "scalarmult_noclamp"
#include "cmptest.h"

static const unsigned char alicesk[crypto_scalarmult_BYTES] = {
    0xb0, 0xfe, 0x2d, 0x1d, 0x33, 0x65, 0x74, 0x3d, 0x5b, 0x8f, 0x3e,
    0x3f, 0x8e, 0x8d, 0x08, 0x4f, 0x5b, 0xac, 0x03, 0x26, 0x2e, 0x1a,
    0xe3, 0x1b, 0x1c, 0xfe, 0xfd, 0xc0, 0x28, 0xfa, 0xbe, 0x57
};

static const unsigned char bobsk[crypto_scalarmult_BYTES] = {
    0x0b, 0xc1, 0x2f, 0x6c, 0x7b, 0xd3, 0x9d, 0xa5, 0x29, 0x01, 0xa6,
    0xb1, 0x9e, 0x6a, 0x1a, 0x05, 0x5b, 0x81, 0x38, 0xa7, 0x8d, 0x0f,
    0xa7, 0x84, 0xec, 0x89, 0x7b, 0x56, 0x0b, 0xe1, 0x7e, 0xd8
};

static char hex[crypto_scalarmult_BYTES * 2 + 1];

int
main(void)
{
    unsigned char *alicepk =
        (unsigned char *) sodium_malloc(crypto_scalarmult_BYTES);
    unsigned char *bobpk =
        (unsigned char *) sodium_malloc(crypto_scalarmult_BYTES);
    unsigned char *k = (unsigned char *) sodium_malloc(crypto_scalarmult_BYTES);
    int            ret;

    assert(alicepk != NULL && bobpk != NULL && k != NULL);

    crypto_scalarmult_base_noclamp(alicepk, alicesk);
    sodium_bin2hex(hex, sizeof hex, alicepk, crypto_scalarmult_BYTES);
    printf("%s\n", hex);

    crypto_scalarmult_base_noclamp(bobpk, bobsk);
    sodium_bin2hex(hex, sizeof hex, bobpk, crypto_scalarmult_BYTES);
    printf("%s\n", hex);

    ret = crypto_scalarmult_noclamp(k, alicesk, bobpk);
    assert(ret == 0);
    sodium_bin2hex(hex, sizeof hex, k, crypto_scalarmult_BYTES);
    printf("%s\n", hex);

    ret = crypto_scalarmult_noclamp(k, bobsk, alicepk);
    assert(ret == 0);
    sodium_bin2hex(hex, sizeof hex, k, crypto_scalarmult_BYTES);
    printf("%s\n", hex);

    alicepk[31] ^= 0x80;
    ret = crypto_scalarmult_noclamp(k, bobsk, alicepk);
    assert(ret == 0);
    sodium_bin2hex(hex, sizeof hex, k, crypto_scalarmult_BYTES);
    printf("%s\n", hex);

    sodium_free(bobpk);
    sodium_free(alicepk);
    sodium_free(k);

    assert(crypto_scalarmult_bytes() > 0U);
    assert(crypto_scalarmult_scalarbytes() > 0U);
    assert(strcmp(crypto_scalarmult_primitive(), "curve25519") == 0);
    assert(crypto_scalarmult_bytes() == crypto_scalarmult_curve25519_bytes());
    assert(crypto_scalarmult_scalarbytes() ==
           crypto_scalarmult_curve25519_scalarbytes());
    assert(crypto_scalarmult_bytes() == crypto_scalarmult_scalarbytes());

    return 0;
}
