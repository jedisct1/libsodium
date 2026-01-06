
#define TEST_NAME "xof_turboshake128"
#include "cmptest.h"

typedef struct {
    const unsigned char *msg;
    size_t               msg_len;
    const unsigned char *out;
    size_t               out_len;
} testvector;

static void
turboshake128_manual_with_domain(unsigned char *out, size_t outlen, const unsigned char *in,
                                 size_t inlen, unsigned char domain)
{
    crypto_core_keccak1600_state state;
    size_t                       rate      = crypto_xof_turboshake128_blockbytes();
    size_t                       offset    = 0;
    size_t                       consumed  = 0;
    size_t                       chunk_size;
    size_t                       extracted = 0;
    unsigned char                pad;

    crypto_core_keccak1600_init(&state);

    while (consumed < inlen) {
        if (offset == rate) {
            crypto_core_keccak1600_permute_12(&state);
            offset = 0;
        }
        chunk_size = rate - offset;
        if (chunk_size > inlen - consumed) {
            chunk_size = inlen - consumed;
        }
        crypto_core_keccak1600_xor_bytes(&state, &in[consumed], offset, chunk_size);
        offset += chunk_size;
        consumed += chunk_size;
    }

    if (offset == rate) {
        crypto_core_keccak1600_permute_12(&state);
        offset = 0;
    }

    if (offset == rate - 1) {
        pad = (unsigned char) (domain ^ 0x80);
        crypto_core_keccak1600_xor_bytes(&state, &pad, offset, 1);
    } else {
        crypto_core_keccak1600_xor_bytes(&state, &domain, offset, 1);
        pad = 0x80;
        crypto_core_keccak1600_xor_bytes(&state, &pad, rate - 1, 1);
    }

    crypto_core_keccak1600_permute_12(&state);
    offset = 0;

    while (extracted < outlen) {
        if (offset == rate) {
            crypto_core_keccak1600_permute_12(&state);
            offset = 0;
        }
        chunk_size = rate - offset;
        if (chunk_size > outlen - extracted) {
            chunk_size = outlen - extracted;
        }
        crypto_core_keccak1600_extract_bytes(&state, &out[extracted], offset, chunk_size);
        offset += chunk_size;
        extracted += chunk_size;
    }
}

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
    static const unsigned char msg_rate_block[168] = { 0 };

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

    static const unsigned char out_rate_block_32[] = { 0xdb, 0xa6, 0xe2, 0x67, 0xbd, 0xd5, 0x67,
                                                       0xdb, 0x0a, 0xd2, 0x63, 0x6e, 0x61, 0xf1,
                                                       0xae, 0x58, 0x9a, 0x81, 0xc1, 0xa9, 0xc1,
                                                       0x1f, 0x7f, 0x76, 0x93, 0x0a, 0x35, 0xea,
                                                       0x42, 0x47, 0x56, 0xd0 };

    testvector vectors[] = { { msg_empty, 0, out_empty_32, 32 },
                             { msg_empty, 0, out_empty_64, 64 },
                             { msg_abc, 3, out_abc_32, 32 },
                             { msg_fox, 43, out_fox_32, 32 },
                             { msg_fox, 43, out_fox_64, 64 },
                             { msg_rate_block, sizeof msg_rate_block, out_rate_block_32, 32 } };

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

    /* Test domain byte with MSB set when padding overlaps */
    {
        unsigned char       msg[crypto_xof_turboshake128_BLOCKBYTES - 1];
        unsigned char       out_manual[32];
        unsigned char       out_impl[32];
        const unsigned char domain = 0x99;

        memset(msg, 0xAA, sizeof msg);

        turboshake128_manual_with_domain(out_manual, sizeof out_manual, msg, sizeof msg, domain);

        crypto_xof_turboshake128_init_with_domain(&state, domain);
        crypto_xof_turboshake128_update(&state, msg, sizeof msg);
        crypto_xof_turboshake128_squeeze(&state, out_impl, sizeof out_impl);

        if (memcmp(out_manual, out_impl, sizeof out_manual) != 0) {
            printf("Domain MSB padding test failed\n");
            return 1;
        }
    }

    /* Test zero-length update and squeeze are no-ops */
    crypto_xof_turboshake128_init(&state);
    crypto_xof_turboshake128_update(&state, msg_abc, 1);
    crypto_xof_turboshake128_update(&state, msg_abc, 0);
    crypto_xof_turboshake128_update(&state, msg_abc + 1, 2);
    crypto_xof_turboshake128_squeeze(&state, out, 0);
    crypto_xof_turboshake128_squeeze(&state, out, 32);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Zero-length update/squeeze test failed\n");
        return 1;
    }

    /* Test rate-sized input and long output (padding after full block) */
    {
        unsigned char out_manual[crypto_xof_turboshake128_BLOCKBYTES + 7];
        unsigned char out_impl[crypto_xof_turboshake128_BLOCKBYTES + 7];

        turboshake128_manual_with_domain(out_manual, sizeof out_manual, msg_rate_block,
                                         sizeof msg_rate_block,
                                         crypto_xof_turboshake128_domain_standard());

        crypto_xof_turboshake128_init(&state);
        crypto_xof_turboshake128_update(&state, msg_rate_block, sizeof msg_rate_block);
        crypto_xof_turboshake128_squeeze(&state, out_impl, sizeof out_impl);

        if (memcmp(out_manual, out_impl, sizeof out_manual) != 0) {
            printf("Rate block long output test failed\n");
            return 1;
        }
    }

    /* Test chunked update across block boundary */
    {
        unsigned char msg_rate_plus1[crypto_xof_turboshake128_BLOCKBYTES + 1];
        unsigned char out_manual[32];
        unsigned char out_impl[32];

        for (i = 0; i < sizeof msg_rate_plus1; i++) {
            msg_rate_plus1[i] = (unsigned char) i;
        }

        turboshake128_manual_with_domain(out_manual, sizeof out_manual, msg_rate_plus1,
                                         sizeof msg_rate_plus1,
                                         crypto_xof_turboshake128_domain_standard());

        crypto_xof_turboshake128_init(&state);
        crypto_xof_turboshake128_update(&state, msg_rate_plus1, 1);
        crypto_xof_turboshake128_update(&state, msg_rate_plus1 + 1,
                                        crypto_xof_turboshake128_BLOCKBYTES - 1);
        crypto_xof_turboshake128_update(&state,
                                        msg_rate_plus1 + crypto_xof_turboshake128_BLOCKBYTES, 1);
        crypto_xof_turboshake128_squeeze(&state, out_impl, sizeof out_impl);

        if (memcmp(out_manual, out_impl, sizeof out_manual) != 0) {
            printf("Chunked update boundary test failed\n");
            return 1;
        }
    }

    printf("All TurboSHAKE-128 tests passed\n");
    return 0;
}
