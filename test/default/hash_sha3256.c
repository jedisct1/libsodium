
#define TEST_NAME "hash_sha3256"
#include "cmptest.h"

#define TEST_SHA3_256_BYTES 32U

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

    static const unsigned char out_empty[32] = {
        0xa7, 0xff, 0xc6, 0xf8, 0xbf, 0x1e, 0xd7, 0x66, 0x51, 0xc1, 0x47, 0x56,
        0xa0, 0x61, 0xd6, 0x62, 0xf5, 0x80, 0xff, 0x4d, 0xe4, 0x3b, 0x49, 0xfa,
        0x82, 0xd8, 0x0a, 0x4b, 0x80, 0xf8, 0x43, 0x4a
    };

    static const unsigned char out_abc[32] = {
        0x3a, 0x98, 0x5d, 0xa7, 0x4f, 0xe2, 0x25, 0xb2, 0x04, 0x5c, 0x17, 0x2d,
        0x6b, 0xd3, 0x90, 0xbd, 0x85, 0x5f, 0x08, 0x6e, 0x3e, 0x9d, 0x52, 0x5b,
        0x46, 0xbf, 0xe2, 0x45, 0x11, 0x43, 0x15, 0x32
    };

    static const unsigned char out_fox[32] = {
        0x69, 0x07, 0x0d, 0xda, 0x01, 0x97, 0x5c, 0x8c, 0x12, 0x0c, 0x3a, 0xad,
        0xa1, 0xb2, 0x82, 0x39, 0x4e, 0x7f, 0x03, 0x2f, 0xa9, 0xcf, 0x32, 0xf4,
        0xcb, 0x22, 0x59, 0xa0, 0x89, 0x7d, 0xfc, 0x04
    };

    unsigned char             out[32];
    crypto_hash_sha3256_state state;
    size_t                    i;

    assert(crypto_hash_sha3256_bytes() == 32);
    assert(crypto_hash_sha3256_statebytes() > 0);

    crypto_hash_sha3256(out, NULL, 0);
    assert(memcmp(out, out_empty, 32) == 0);
    printf("SHA3-256(\"\") = ");
    for (i = 0; i < 32; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");

    crypto_hash_sha3256(out, msg_abc, 3);
    assert(memcmp(out, out_abc, 32) == 0);
    printf("SHA3-256(\"abc\") = ");
    for (i = 0; i < 32; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");

    crypto_hash_sha3256(out, msg_fox, 43);
    assert(memcmp(out, out_fox, 32) == 0);
    printf("SHA3-256(\"The quick brown fox...\") = ");
    for (i = 0; i < 32; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");

    crypto_hash_sha3256_init(&state);
    crypto_hash_sha3256_update(&state, msg_abc, 3);
    crypto_hash_sha3256_final(&state, out);
    assert(memcmp(out, out_abc, 32) == 0);
    printf("Streaming API test passed\n");

    crypto_hash_sha3256_init(&state);
    crypto_hash_sha3256_update(&state, msg_abc, 1);
    crypto_hash_sha3256_update(&state, msg_abc + 1, 1);
    crypto_hash_sha3256_update(&state, msg_abc + 2, 1);
    crypto_hash_sha3256_final(&state, out);
    assert(memcmp(out, out_abc, 32) == 0);
    printf("Chunked update test passed\n");

    {
        unsigned char msg_rate[136];
        unsigned char out_rate[32];
        unsigned char out_rate_stream[32];

        memset(msg_rate, 0xAB, sizeof msg_rate);
        crypto_hash_sha3256(out_rate, msg_rate, sizeof msg_rate);

        crypto_hash_sha3256_init(&state);
        crypto_hash_sha3256_update(&state, msg_rate, sizeof msg_rate);
        crypto_hash_sha3256_final(&state, out_rate_stream);

        assert(memcmp(out_rate, out_rate_stream, 32) == 0);
        printf("Rate boundary test passed\n");
    }

    {
        unsigned char msg_rate_plus1[137];
        unsigned char out_rate_plus1[32];
        unsigned char out_rate_plus1_stream[32];

        memset(msg_rate_plus1, 0xCD, sizeof msg_rate_plus1);
        crypto_hash_sha3256(out_rate_plus1, msg_rate_plus1, sizeof msg_rate_plus1);

        crypto_hash_sha3256_init(&state);
        crypto_hash_sha3256_update(&state, msg_rate_plus1, 100);
        crypto_hash_sha3256_update(&state, msg_rate_plus1 + 100, 37);
        crypto_hash_sha3256_final(&state, out_rate_plus1_stream);

        assert(memcmp(out_rate_plus1, out_rate_plus1_stream, 32) == 0);
        printf("Rate+1 boundary test passed\n");
    }

    {
        unsigned char msg_rate_minus1[135];
        unsigned char out1[32], out2[32];

        memset(msg_rate_minus1, 0xEF, sizeof msg_rate_minus1);
        crypto_hash_sha3256(out1, msg_rate_minus1, sizeof msg_rate_minus1);

        crypto_hash_sha3256_init(&state);
        crypto_hash_sha3256_update(&state, msg_rate_minus1, sizeof msg_rate_minus1);
        crypto_hash_sha3256_final(&state, out2);

        assert(memcmp(out1, out2, 32) == 0);
        printf("Rate-1 boundary test passed\n");
    }

    {
        static const unsigned char resumed_msg[]      = { 0x78, 0x79 };
        static const unsigned char resumed_msg_other[] = { 0x78, 0x7a };
        unsigned char              resumed_out1[TEST_SHA3_256_BYTES];
        unsigned char              resumed_out2[TEST_SHA3_256_BYTES];
        unsigned char              resumed_out3[TEST_SHA3_256_BYTES];
        crypto_hash_sha3256_state  state2;
        crypto_hash_sha3256_state  state3;
        int                        ret;

        crypto_hash_sha3256_init(&state);
        crypto_hash_sha3256_init(&state2);
        crypto_hash_sha3256_init(&state3);
        crypto_hash_sha3256_update(&state, msg_abc, 3);
        crypto_hash_sha3256_update(&state2, msg_abc, 3);
        crypto_hash_sha3256_update(&state3, msg_abc, 3);
        crypto_hash_sha3256_final(&state, out);
        crypto_hash_sha3256_final(&state2, resumed_out2);
        crypto_hash_sha3256_final(&state3, resumed_out3);
        assert(memcmp(out, out_abc, 32) == 0);
        assert(memcmp(resumed_out2, out_abc, 32) == 0);
        assert(memcmp(resumed_out3, out_abc, 32) == 0);

        ret = crypto_hash_sha3256_update(&state, resumed_msg, sizeof resumed_msg);
        assert(ret == -1);
        ret = crypto_hash_sha3256_update(&state2, resumed_msg, sizeof resumed_msg);
        assert(ret == -1);
        ret = crypto_hash_sha3256_update(&state3, resumed_msg_other, sizeof resumed_msg_other);
        assert(ret == -1);

        ret = crypto_hash_sha3256_final(&state, resumed_out1);
        assert(ret == 0);
        ret = crypto_hash_sha3256_final(&state2, resumed_out2);
        assert(ret == 0);
        ret = crypto_hash_sha3256_final(&state3, resumed_out3);
        assert(ret == 0);

        assert(memcmp(resumed_out1, resumed_out2, TEST_SHA3_256_BYTES) == 0);
        assert(memcmp(resumed_out1, resumed_out3, TEST_SHA3_256_BYTES) != 0);
        printf("Update-after-final recovery test passed\n");
    }

    {
        unsigned char             repeated_out1[TEST_SHA3_256_BYTES];
        unsigned char             repeated_out2[TEST_SHA3_256_BYTES];
        crypto_hash_sha3256_state state2;
        int                       ret;

        memset(repeated_out1, 0xAA, sizeof repeated_out1);
        memset(repeated_out2, 0xAA, sizeof repeated_out2);

        crypto_hash_sha3256_init(&state);
        crypto_hash_sha3256_init(&state2);
        crypto_hash_sha3256_update(&state, msg_abc, 3);
        crypto_hash_sha3256_update(&state2, msg_abc, 3);
        ret = crypto_hash_sha3256_final(&state, out);
        assert(ret == 0);
        ret = crypto_hash_sha3256_final(&state2, out);
        assert(ret == 0);

        ret = crypto_hash_sha3256_final(&state, repeated_out1);
        assert(ret == -1);
        ret = crypto_hash_sha3256_final(&state2, repeated_out2);
        assert(ret == -1);

        assert(memcmp(repeated_out1, repeated_out2, TEST_SHA3_256_BYTES) == 0);
        assert(memcmp(repeated_out1, out, TEST_SHA3_256_BYTES) != 0);
        assert(repeated_out1[0] != 0xAA);
        printf("Final-after-final test passed\n");
    }

    printf("OK\n");

    return 0;
}
