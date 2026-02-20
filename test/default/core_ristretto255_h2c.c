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

#define H2CHASH crypto_core_ristretto255_H2CSHA512

int
main(void)
{
    unsigned char *p, *expected, *input;
    char          *p_hex, *expected_hex;
    char          *oversized_ctx;
    size_t         i;
    size_t         oversized_ctx_len = 500U;
    size_t         input_len;

    /*
     * DST = "HashToGroup-OPRFV1-" || mode_byte || "-ristretto255-SHA512"
     * The mode byte at offset 19 is patched per test vector.
     */
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

    printf("OK\n");

    return 0;
}
