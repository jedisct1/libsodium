
#define TEST_NAME "xof_turboshake256"
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
    /* Test vectors from TurboSHAKE reference implementation */
    static const unsigned char msg_empty[] = "";
    static const unsigned char msg_abc[]   = { 0x61, 0x62, 0x63 };
    static const unsigned char msg_fox[]   = { 0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63, 0x6b,
                                               0x20, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x20, 0x66, 0x6f,
                                               0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70, 0x73, 0x20, 0x6f,
                                               0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6c,
                                               0x61, 0x7a, 0x79, 0x20, 0x64, 0x6f, 0x67 };

    static const unsigned char out_empty_32[] = { 0xe3, 0xdd, 0x2d, 0xf0, 0x94, 0x3b, 0xde, 0x6d,
                                                  0x82, 0xe3, 0x9e, 0xc3, 0x60, 0x59, 0xf3, 0x5c,
                                                  0xd7, 0x67, 0x20, 0xe2, 0xdf, 0x38, 0xcc, 0x6b,
                                                  0x10, 0xb6, 0x9f, 0xdd, 0xfc, 0xaa, 0x3a, 0x4a };

    static const unsigned char out_empty_64[] = {
        0xe3, 0xdd, 0x2d, 0xf0, 0x94, 0x3b, 0xde, 0x6d, 0x82, 0xe3, 0x9e, 0xc3, 0x60,
        0x59, 0xf3, 0x5c, 0xd7, 0x67, 0x20, 0xe2, 0xdf, 0x38, 0xcc, 0x6b, 0x10, 0xb6,
        0x9f, 0xdd, 0xfc, 0xaa, 0x3a, 0x4a, 0x72, 0xfb, 0xbb, 0xe4, 0x2c, 0x00, 0xce,
        0xd7, 0xaa, 0x88, 0xe2, 0x6d, 0x46, 0x75, 0xdd, 0x6e, 0x2c, 0x43, 0xc4, 0x41,
        0x3c, 0x4e, 0xa4, 0xd4, 0x4b, 0xb1, 0x70, 0xf0, 0x3a, 0x98, 0x1c, 0xab
    };

    static const unsigned char out_abc_32[] = { 0x88, 0xfb, 0x36, 0x9d, 0x5d, 0x85, 0x6b, 0x22,
                                                0xcc, 0xe4, 0xd6, 0xa2, 0x40, 0x56, 0x60, 0x0b,
                                                0xa7, 0x27, 0x44, 0xcd, 0xb3, 0x26, 0x37, 0x49,
                                                0x07, 0x91, 0xcc, 0xd9, 0x85, 0x3b, 0xc9, 0x14 };

    static const unsigned char out_fox_32[] = { 0xe9, 0x17, 0xad, 0xfe, 0x65, 0x54, 0x6d, 0x28,
                                                0xa1, 0x64, 0x57, 0x61, 0x07, 0xe5, 0xd4, 0x77,
                                                0x4d, 0x46, 0x64, 0x42, 0xc5, 0xe8, 0x86, 0xf1,
                                                0xb8, 0xc9, 0xee, 0xab, 0x5d, 0x3f, 0xbf, 0xa8 };

    static const unsigned char out_fox_64[] = {
        0xe9, 0x17, 0xad, 0xfe, 0x65, 0x54, 0x6d, 0x28, 0xa1, 0x64, 0x57, 0x61, 0x07,
        0xe5, 0xd4, 0x77, 0x4d, 0x46, 0x64, 0x42, 0xc5, 0xe8, 0x86, 0xf1, 0xb8, 0xc9,
        0xee, 0xab, 0x5d, 0x3f, 0xbf, 0xa8, 0x96, 0x53, 0xea, 0xd9, 0x8b, 0x2a, 0x52,
        0x05, 0x78, 0x38, 0x9b, 0x24, 0xeb, 0xab, 0x8f, 0xcf, 0xe9, 0x12, 0x3e, 0x71,
        0x42, 0x1a, 0x14, 0xfb, 0xe2, 0x4e, 0x13, 0xe6, 0x27, 0x31, 0x3a, 0xa1
    };

    testvector vectors[] = { { msg_empty, 0, out_empty_32, 32 },
                             { msg_empty, 0, out_empty_64, 64 },
                             { msg_abc, 3, out_abc_32, 32 },
                             { msg_fox, 43, out_fox_32, 32 },
                             { msg_fox, 43, out_fox_64, 64 } };

    unsigned char                  out[256];
    crypto_xof_turboshake256_state state;
    size_t                         i;

    /* Test constants */
    assert(crypto_xof_turboshake256_blockbytes() == 136);
    assert(crypto_xof_turboshake256_statebytes() == 256);
    assert(crypto_xof_turboshake256_domain_standard() == 0x01);

    /* Test one-shot API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_turboshake256(out, vectors[i].out_len, vectors[i].msg, vectors[i].msg_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (one-shot)\n", i);
            return 1;
        }
    }

    /* Test streaming API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_turboshake256_init(&state);
        crypto_xof_turboshake256_update(&state, vectors[i].msg, vectors[i].msg_len);
        crypto_xof_turboshake256_final(&state, out, vectors[i].out_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (streaming)\n", i);
            return 1;
        }
    }

    /* Test multiple squeeze calls */
    crypto_xof_turboshake256_init(&state);
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_final(&state, out, 16);
    crypto_xof_turboshake256_squeeze(&state, out + 16, 16);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Multiple squeeze test failed\n");
        return 1;
    }

    /* Test custom domain byte produces different output */
    crypto_xof_turboshake256_init(&state);
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_final(&state, out, 32);

    crypto_xof_turboshake256_init_with_domain(&state, 0x99);
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_final(&state, out + 32, 32);

    if (memcmp(out, out + 32, 32) == 0) {
        printf("Custom domain byte test failed (outputs should differ)\n");
        return 1;
    }

    /* Test standard domain constant */
    crypto_xof_turboshake256_init_with_domain(&state, crypto_xof_turboshake256_domain_standard());
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_final(&state, out + 64, 32);

    if (memcmp(out, out + 64, 32) != 0) {
        printf("Domain constant test failed (should match standard init)\n");
        return 1;
    }

    /* Test cannot absorb after squeezing */
    crypto_xof_turboshake256_init(&state);
    crypto_xof_turboshake256_final(&state, out, 32);
    if (crypto_xof_turboshake256_update(&state, msg_abc, 3) == 0) {
        printf("Should not be able to absorb after squeezing\n");
        return 1;
    }

    printf("All TurboSHAKE-256 tests passed\n");
    return 0;
}
