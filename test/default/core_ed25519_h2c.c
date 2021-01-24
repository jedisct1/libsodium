#define TEST_NAME "core_ed25519_h2c"
#include "cmptest.h"

typedef struct TestData_ {
    int         ro;
    const char *msg;
    const char  tv_y[64 + 1];
} TestData;

static TestData test_data[] = {
    { 0, "",
      "222e314d04a4d5725e9f2aff9fb2a6b69ef375a1214eb19021ceab2d687f0f9b" },
    { 0, "abc",
      "67732d50f9a26f73111dd1ed5dba225614e538599db58ba30aaea1f5c827fa42" },
    { 0, "abcdef0123456789",
      "2f8a6c24dd1adde73909cada6a4a137577b0f179d336685c4a955a0a8e1a86fb" },
    { 0,
      "q128_"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",
      "2af6ff6ef5ebba128b0774f4296cb4c2279a074658b083b8dcca91f57a603450" },
    { 0,
      "a512_"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaa",
      "2c90c3d39eb18ff291d33441b35f3262cdd307162cc97c31bfcc7a4245891a37" },
    { 1, "",
      "09a6c8561a0b22bef63124c588ce4c62ea83a3c899763af26d795302e115dc21" },
    { 1, "abc",
      "1a8395b88338f22e435bbd301183e7f20a5f9de643f11882fb237f88268a5531" },
    { 1, "abcdef0123456789",
      "53060a3d140e7fbcda641ed3cf42c88a75411e648a1add71217f70ea8ec561a6" },
    { 1,
      "q128_"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",
      "2eca15e355fcfa39d2982f67ddb0eea138e2994f5956ed37b7f72eea5e89d2f7" },
    { 1,
      "a512_"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaa",
      "6dc2fc04f266c5c27f236a80b14f92ccd051ef1ff027f26a07f8c0f327d8f995" }
};

#define H2CHASH crypto_core_ed25519_H2CSHA512

int
main(void)
{
    unsigned char *expected_yr, *expected_y, *y;
    char *         expected_y_hex, *y_hex;
    char *         oversized_ctx;
    size_t         i, j;
    size_t         oversized_ctx_len = 500U;

    expected_yr = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    expected_y  = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    y           = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    expected_y_hex =
        (char *) sodium_malloc(crypto_core_ed25519_BYTES * 2U + 1U);
    y_hex = (char *) sodium_malloc(crypto_core_ed25519_BYTES * 2U + 1U);
    for (i = 0U; i < (sizeof test_data) / (sizeof test_data[0]); i++) {
        sodium_hex2bin(expected_yr, crypto_core_ed25519_BYTES,
                       test_data[i].tv_y, (size_t) -1U, NULL, NULL, NULL);
        for (j = 0; j < crypto_core_ed25519_BYTES; j++) {
            expected_y[j] = expected_yr[crypto_core_ed25519_BYTES - 1U - j];
        }
        if (test_data[i].ro == 0) {
            if (crypto_core_ed25519_from_string(
                    y, "QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_",
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg), H2CHASH) != 0) {
                printf("crypto_core_ed25519_from_string() failed\n");
            }
        } else {
            if (crypto_core_ed25519_from_string_ro(
                    y, "QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_",
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg), H2CHASH) != 0) {
                printf("crypto_core_ed25519_from_string_ro() failed\n");
            }
        }
        y[crypto_core_ed25519_BYTES - 1U] &= 0x7f;
        if (memcmp(y, expected_y, crypto_core_ed25519_BYTES) != 0) {
            sodium_bin2hex(expected_y_hex, crypto_core_ed25519_BYTES * 2U + 1U,
                           expected_y, crypto_core_ed25519_BYTES);
            sodium_bin2hex(y_hex, crypto_core_ed25519_BYTES * 2U + 1U, y,
                           crypto_core_ed25519_BYTES);
            printf("Test #%u failed - expected [%s] got [%s]\n", (unsigned) i,
                   expected_y_hex, y_hex);
        }
    }

    if (crypto_core_ed25519_from_string(y, NULL, (const unsigned char *) "msg",
                                        3U, H2CHASH) != 0 ||
        crypto_core_ed25519_from_string(y, "", guard_page, 0U, H2CHASH) != 0 ||
        crypto_core_ed25519_from_string_ro(
            y, NULL, (const unsigned char *) "msg", 3U, H2CHASH) != 0 ||
        crypto_core_ed25519_from_string_ro(y, "", guard_page, 0U,
                                           H2CHASH) != 0) {
        printf("Failed with empty parameters");
    }

    oversized_ctx = (char *) sodium_malloc(oversized_ctx_len);
    memset(oversized_ctx, 'X', oversized_ctx_len - 1U);
    oversized_ctx[oversized_ctx_len - 1U] = 0;
    crypto_core_ed25519_from_string(y, oversized_ctx,
                                    (const unsigned char *) "msg", 3U,
                                    H2CHASH);
    sodium_bin2hex(y_hex, crypto_core_ed25519_BYTES * 2U + 1U, y,
                   crypto_core_ed25519_BYTES);
    printf("NU with oversized context: %s\n", y_hex);
    crypto_core_ed25519_from_string_ro(y, oversized_ctx,
                                       (const unsigned char *) "msg", 3U,
                                       H2CHASH);
    sodium_bin2hex(y_hex, crypto_core_ed25519_BYTES * 2U + 1U, y,
                   crypto_core_ed25519_BYTES);
    printf("RO with oversized context: %s\n", y_hex);

    sodium_free(oversized_ctx);
    sodium_free(y_hex);
    sodium_free(expected_y_hex);
    sodium_free(y);
    sodium_free(expected_y);
    sodium_free(expected_yr);

    printf("OK\n");

    return 0;
}
