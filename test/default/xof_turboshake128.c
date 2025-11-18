
#define TEST_NAME "xof_turboshake128"
#include "cmptest.h"

typedef struct {
    const unsigned char *msg;
    size_t               msg_len;
    const unsigned char *out;
    size_t               out_len;
} testvector;

int
main(void)
{
    /* Test vectors from Zig standard library (domain byte 0x1F) */
    static const unsigned char msg_empty[] = "";
    static const unsigned char msg_abc[]   = { 0x61, 0x62, 0x63 };
    static const unsigned char msg_fox[]   = { 0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63, 0x6b,
                                               0x20, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x20, 0x66, 0x6f,
                                               0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70, 0x73, 0x20, 0x6f,
                                               0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6c,
                                               0x61, 0x7a, 0x79, 0x20, 0x64, 0x6f, 0x67 };

    static const unsigned char out_empty_32[] = { 0x1e, 0x41, 0x5f, 0x1c, 0x59, 0x83, 0xaf, 0xf2,
                                                  0x16, 0x92, 0x17, 0x27, 0x7d, 0x17, 0xbb, 0x53,
                                                  0x8c, 0xd9, 0x45, 0xa3, 0x97, 0xdd, 0xec, 0x54,
                                                  0x1f, 0x1c, 0xe4, 0x1a, 0xf2, 0xc1, 0xb7, 0x4c };

    static const unsigned char out_empty_64[] = {
        0x1e, 0x41, 0x5f, 0x1c, 0x59, 0x83, 0xaf, 0xf2, 0x16, 0x92, 0x17, 0x27, 0x7d,
        0x17, 0xbb, 0x53, 0x8c, 0xd9, 0x45, 0xa3, 0x97, 0xdd, 0xec, 0x54, 0x1f, 0x1c,
        0xe4, 0x1a, 0xf2, 0xc1, 0xb7, 0x4c, 0x3e, 0x8c, 0xca, 0xe2, 0xa4, 0xda, 0xe5,
        0x6c, 0x84, 0xa0, 0x4c, 0x23, 0x85, 0xc0, 0x3c, 0x15, 0xe8, 0x19, 0x3b, 0xdf,
        0x58, 0x73, 0x73, 0x63, 0x32, 0x16, 0x91, 0xc0, 0x54, 0x62, 0xc8, 0xdf
    };

    static const unsigned char out_abc_32[] = { 0xdc, 0xf1, 0x64, 0x6d, 0xfe, 0x99, 0x3a, 0x8e,
                                                0xb6, 0xb7, 0x82, 0xd1, 0xfa, 0xac, 0xa6, 0xd8,
                                                0x24, 0x16, 0xa5, 0xdc, 0xf1, 0xde, 0x98, 0xee,
                                                0x3c, 0x6d, 0xbc, 0x5e, 0x1d, 0xc6, 0x30, 0x18 };

    static const unsigned char out_fox_32[] = { 0x76, 0xa1, 0x72, 0x0a, 0x48, 0x48, 0xab, 0x64,
                                                0xe6, 0x7e, 0x56, 0x3f, 0x16, 0xb8, 0xc5, 0xaa,
                                                0x49, 0x2b, 0x69, 0x8a, 0x4d, 0x93, 0x42, 0x97,
                                                0x35, 0xfd, 0x02, 0x35, 0x46, 0x57, 0xfb, 0xf7 };

    static const unsigned char out_fox_64[] = {
        0x76, 0xa1, 0x72, 0x0a, 0x48, 0x48, 0xab, 0x64, 0xe6, 0x7e, 0x56, 0x3f, 0x16,
        0xb8, 0xc5, 0xaa, 0x49, 0x2b, 0x69, 0x8a, 0x4d, 0x93, 0x42, 0x97, 0x35, 0xfd,
        0x02, 0x35, 0x46, 0x57, 0xfb, 0xf7, 0xa0, 0x68, 0x9e, 0xc7, 0x7b, 0x4c, 0x79,
        0x5f, 0xda, 0x9d, 0xaa, 0xb4, 0x10, 0xc6, 0x30, 0x92, 0xf5, 0x42, 0x00, 0x84,
        0x6c, 0x34, 0x12, 0x0f, 0xf2, 0xb2, 0x53, 0xe9, 0xfd, 0x8d, 0x9f, 0xc4
    };

    testvector vectors[] = { { msg_empty, 0, out_empty_32, 32 },
                             { msg_empty, 0, out_empty_64, 64 },
                             { msg_abc, 3, out_abc_32, 32 },
                             { msg_fox, 43, out_fox_32, 32 },
                             { msg_fox, 43, out_fox_64, 64 } };

    unsigned char                  out[256];
    crypto_xof_turboshake128_state state;
    size_t                         i;

    /* Test constants */
    assert(crypto_xof_turboshake128_blockbytes() == 168);
    assert(crypto_xof_turboshake128_statebytes() == 256);
    assert(crypto_xof_turboshake128_domain_standard() == 0x1F);

    /* Test one-shot API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_turboshake128(out, vectors[i].out_len, vectors[i].msg, vectors[i].msg_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (one-shot)\n", i);
            return 1;
        }
    }

    /* Test streaming API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_turboshake128_init(&state);
        crypto_xof_turboshake128_update(&state, vectors[i].msg, vectors[i].msg_len);
        crypto_xof_turboshake128_squeeze(&state, out, vectors[i].out_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (streaming)\n", i);
            return 1;
        }
    }

    /* Test multiple squeeze calls */
    crypto_xof_turboshake128_init(&state);
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_squeeze(&state, out, 16);
    crypto_xof_turboshake128_squeeze(&state, out + 16, 16);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Multiple squeeze test failed\n");
        return 1;
    }

    /* Test custom domain byte produces different output */
    crypto_xof_turboshake128_init(&state);
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_squeeze(&state, out, 32);

    crypto_xof_turboshake128_init_with_domain(&state, 0x99);
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_squeeze(&state, out + 32, 32);

    if (memcmp(out, out + 32, 32) == 0) {
        printf("Custom domain byte test failed (outputs should differ)\n");
        return 1;
    }

    /* Test standard domain constant */
    crypto_xof_turboshake128_init_with_domain(&state, crypto_xof_turboshake128_domain_standard());
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_squeeze(&state, out + 64, 32);

    if (memcmp(out, out + 64, 32) != 0) {
        printf("Domain constant test failed (should match standard init)\n");
        return 1;
    }

    printf("All TurboSHAKE-128 tests passed\n");
    return 0;
}
