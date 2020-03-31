#define TEST_NAME "core_ed25519_h2c"
#include "cmptest.h"

typedef struct TestData_ {
    int         ro;
    const char *msg;
    const char  tv_y[64 + 1];
} TestData;

static TestData test_data[] = {
    { 0, "",
      "115fe9c145cec75332210f75537f89a5af2e9e81928ab63225fcf0db4b9f0b41" },
    { 0, "abc",
      "2a78994752306e4a4eb1c629b3c8625cc26f143f1fe35fc728d295222eba7890" },
    { 0, "abcdef0123456789",
      "0ca9e2b900bcdfc8b26d705040b6b6a6fd6bfadfd43982a2a59e1d069898ae6b" },
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
      "511dee3fcc3a422738c453a58acdeafc08fc76f862e97d678adb26f03d0963b5" },

    { 1, "",
      "1af40833bed99ac42f445e9494dbcc489561b3995a40e3864a1b1bdb6ed6ecd0" },
    { 1, "abc",
      "7bdb83b6322c4977ffbb69b18df168b56eec733a0254cac0e85eb790460ee4b2" },
    { 1, "abcdef0123456789",
      "2dd85e9585fba5ab3c22a07cf07b2cf4a19b10dcab49e1a4c8f952461e644bbc" },
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
      "160e08388e85b6bcbec8f321ed1de259ffe88dc547a1ce506d61b30fd2378fc5" }
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
