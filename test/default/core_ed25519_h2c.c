#define TEST_NAME "core_ed25519_h2c"
#include "cmptest.h"

typedef struct TestData_ {
    int         ro;
    const char *msg;
    const char  tv_y[64 + 1];
} TestData;

static TestData test_data[] = {
    { 0, "",
      "4af6284e3cc7116df104f6708e0c44d79b0e294ccd89b87c4c3c892ebd2f03b1" },
    { 0, "abc",
      "23e704500ac22fd7106ceedd86bfcc8d50351a6303be22b2724fcc1280d00544" },
    { 0, "abcdef0123456789",
      "34b8a16b923101f2d4caa48d9bb86fef4f92be0ce0f55c8ba9db55da23ad623e" },
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
      "31e648bbade3b272b7676f82da905d27de37f41581b1d170250dd9d56f95413c" },

    { 1, "",
      "5c307efcdf7f0822428f932e66b46b3d88f59880772a1ab07fac5231609c5f76" },
    { 1, "abc",
      "4bc72bbd27a3207ad932ea6e218b257875ad2cb34695d964da83a57f4807394d" },
    { 1, "abcdef0123456789",
      "3c67b98967527a551ea54e4de791689834552009a7a40393fd4a23d1f04061ef" },
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
      "6ce3485dc2cd37462e08e46f9931b16bb272280aea3efc38fb96d2d2df4870bd" }
};

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
                    y, "edwards25519_XMD:SHA-512_ELL2_NU_TESTGEN",
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg)) != 0) {
                printf("crypto_core_ed25519_from_string() failed\n");
            }
        } else {
            if (crypto_core_ed25519_from_string_ro(
                    y, "edwards25519_XMD:SHA-512_ELL2_RO_TESTGEN",
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg)) != 0) {
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
                                        3U) != 0 ||
        crypto_core_ed25519_from_string(y, "", guard_page, 0U) != 0 ||
        crypto_core_ed25519_from_string_ro(
            y, NULL, (const unsigned char *) "msg", 3U) != 0 ||
        crypto_core_ed25519_from_string_ro(y, "", guard_page, 0U) != 0) {
        printf("Failed with empty parameters");
    }

    oversized_ctx = (char *) sodium_malloc(oversized_ctx_len);
    memset(oversized_ctx, 'X', oversized_ctx_len - 1U);
    oversized_ctx[oversized_ctx_len - 1U] = 0;
    crypto_core_ed25519_from_string(y, oversized_ctx,
                                    (const unsigned char *) "msg", 3U);
    sodium_bin2hex(y_hex, crypto_core_ed25519_BYTES * 2U + 1U, y,
                   crypto_core_ed25519_BYTES);
    printf("NU with oversized context: %s\n", y_hex);
    crypto_core_ed25519_from_string_ro(y, oversized_ctx,
                                       (const unsigned char *) "msg", 3U);
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
