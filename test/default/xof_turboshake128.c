
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
    /* Test vectors from TurboSHAKE reference implementation */
    static const unsigned char msg_empty[] = "";
    static const unsigned char msg_abc[]   = { 0x61, 0x62, 0x63 };
    static const unsigned char msg_fox[]   = { 0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63, 0x6b,
                                               0x20, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x20, 0x66, 0x6f,
                                               0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70, 0x73, 0x20, 0x6f,
                                               0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6c,
                                               0x61, 0x7a, 0x79, 0x20, 0x64, 0x6f, 0x67 };

    static const unsigned char out_empty_32[] = { 0x86, 0x8c, 0xbd, 0x53, 0xb0, 0x78, 0x20, 0x5a,
                                                  0xbb, 0x85, 0x81, 0x5d, 0x94, 0x1f, 0x7d, 0x03,
                                                  0x76, 0xbf, 0xf5, 0xb8, 0x88, 0x8a, 0x6a, 0x2d,
                                                  0x03, 0x48, 0x3a, 0xfb, 0xaf, 0x83, 0x96, 0x7f };

    static const unsigned char out_empty_64[] = {
        0x86, 0x8c, 0xbd, 0x53, 0xb0, 0x78, 0x20, 0x5a, 0xbb, 0x85, 0x81, 0x5d, 0x94,
        0x1f, 0x7d, 0x03, 0x76, 0xbf, 0xf5, 0xb8, 0x88, 0x8a, 0x6a, 0x2d, 0x03, 0x48,
        0x3a, 0xfb, 0xaf, 0x83, 0x96, 0x7f, 0x22, 0x6e, 0x2c, 0xad, 0x5e, 0x7b, 0x1e,
        0xc4, 0xca, 0x72, 0x23, 0x6f, 0x07, 0x64, 0x62, 0x19, 0x9f, 0xea, 0x48, 0xc9,
        0x34, 0x38, 0xad, 0x4c, 0x49, 0xc7, 0x67, 0xf9, 0x41, 0x7b, 0xe7, 0xc5
    };

    static const unsigned char out_abc_32[] = { 0x59, 0xcc, 0xfc, 0x22, 0xa3, 0xb8, 0x47, 0x42,
                                                0x58, 0x6b, 0x41, 0xf5, 0x1f, 0x8a, 0x94, 0x73,
                                                0x8d, 0xd0, 0x2b, 0xc7, 0x45, 0x51, 0xeb, 0x0e,
                                                0xf5, 0x0f, 0xc4, 0x09, 0x4e, 0xb0, 0xfc, 0x7b };

    static const unsigned char out_fox_32[] = { 0x6f, 0x16, 0x24, 0x47, 0xdd, 0xd3, 0x30, 0xc8,
                                                0xee, 0x8b, 0x21, 0x89, 0x88, 0xca, 0x1b, 0xef,
                                                0x35, 0xd9, 0x02, 0xa5, 0xfd, 0x6b, 0xaa, 0xf3,
                                                0x44, 0x20, 0x48, 0x32, 0x26, 0xa1, 0x72, 0x4a };

    static const unsigned char out_fox_64[] = {
        0x6f, 0x16, 0x24, 0x47, 0xdd, 0xd3, 0x30, 0xc8, 0xee, 0x8b, 0x21, 0x89, 0x88,
        0xca, 0x1b, 0xef, 0x35, 0xd9, 0x02, 0xa5, 0xfd, 0x6b, 0xaa, 0xf3, 0x44, 0x20,
        0x48, 0x32, 0x26, 0xa1, 0x72, 0x4a, 0xb9, 0x02, 0xc6, 0x85, 0x7e, 0xbd, 0x0b,
        0xa1, 0x76, 0x00, 0xdf, 0x9e, 0xe4, 0x9a, 0x06, 0x18, 0x03, 0x36, 0x92, 0x2d,
        0x9b, 0x61, 0x80, 0x7e, 0x8f, 0xc8, 0x03, 0xec, 0xb0, 0x1a, 0x81, 0xf9
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
    assert(crypto_xof_turboshake128_domain_standard() == 0x01);

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
        crypto_xof_turboshake128_final(&state, out, vectors[i].out_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (streaming)\n", i);
            return 1;
        }
    }

    /* Test multiple squeeze calls */
    crypto_xof_turboshake128_init(&state);
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_final(&state, out, 16);
    crypto_xof_turboshake128_squeeze(&state, out + 16, 16);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Multiple squeeze test failed\n");
        return 1;
    }

    /* Test custom domain byte produces different output */
    crypto_xof_turboshake128_init(&state);
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_final(&state, out, 32);

    crypto_xof_turboshake128_init_with_domain(&state, 0x99);
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_final(&state, out + 32, 32);

    if (memcmp(out, out + 32, 32) == 0) {
        printf("Custom domain byte test failed (outputs should differ)\n");
        return 1;
    }

    /* Test standard domain constant */
    crypto_xof_turboshake128_init_with_domain(&state, crypto_xof_turboshake128_domain_standard());
    crypto_xof_turboshake128_update(&state, msg_abc, 3);
    crypto_xof_turboshake128_final(&state, out + 64, 32);

    if (memcmp(out, out + 64, 32) != 0) {
        printf("Domain constant test failed (should match standard init)\n");
        return 1;
    }

    /* Test cannot absorb after squeezing */
    crypto_xof_turboshake128_init(&state);
    crypto_xof_turboshake128_final(&state, out, 32);
    if (crypto_xof_turboshake128_update(&state, msg_abc, 3) == 0) {
        printf("Should not be able to absorb after squeezing\n");
        return 1;
    }

    printf("All TurboSHAKE-128 tests passed\n");
    return 0;
}
