
#define TEST_NAME "xof_turboshake256"
#include "cmptest.h"

typedef struct {
    const unsigned char *msg;
    size_t               msg_len;
    const unsigned char *out;
    size_t               out_len;
} testvector;

static void
turboshake256_manual_with_domain(unsigned char *out, size_t outlen, const unsigned char *in,
                                 size_t inlen, unsigned char domain)
{
    crypto_core_keccak1600_state state;
    size_t                       rate      = crypto_xof_turboshake256_blockbytes();
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
    static const unsigned char msg_rate_block[136] = { 0 };

    static const unsigned char out_empty_32[] = { 0x36, 0x7a, 0x32, 0x9d, 0xaf, 0xea, 0x87, 0x1c,
                                                  0x78, 0x02, 0xec, 0x67, 0xf9, 0x05, 0xae, 0x13,
                                                  0xc5, 0x76, 0x95, 0xdc, 0x2c, 0x66, 0x63, 0xc6,
                                                  0x10, 0x35, 0xf5, 0x9a, 0x18, 0xf8, 0xe7, 0xdb };

    static const unsigned char out_empty_64[] = {
        0x36, 0x7a, 0x32, 0x9d, 0xaf, 0xea, 0x87, 0x1c, 0x78, 0x02, 0xec, 0x67, 0xf9,
        0x05, 0xae, 0x13, 0xc5, 0x76, 0x95, 0xdc, 0x2c, 0x66, 0x63, 0xc6, 0x10, 0x35,
        0xf5, 0x9a, 0x18, 0xf8, 0xe7, 0xdb, 0x11, 0xed, 0xc0, 0xe1, 0x2e, 0x91, 0xea,
        0x60, 0xeb, 0x6b, 0x32, 0xdf, 0x06, 0xdd, 0x7f, 0x00, 0x2f, 0xba, 0xfa, 0xbb,
        0x6e, 0x13, 0xec, 0x1c, 0xc2, 0x0d, 0x99, 0x55, 0x47, 0x60, 0x0d, 0xb0
    };

    static const unsigned char out_abc_32[] = { 0x63, 0x82, 0x4b, 0x14, 0x31, 0xa7, 0x37, 0x2e,
                                                0x85, 0xed, 0xc0, 0x22, 0xc9, 0xd7, 0xaf, 0xdd,
                                                0x02, 0x74, 0x72, 0xfc, 0xfa, 0x33, 0xc8, 0x87,
                                                0xd6, 0xf5, 0xaa, 0xf8, 0xdc, 0x5d, 0x4d, 0xb6 };

    static const unsigned char out_fox_32[] = { 0xb6, 0xe9, 0x1a, 0x41, 0x2c, 0x26, 0x2c, 0x79,
                                                0x36, 0xb0, 0x69, 0xf6, 0x7b, 0xd2, 0x1c, 0x2f,
                                                0x8e, 0xcc, 0x48, 0xbd, 0xa8, 0xdc, 0x6e, 0xeb,
                                                0xfb, 0xaf, 0x6f, 0xca, 0xa8, 0x21, 0x91, 0xc3 };

    static const unsigned char out_fox_64[] = {
        0xb6, 0xe9, 0x1a, 0x41, 0x2c, 0x26, 0x2c, 0x79, 0x36, 0xb0, 0x69, 0xf6, 0x7b,
        0xd2, 0x1c, 0x2f, 0x8e, 0xcc, 0x48, 0xbd, 0xa8, 0xdc, 0x6e, 0xeb, 0xfb, 0xaf,
        0x6f, 0xca, 0xa8, 0x21, 0x91, 0xc3, 0x97, 0x44, 0x62, 0x70, 0x7a, 0xb2, 0xa5,
        0xc5, 0xd7, 0x04, 0xb0, 0xe8, 0x74, 0x86, 0x0a, 0x2a, 0x3f, 0xdd, 0xb5, 0x88,
        0xf5, 0x07, 0xc9, 0xb4, 0xf0, 0x41, 0x7e, 0x2b, 0x66, 0x31, 0x60, 0x90
    };

    static const unsigned char out_rate_block_32[] = { 0x91, 0xef, 0xfd, 0x08, 0xdd, 0x4c, 0xcc,
                                                       0xb6, 0x89, 0xc6, 0x26, 0xb4, 0x64, 0x93,
                                                       0x67, 0xad, 0x5a, 0x2e, 0xbf, 0xab, 0x61,
                                                       0x61, 0x17, 0x69, 0xa3, 0x74, 0x93, 0xfa,
                                                       0x70, 0x12, 0x28, 0xea };

    testvector vectors[] = { { msg_empty, 0, out_empty_32, 32 },
                             { msg_empty, 0, out_empty_64, 64 },
                             { msg_abc, 3, out_abc_32, 32 },
                             { msg_fox, 43, out_fox_32, 32 },
                             { msg_fox, 43, out_fox_64, 64 },
                             { msg_rate_block, sizeof msg_rate_block, out_rate_block_32, 32 } };

    unsigned char                  out[256];
    crypto_xof_turboshake256_state state;
    size_t                         i;

    /* Test constants */
    assert(crypto_xof_turboshake256_blockbytes() == 136);
    assert(crypto_xof_turboshake256_statebytes() == 256);
    assert(crypto_xof_turboshake256_domain_standard() == 0x1F);

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
        crypto_xof_turboshake256_squeeze(&state, out, vectors[i].out_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (streaming)\n", i);
            return 1;
        }
    }

    /* Test multiple squeeze calls */
    crypto_xof_turboshake256_init(&state);
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_squeeze(&state, out, 16);
    crypto_xof_turboshake256_squeeze(&state, out + 16, 16);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Multiple squeeze test failed\n");
        return 1;
    }

    /* Test custom domain byte produces different output */
    crypto_xof_turboshake256_init(&state);
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_squeeze(&state, out, 32);

    crypto_xof_turboshake256_init_with_domain(&state, 0x99);
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_squeeze(&state, out + 32, 32);

    if (memcmp(out, out + 32, 32) == 0) {
        printf("Custom domain byte test failed (outputs should differ)\n");
        return 1;
    }

    /* Test standard domain constant */
    crypto_xof_turboshake256_init_with_domain(&state, crypto_xof_turboshake256_domain_standard());
    crypto_xof_turboshake256_update(&state, msg_abc, 3);
    crypto_xof_turboshake256_squeeze(&state, out + 64, 32);

    if (memcmp(out, out + 64, 32) != 0) {
        printf("Domain constant test failed (should match standard init)\n");
        return 1;
    }

    /* Test domain byte with MSB set when padding overlaps */
    {
        unsigned char       msg[crypto_xof_turboshake256_BLOCKBYTES - 1];
        unsigned char       out_manual[32];
        unsigned char       out_impl[32];
        const unsigned char domain = 0x99;

        memset(msg, 0xAA, sizeof msg);

        turboshake256_manual_with_domain(out_manual, sizeof out_manual, msg, sizeof msg, domain);

        crypto_xof_turboshake256_init_with_domain(&state, domain);
        crypto_xof_turboshake256_update(&state, msg, sizeof msg);
        crypto_xof_turboshake256_squeeze(&state, out_impl, sizeof out_impl);

        if (memcmp(out_manual, out_impl, sizeof out_manual) != 0) {
            printf("Domain MSB padding test failed\n");
            return 1;
        }
    }

    /* Test zero-length update and squeeze are no-ops */
    crypto_xof_turboshake256_init(&state);
    crypto_xof_turboshake256_update(&state, msg_abc, 1);
    crypto_xof_turboshake256_update(&state, msg_abc, 0);
    crypto_xof_turboshake256_update(&state, msg_abc + 1, 2);
    crypto_xof_turboshake256_squeeze(&state, out, 0);
    crypto_xof_turboshake256_squeeze(&state, out, 32);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Zero-length update/squeeze test failed\n");
        return 1;
    }

    /* Test rate-sized input and long output (padding after full block) */
    {
        unsigned char out_manual[crypto_xof_turboshake256_BLOCKBYTES + 7];
        unsigned char out_impl[crypto_xof_turboshake256_BLOCKBYTES + 7];

        turboshake256_manual_with_domain(out_manual, sizeof out_manual, msg_rate_block,
                                         sizeof msg_rate_block,
                                         crypto_xof_turboshake256_domain_standard());

        crypto_xof_turboshake256_init(&state);
        crypto_xof_turboshake256_update(&state, msg_rate_block, sizeof msg_rate_block);
        crypto_xof_turboshake256_squeeze(&state, out_impl, sizeof out_impl);

        if (memcmp(out_manual, out_impl, sizeof out_manual) != 0) {
            printf("Rate block long output test failed\n");
            return 1;
        }
    }

    /* Test chunked update across block boundary */
    {
        unsigned char msg_rate_plus1[crypto_xof_turboshake256_BLOCKBYTES + 1];
        unsigned char out_manual[32];
        unsigned char out_impl[32];

        for (i = 0; i < sizeof msg_rate_plus1; i++) {
            msg_rate_plus1[i] = (unsigned char) i;
        }

        turboshake256_manual_with_domain(out_manual, sizeof out_manual, msg_rate_plus1,
                                         sizeof msg_rate_plus1,
                                         crypto_xof_turboshake256_domain_standard());

        crypto_xof_turboshake256_init(&state);
        crypto_xof_turboshake256_update(&state, msg_rate_plus1, 1);
        crypto_xof_turboshake256_update(&state, msg_rate_plus1 + 1,
                                        crypto_xof_turboshake256_BLOCKBYTES - 1);
        crypto_xof_turboshake256_update(&state,
                                        msg_rate_plus1 + crypto_xof_turboshake256_BLOCKBYTES, 1);
        crypto_xof_turboshake256_squeeze(&state, out_impl, sizeof out_impl);

        if (memcmp(out_manual, out_impl, sizeof out_manual) != 0) {
            printf("Chunked update boundary test failed\n");
            return 1;
        }
    }

    printf("All TurboSHAKE-256 tests passed\n");
    return 0;
}
