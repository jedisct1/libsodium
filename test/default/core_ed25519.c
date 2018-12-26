#define TEST_NAME "core_ed25519"
#include "cmptest.h"

static const unsigned char non_canonical_p[32] = {
    0xf6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f
};
static const unsigned char non_canonical_invalid_p[32] = {
    0xf5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f
};
static const unsigned char max_canonical_p[32] = {
    0xe4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f
};

static void
add_P(unsigned char * const S)
{
    static const unsigned char P[32] = {
        0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f
    };

    sodium_add(S, P, sizeof P);
}

static void
add_l64(unsigned char * const S)
{
    static const unsigned char l[crypto_core_ed25519_NONREDUCEDSCALARBYTES] =
      { 0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
        0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    sodium_add(S, l, sizeof l);
}

int
main(void)
{
    unsigned char *h;
    unsigned char *p, *p2, *p3;
    unsigned char *sc;
    unsigned char *sc64;
    unsigned int   i, j;

    h = (unsigned char *) sodium_malloc(crypto_core_ed25519_UNIFORMBYTES);
    p = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    for (i = 0; i < 1000; i++) {
        randombytes_buf(h, crypto_core_ed25519_UNIFORMBYTES);
        if (crypto_core_ed25519_from_uniform(p, h) != 0) {
            printf("crypto_core_ed25519_from_uniform() failed\n");
        }
        if (crypto_core_ed25519_is_valid_point(p) == 0) {
            printf("crypto_core_ed25519_from_uniform() returned an invalid point\n");
        }
    }

    p2 = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    p3 = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    randombytes_buf(h, crypto_core_ed25519_UNIFORMBYTES);
    crypto_core_ed25519_from_uniform(p2, h);

    j = 1 + (unsigned int) randombytes_uniform(100);
    memcpy(p3, p, crypto_core_ed25519_BYTES);
    for (i = 0; i < j; i++) {
        crypto_core_ed25519_add(p, p, p2);
        if (crypto_core_ed25519_is_valid_point(p) != 1) {
            printf("crypto_core_add() returned an invalid point\n");
        }
    }
    if (memcmp(p, p3, crypto_core_ed25519_BYTES) == 0) {
        printf("crypto_core_add() failed\n");
    }
    for (i = 0; i < j; i++) {
        crypto_core_ed25519_sub(p, p, p2);
    }
    if (memcmp(p, p3, crypto_core_ed25519_BYTES) != 0) {
        printf("crypto_core_add() or crypto_core_sub() failed\n");
    }
    sc = (unsigned char *) sodium_malloc(crypto_scalarmult_ed25519_SCALARBYTES);
    memset(sc, 0, crypto_scalarmult_ed25519_SCALARBYTES);
    sc[0] = 8;
    memcpy(p2, p, crypto_core_ed25519_BYTES);
    memcpy(p3, p, crypto_core_ed25519_BYTES);

    for (i = 0; i < 254; i++) {
        crypto_core_ed25519_add(p2, p2, p2);
    }
    for (i = 0; i < 8; i++) {
        crypto_core_ed25519_add(p2, p2, p);
    }
    if (crypto_scalarmult_ed25519(p3, sc, p) != 0) {
        printf("crypto_scalarmult_ed25519() failed\n");
    }
    if (memcmp(p2, p3, crypto_core_ed25519_BYTES) != 0) {
        printf("crypto_scalarmult_ed25519() is inconsistent with crypto_core_ed25519_add()\n");
    }

    assert(crypto_core_ed25519_is_valid_point(p) == 1);

    memset(p, 0, crypto_core_ed25519_BYTES);
    assert(crypto_core_ed25519_is_valid_point(p) == 0);

    p[0] = 1;
    assert(crypto_core_ed25519_is_valid_point(p) == 0);

    p[0] = 2;
    assert(crypto_core_ed25519_is_valid_point(p) == 0);

    p[0] = 9;
    assert(crypto_core_ed25519_is_valid_point(p) == 1);

    assert(crypto_core_ed25519_is_valid_point(max_canonical_p) == 1);
    assert(crypto_core_ed25519_is_valid_point(non_canonical_invalid_p) == 0);
    assert(crypto_core_ed25519_is_valid_point(non_canonical_p) == 0);

    memcpy(p2, p, crypto_core_ed25519_BYTES);
    add_P(p2);
    crypto_core_ed25519_add(p3, p2, p2);
    crypto_core_ed25519_sub(p3, p3, p2);
    assert(memcmp(p2, p, crypto_core_ed25519_BYTES) != 0);
    assert(memcmp(p3, p, crypto_core_ed25519_BYTES) == 0);

    p[0] = 2;
    assert(crypto_core_ed25519_add(p3, p2, p) == -1);
    assert(crypto_core_ed25519_add(p3, p2, non_canonical_p) == 0);
    assert(crypto_core_ed25519_add(p3, p2, non_canonical_invalid_p) == -1);
    assert(crypto_core_ed25519_add(p3, p, p3) == -1);
    assert(crypto_core_ed25519_add(p3, non_canonical_p, p3) == 0);
    assert(crypto_core_ed25519_add(p3, non_canonical_invalid_p, p3) == -1);

    assert(crypto_core_ed25519_sub(p3, p2, p) == -1);
    assert(crypto_core_ed25519_sub(p3, p2, non_canonical_p) == 0);
    assert(crypto_core_ed25519_sub(p3, p2, non_canonical_invalid_p) == -1);
    assert(crypto_core_ed25519_sub(p3, p, p3) == -1);
    assert(crypto_core_ed25519_sub(p3, non_canonical_p, p3) == 0);
    assert(crypto_core_ed25519_sub(p3, non_canonical_invalid_p, p3) == -1);

    for (i = 0; i < 1000; i++) {
        randombytes_buf(h, crypto_core_ed25519_UNIFORMBYTES);
        crypto_core_ed25519_from_uniform(p, h);
        crypto_core_ed25519_scalar_random(sc);
        if (crypto_scalarmult_ed25519_noclamp(p2, sc, p) != 0) {
            printf("crypto_scalarmult_ed25519_noclamp() failed\n");
        }
        assert(crypto_core_ed25519_is_valid_point(p2));
        if (crypto_core_ed25519_scalar_invert(sc, sc) != 0) {
            printf("crypto_core_ed25519_scalar_invert() failed\n");
        }
        if (crypto_scalarmult_ed25519_noclamp(p3, sc, p2) != 0) {
            printf("crypto_scalarmult_ed25519_noclamp() failed\n");
        }
        assert(memcmp(p3, p, crypto_core_ed25519_BYTES) == 0);
    }

    sc64 = (unsigned char *) sodium_malloc(64);
    crypto_core_ed25519_scalar_random(sc);
    memcpy(sc64, sc, crypto_core_ed25519_BYTES);
    memset(sc64 + crypto_core_ed25519_BYTES, 0,
           64 - crypto_core_ed25519_BYTES);
    i = (unsigned int) randombytes_uniform(100);
    do {
        add_l64(sc64);
    } while (i-- > 0);
    crypto_core_ed25519_scalar_reduce(sc64, sc64);
    if (memcmp(sc64, sc, crypto_core_ed25519_BYTES) != 0) {
        printf("crypto_core_ed25519_scalar_reduce() failed\n");
    }

    sodium_free(sc);
    sodium_free(p3);
    sodium_free(p2);
    sodium_free(p);
    sodium_free(h);

    assert(crypto_core_ed25519_BYTES == crypto_core_ed25519_bytes());
    assert(crypto_core_ed25519_SCALARBYTES == crypto_core_ed25519_scalarbytes());
    assert(crypto_core_ed25519_NONREDUCEDSCALARBYTES == crypto_core_ed25519_nonreducedscalarbytes());
    assert(crypto_core_ed25519_NONREDUCEDSCALARBYTES >= crypto_core_ed25519_SCALARBYTES);
    assert(crypto_core_ed25519_UNIFORMBYTES == crypto_core_ed25519_uniformbytes());
    assert(crypto_core_ed25519_UNIFORMBYTES >= crypto_core_ed25519_BYTES);

    printf("OK\n");

    return 0;
}
