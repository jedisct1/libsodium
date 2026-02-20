#define TEST_NAME "core_ristretto255_h2c"
#include "cmptest.h"

/*
 * Test vectors derived from RFC 9497 (OPRFs Using Prime-Order Groups)
 */

typedef struct TestData_ {
    unsigned char mode;
    const char   *input_hex;
    const char    expected_hex[65];
} TestData;

static TestData test_data[] = {
    /* OPRF mode 0, RFC 9497 A.1.1, Test Vector 1 */
    { 0x00, "00",
      "5873db2e5f8f4f544ce3e574c74c487f03bc64a2cf63b7c913908091aab03357" },
    /* OPRF mode 0, RFC 9497 A.1.1, Test Vector 2 */
    { 0x00, "5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a",
      "743d49d207339ae67aef8f4d0777744e5a604b94df5cbcc13e3dd87e79985a39" },
    /* VOPRF mode 1, RFC 9497 A.1.2, Test Vector 1 */
    { 0x01, "00",
      "868c9140811d0dc38291c7bbc0bd8f301d0d4e8b15f65e442184a233b8791703" },
    /* VOPRF mode 1, RFC 9497 A.1.2, Test Vector 2 */
    { 0x01, "5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a",
      "caff66fcdc41da4d87ccc72aaac70c6e267a4b55c3dc9489bb365a70a04f1a52" },
};

static void
test_scalar_from_string(void)
{
    static const struct {
        int         hash_alg;
        const char *msg;
        size_t      msg_len;
        const char *ctx;
        size_t      ctx_len;
        const char  expected[65];
    } scalar_tvs[] = {
        { 1, "", 0U, "test-dst", 8U,
          "28dae5fcb05a81163b41a90fc54e3e815613194ddd44b1e718bfab7457536705" },
        { 2, "", 0U, "test-dst", 8U,
          "110d25b6c0f2ab15d9387d4d77847e62c3b133da1bcc71e588076765c74e0b03" },
        { 1, "abc", 3U, "test-dst", 8U,
          "8a2383018254b8b57b624f049bc2e72c1c401ad0959b10d1d24ad863fcb5060c" },
        { 2, "abc", 3U, "test-dst", 8U,
          "cc954e2d86fdba2af74270642bd6375bdf13e485077bb56a648aaf243d4a8007" },
        { 2, "", 0U, "", 0U,
          "c9fb227ffae5fa378f3e6cbbafe054989748daae21c9ebe907f7b68d14093a06" },
    };
    unsigned char  s[crypto_core_ristretto255_SCALARBYTES];
    unsigned char  expected_s[crypto_core_ristretto255_SCALARBYTES];
    char           s_hex[crypto_core_ristretto255_SCALARBYTES * 2U + 1U];
    size_t         oversized_len = 256U;
    unsigned char *big_ctx;
    size_t         t;

    for (t = 0U; t < sizeof scalar_tvs / sizeof scalar_tvs[0]; t++) {
        sodium_hex2bin(expected_s, sizeof expected_s,
                       scalar_tvs[t].expected, (size_t) -1U,
                       NULL, NULL, NULL);
        if (crypto_core_ristretto255_scalar_from_string(
                s,
                (const unsigned char *) scalar_tvs[t].ctx,
                scalar_tvs[t].ctx_len,
                (const unsigned char *) scalar_tvs[t].msg,
                scalar_tvs[t].msg_len,
                scalar_tvs[t].hash_alg) != 0) {
            printf("scalar_from_string: call failed for tv %u\n",
                   (unsigned) t);
            continue;
        }
        if (memcmp(s, expected_s, sizeof expected_s) != 0) {
            sodium_bin2hex(s_hex, sizeof s_hex, s, sizeof s);
            printf("scalar_from_string: KAT %u mismatch: %s\n",
                   (unsigned) t, s_hex);
        }
        if (crypto_core_ristretto255_scalar_is_canonical(s) != 1) {
            printf("scalar_from_string: result %u not canonical\n",
                   (unsigned) t);
        }
    }

    if (crypto_core_ristretto255_scalar_from_string(
            s,
            (const unsigned char *) "test-dst", 8U,
            (const unsigned char *) "abc", 3U,
            0) != -1) {
        printf("scalar_from_string: invalid hash_alg 0 should fail\n");
    }
    if (crypto_core_ristretto255_scalar_from_string(
            s,
            (const unsigned char *) "test-dst", 8U,
            (const unsigned char *) "abc", 3U,
            99) != -1) {
        printf("scalar_from_string: invalid hash_alg 99 should fail\n");
    }

    big_ctx = (unsigned char *) sodium_malloc(oversized_len);
    memset(big_ctx, 'X', oversized_len);

    if (crypto_core_ristretto255_scalar_from_string(
            s, big_ctx, oversized_len,
            (const unsigned char *) "abc", 3U, 2) != 0) {
        printf("scalar_from_string: oversized DST SHA-512 failed\n");
    }
    sodium_bin2hex(s_hex, sizeof s_hex, s, sizeof s);
    printf("scalar_from_string oversized DST/sha512: %s\n", s_hex);

    if (crypto_core_ristretto255_scalar_from_string(
            s, big_ctx, oversized_len,
            (const unsigned char *) "abc", 3U, 1) != 0) {
        printf("scalar_from_string: oversized DST SHA-256 failed\n");
    }
    sodium_bin2hex(s_hex, sizeof s_hex, s, sizeof s);
    printf("scalar_from_string oversized DST/sha256: %s\n", s_hex);

    sodium_free(big_ctx);
}

