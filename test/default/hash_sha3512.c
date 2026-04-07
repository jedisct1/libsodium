
#define TEST_NAME "hash_sha3512"
#include "cmptest.h"

int
main(void)
{
    static const unsigned char msg_abc[] = { 0x61, 0x62, 0x63 };

    static const unsigned char msg_fox[] = {
        0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63, 0x6b, 0x20, 0x62, 0x72,
        0x6f, 0x77, 0x6e, 0x20, 0x66, 0x6f, 0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70,
        0x73, 0x20, 0x6f, 0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6c,
        0x61, 0x7a, 0x79, 0x20, 0x64, 0x6f, 0x67
    };

    static const unsigned char out_empty[64] = {
        0xa6, 0x9f, 0x73, 0xcc, 0xa2, 0x3a, 0x9a, 0xc5, 0xc8, 0xb5, 0x67, 0xdc,
        0x18, 0x5a, 0x75, 0x6e, 0x97, 0xc9, 0x82, 0x16, 0x4f, 0xe2, 0x58, 0x59,
        0xe0, 0xd1, 0xdc, 0xc1, 0x47, 0x5c, 0x80, 0xa6, 0x15, 0xb2, 0x12, 0x3a,
        0xf1, 0xf5, 0xf9, 0x4c, 0x11, 0xe3, 0xe9, 0x40, 0x2c, 0x3a, 0xc5, 0x58,
        0xf5, 0x00, 0x19, 0x9d, 0x95, 0xb6, 0xd3, 0xe3, 0x01, 0x75, 0x85, 0x86,
        0x28, 0x1d, 0xcd, 0x26
    };

    static const unsigned char out_abc[64] = {
        0xb7, 0x51, 0x85, 0x0b, 0x1a, 0x57, 0x16, 0x8a, 0x56, 0x93, 0xcd, 0x92,
        0x4b, 0x6b, 0x09, 0x6e, 0x08, 0xf6, 0x21, 0x82, 0x74, 0x44, 0xf7, 0x0d,
        0x88, 0x4f, 0x5d, 0x02, 0x40, 0xd2, 0x71, 0x2e, 0x10, 0xe1, 0x16, 0xe9,
        0x19, 0x2a, 0xf3, 0xc9, 0x1a, 0x7e, 0xc5, 0x76, 0x47, 0xe3, 0x93, 0x40,
        0x57, 0x34, 0x0b, 0x4c, 0xf4, 0x08, 0xd5, 0xa5, 0x65, 0x92, 0xf8, 0x27,
        0x4e, 0xec, 0x53, 0xf0
    };

    static const unsigned char out_fox[64] = {
        0x01, 0xde, 0xdd, 0x5d, 0xe4, 0xef, 0x14, 0x64, 0x24, 0x45, 0xba, 0x5f,
        0x5b, 0x97, 0xc1, 0x5e, 0x47, 0xb9, 0xad, 0x93, 0x13, 0x26, 0xe4, 0xb0,
        0x72, 0x7c, 0xd9, 0x4c, 0xef, 0xc4, 0x4f, 0xff, 0x23, 0xf0, 0x7b, 0xf5,
        0x43, 0x13, 0x99, 0x39, 0xb4, 0x91, 0x28, 0xca, 0xf4, 0x36, 0xdc, 0x1b,
        0xde, 0xe5, 0x4f, 0xcb, 0x24, 0x02, 0x3a, 0x08, 0xd9, 0x40, 0x3f, 0x9b,
        0x4b, 0xf0, 0xd4, 0x50
    };

    unsigned char             out[64];
    crypto_hash_sha3512_state state;
    size_t                    i;

    assert(crypto_hash_sha3512_bytes() == 64);
    assert(crypto_hash_sha3512_statebytes() > 0);

    crypto_hash_sha3512(out, NULL, 0);
    assert(memcmp(out, out_empty, 64) == 0);
    printf("SHA3-512(\"\") = ");
    for (i = 0; i < 64; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");

    crypto_hash_sha3512(out, msg_abc, 3);
    assert(memcmp(out, out_abc, 64) == 0);
    printf("SHA3-512(\"abc\") = ");
    for (i = 0; i < 64; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");

    crypto_hash_sha3512(out, msg_fox, 43);
    assert(memcmp(out, out_fox, 64) == 0);
    printf("SHA3-512(\"The quick brown fox...\") = ");
    for (i = 0; i < 64; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");

    crypto_hash_sha3512_init(&state);
    crypto_hash_sha3512_update(&state, msg_abc, 3);
    crypto_hash_sha3512_final(&state, out);
    assert(memcmp(out, out_abc, 64) == 0);
    printf("Streaming API test passed\n");

    crypto_hash_sha3512_init(&state);
    crypto_hash_sha3512_update(&state, msg_abc, 1);
    crypto_hash_sha3512_update(&state, msg_abc + 1, 1);
    crypto_hash_sha3512_update(&state, msg_abc + 2, 1);
    crypto_hash_sha3512_final(&state, out);
    assert(memcmp(out, out_abc, 64) == 0);
    printf("Chunked update test passed\n");

    {
        unsigned char msg_rate[72];
        unsigned char out_rate[64];
        unsigned char out_rate_stream[64];

        memset(msg_rate, 0xAB, sizeof msg_rate);
        crypto_hash_sha3512(out_rate, msg_rate, sizeof msg_rate);

        crypto_hash_sha3512_init(&state);
        crypto_hash_sha3512_update(&state, msg_rate, sizeof msg_rate);
        crypto_hash_sha3512_final(&state, out_rate_stream);

        assert(memcmp(out_rate, out_rate_stream, 64) == 0);
        printf("Rate boundary test passed\n");
    }

    {
        unsigned char msg_rate_plus1[73];
        unsigned char out_rate_plus1[64];
        unsigned char out_rate_plus1_stream[64];

        memset(msg_rate_plus1, 0xCD, sizeof msg_rate_plus1);
        crypto_hash_sha3512(out_rate_plus1, msg_rate_plus1, sizeof msg_rate_plus1);

        crypto_hash_sha3512_init(&state);
        crypto_hash_sha3512_update(&state, msg_rate_plus1, 50);
        crypto_hash_sha3512_update(&state, msg_rate_plus1 + 50, 23);
        crypto_hash_sha3512_final(&state, out_rate_plus1_stream);

        assert(memcmp(out_rate_plus1, out_rate_plus1_stream, 64) == 0);
        printf("Rate+1 boundary test passed\n");
    }

    {
        unsigned char msg_rate_minus1[71];
        unsigned char out1[64], out2[64];

        memset(msg_rate_minus1, 0xEF, sizeof msg_rate_minus1);
        crypto_hash_sha3512(out1, msg_rate_minus1, sizeof msg_rate_minus1);

        crypto_hash_sha3512_init(&state);
        crypto_hash_sha3512_update(&state, msg_rate_minus1, sizeof msg_rate_minus1);
        crypto_hash_sha3512_final(&state, out2);

        assert(memcmp(out1, out2, 64) == 0);
        printf("Rate-1 boundary test passed\n");
    }

    printf("OK\n");

    return 0;
}
