
#define TEST_NAME "stream_serpent256"
#include "cmptest.h"

/* Fixed test key: 32 bytes. */
static const unsigned char key[crypto_stream_serpent256_KEYBYTES] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

/* Fixed nonce: 16 bytes. */
static const unsigned char nonce[crypto_stream_serpent256_NONCEBYTES] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
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

    /* 1. Generate 4096 bytes of keystream, SHA-256 hash it. */
    crypto_stream_serpent256(output, 4096, nonce, key);
    crypto_hash_sha256(h, output, 4096);
    sodium_bin2hex(hex, sizeof hex, h, sizeof h);
    printf("%s\n", hex);

    /* 2. XOR zero bytes must equal raw keystream. */
    memset(m, 0, 4096);
    crypto_stream_serpent256_xor(m, m, 4096, nonce, key);
    assert(memcmp(m, output, 4096) == 0);
    printf("xor_zero: ok\n");

    /* 3. Round-trip: xor(xor(m)) == m. */
    for (i = 0; i < 4096; i++) {
        m[i] = (unsigned char) i;
    }
    crypto_stream_serpent256_xor(output, m, 4096, nonce, key);
    crypto_stream_serpent256_xor(output, output, 4096, nonce, key);
    assert(memcmp(output, m, 4096) == 0);
    printf("xor_roundtrip: ok\n");

    /* 4. xor_ic: offset 1 must match bytes [16..] of full stream. */
    {
        unsigned char full[64], partial[48];

        crypto_stream_serpent256(full, 64, nonce, key);
        memset(partial, 0, 48);
        crypto_stream_serpent256_xor_ic(partial, partial, 48, nonce, 1, key);
        /* partial = keystream starting at block 1 = full[16..63] */
        assert(memcmp(partial, full + 16, 48) == 0);
        printf("xor_ic: ok\n");
    }

    /* 5. xor_ic128: (ic_lo=1, ic_hi=0) must match xor_ic(1). */
    {
        unsigned char a[48], b[48];

        memset(a, 0, 48);
        memset(b, 0, 48);
        crypto_stream_serpent256_xor_ic(a, a, 48, nonce, 1, key);
        crypto_stream_serpent256_xor_ic128(b, b, 48, nonce, 1, 0, key);
        assert(memcmp(a, b, 48) == 0);
        printf("xor_ic128: ok\n");
    }

    /* 6. Length sweep: generate 0..63 bytes, print hex of first 64. */
    for (i = 0; i < 64; i++) {
        memset(output, 0xdb, 64);
        crypto_stream_serpent256(output, (unsigned long long) i, nonce, key);
        sodium_bin2hex(hex, sizeof hex, output, 64);
        printf("%s\n", hex);
    }

    /* 7. Zero-length must not touch output. */
    memset(output, 0xaa, 64);
    crypto_stream_serpent256(output, 0, nonce, key);
    assert(output[0] == 0xaa);
    printf("zero_len: ok\n");

    /* 8. keygen produces non-zero key. */
    {
        unsigned char k2[crypto_stream_serpent256_KEYBYTES];
        int           all_zero;

        crypto_stream_serpent256_keygen(k2);
        all_zero = sodium_is_zero(k2, sizeof k2);
        assert(all_zero == 0);
    }
    printf("keygen: ok\n");

    /* 9. Constants. */
    assert(crypto_stream_serpent256_keybytes() == crypto_stream_serpent256_KEYBYTES);
    assert(crypto_stream_serpent256_noncebytes() == crypto_stream_serpent256_NONCEBYTES);
    assert(crypto_stream_serpent256_messagebytes_max() > 0U);
    assert(crypto_stream_serpent256_keybytes() == 32U);
    assert(crypto_stream_serpent256_noncebytes() == 16U);
    printf("constants: ok\n");

    sodium_free(m);
    sodium_free(output);

    return 0;
}