#define H2CHASH crypto_core_ristretto255_H2CSHA512

static void
test_from_string_ro(void)
{
    unsigned char *p, *expected, *input;
    char          *p_hex, *expected_hex;
    char          *oversized_ctx;
    size_t         i;
    size_t         oversized_ctx_len = 500U;
    size_t         input_len;

    unsigned char dst[] = {
        'H','a','s','h','T','o','G','r','o','u','p','-',
        'O','P','R','F','V','1','-', 0x00, '-',
        'r','i','s','t','r','e','t','t','o','2','5','5','-',
        'S','H','A','5','1','2'
    };
    const size_t dst_len     = sizeof dst;
    const size_t mode_offset = 19U;

    p            = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);
    expected     = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);
    input        = (unsigned char *) sodium_malloc(256U);
    p_hex        = (char *) sodium_malloc(crypto_core_ristretto255_BYTES * 2U + 1U);
    expected_hex = (char *) sodium_malloc(crypto_core_ristretto255_BYTES * 2U + 1U);

    for (i = 0U; i < (sizeof test_data) / (sizeof test_data[0]); i++) {
        dst[mode_offset] = test_data[i].mode;

        input_len = strlen(test_data[i].input_hex) / 2U;
        sodium_hex2bin(input, 256U,
                       test_data[i].input_hex, strlen(test_data[i].input_hex),
                       NULL, NULL, NULL);
        sodium_hex2bin(expected, crypto_core_ristretto255_BYTES,
                       test_data[i].expected_hex, 64U, NULL, NULL, NULL);

        if (crypto_core_ristretto255_from_string_ro(
                p, dst, dst_len, input, input_len, H2CHASH) != 0) {
            printf("crypto_core_ristretto255_from_string_ro() failed (test #%u)\n",
                   (unsigned) i);
            continue;
        }
        if (memcmp(p, expected, crypto_core_ristretto255_BYTES) != 0) {
            sodium_bin2hex(expected_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                           expected, crypto_core_ristretto255_BYTES);
            sodium_bin2hex(p_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                           p, crypto_core_ristretto255_BYTES);
            printf("Test #%u failed - expected [%s] got [%s]\n",
                   (unsigned) i, expected_hex, p_hex);
        }
    }

    if (crypto_core_ristretto255_from_string_ro(
            p, NULL, 0U, (const unsigned char *) "msg", 3U, H2CHASH) != 0 ||
        crypto_core_ristretto255_from_string_ro(
            p, (const unsigned char *) "", 0U, guard_page, 0U, H2CHASH) != 0) {
        printf("Failed with empty parameters\n");
    }

    oversized_ctx = (char *) sodium_malloc(oversized_ctx_len);
    memset(oversized_ctx, 'X', oversized_ctx_len);
    crypto_core_ristretto255_from_string_ro(
        p, (const unsigned char *) oversized_ctx, oversized_ctx_len - 1U,
        (const unsigned char *) "msg", 3U, H2CHASH);
    sodium_bin2hex(p_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                   p, crypto_core_ristretto255_BYTES);
    printf("RO with oversized context: %s\n", p_hex);

    sodium_free(oversized_ctx);
    sodium_free(expected_hex);
    sodium_free(p_hex);
    sodium_free(input);
    sodium_free(expected);
    sodium_free(p);
}

int
main(void)
{
    test_scalar_from_string();
    test_from_string_ro();

    printf("OK\n");

    return 0;
}
