
#define TEST_NAME "stream5"
#include "cmptest.h"

static const unsigned char key[crypto_stream_salsa2012_KEYBYTES] = {
    0x1b, 0x27, 0x55, 0x64, 0x73, 0xe9, 0x85, 0xd4,
    0x62, 0xcd, 0x51, 0x19, 0x7a, 0x9a, 0x46, 0xc7,
    0x60, 0x09, 0x54, 0x9e, 0xac, 0x64, 0x74, 0xf2,
    0x06, 0xc4, 0xee, 0x08, 0x44, 0xf6, 0x83, 0x89
};

static const unsigned char nonce[crypto_stream_salsa2012_NONCEBYTES] = {
    0x69, 0x69, 0x6e, 0xe9, 0x55, 0xb6, 0x2b, 0x73
};

int
main(void)
{
    unsigned char  h[32];
    char           hex[2 * 64 + 1];
    unsigned char *output;
    unsigned char *m;
    int            i;

    output = (unsigned char *) sodium_malloc(4096);
    m = (unsigned char *) sodium_malloc(4096);

    crypto_stream_salsa2012(output, 4096, nonce, key);
    crypto_hash_sha256(h, output, 4096);
    sodium_bin2hex(hex, sizeof hex, h, sizeof h);
    printf("%s\n", hex);

    memset(m, 0, 4096);
    crypto_stream_salsa2012_xor(m, m, 4096, nonce, key);
    assert(memcmp(m, output, 4096) == 0);
    printf("xor_zero: ok\n");

    for (i = 0; i < 4096; i++) {
        m[i] = (unsigned char) i;
    }
    crypto_stream_salsa2012_xor(output, m, 4096, nonce, key);
    crypto_stream_salsa2012_xor(output, output, 4096, nonce, key);
    assert(memcmp(output, m, 4096) == 0);
    printf("xor_roundtrip: ok\n");

    for (i = 0; i < 64; i++) {
        memset(output, 0xdb, 64);
        crypto_stream_salsa2012(output, (unsigned long long) i, nonce, key);
        sodium_bin2hex(hex, sizeof hex, output, 64);
        printf("%s\n", hex);
    }

    memset(output, 0xaa, 64);
    crypto_stream_salsa2012(output, 0, nonce, key);
    assert(output[0] == 0xaa);
    printf("zero_len: ok\n");

    {
        unsigned char k2[crypto_stream_salsa2012_KEYBYTES];
        int           all_zero;

        crypto_stream_salsa2012_keygen(k2);
        all_zero = sodium_is_zero(k2, sizeof k2);
        assert(all_zero == 0);
    }
    printf("keygen: ok\n");

    assert(crypto_stream_salsa2012_keybytes() == crypto_stream_salsa2012_KEYBYTES);
    assert(crypto_stream_salsa2012_noncebytes() == crypto_stream_salsa2012_NONCEBYTES);
    assert(crypto_stream_salsa2012_messagebytes_max() > 0U);
    assert(crypto_stream_salsa2012_keybytes() == 32U);
    assert(crypto_stream_salsa2012_noncebytes() == 8U);
    printf("constants: ok\n");

    sodium_free(m);
    sodium_free(output);

    return 0;
}
