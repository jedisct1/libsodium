#define TEST_NAME "core_ed25519_h2c"
#include "cmptest.h"

typedef struct TestData_ {
    int         ro;
    const char *msg;
    const char  tv_x[64 + 1];
    const char  tv_y[64 + 1];
} TestData;

static TestData test_data[] = {
    { 0, "",
      "1ff2b70ecf862799e11b7ae744e3489aa058ce805dd323a936375a84695e76da",
      "222e314d04a4d5725e9f2aff9fb2a6b69ef375a1214eb19021ceab2d687f0f9b" },
    { 0, "abc",
      "5f13cc69c891d86927eb37bd4afc6672360007c63f68a33ab423a3aa040fd2a8",
      "67732d50f9a26f73111dd1ed5dba225614e538599db58ba30aaea1f5c827fa42" },
    { 0, "abcdef0123456789",
      "1dd2fefce934ecfd7aae6ec998de088d7dd03316aa1847198aecf699ba6613f1",
      "2f8a6c24dd1adde73909cada6a4a137577b0f179d336685c4a955a0a8e1a86fb" },
    { 0,
      "q128_"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",
      "35fbdc5143e8a97afd3096f2b843e07df72e15bfca2eaf6879bf97c5d3362f73",
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
      "6e5e1f37e99345887fc12111575fc1c3e36df4b289b8759d23af14d774b66bff",
      "2c90c3d39eb18ff291d33441b35f3262cdd307162cc97c31bfcc7a4245891a37" },
    { 1, "",
      "3c3da6925a3c3c268448dcabb47ccde5439559d9599646a8260e47b1e4822fc6",
      "09a6c8561a0b22bef63124c588ce4c62ea83a3c899763af26d795302e115dc21" },
    { 1, "abc",
      "608040b42285cc0d72cbb3985c6b04c935370c7361f4b7fbdb1ae7f8c1a8ecad",
      "1a8395b88338f22e435bbd301183e7f20a5f9de643f11882fb237f88268a5531" },
    { 1, "abcdef0123456789",
      "6d7fabf47a2dc03fe7d47f7dddd21082c5fb8f86743cd020f3fb147d57161472",
      "53060a3d140e7fbcda641ed3cf42c88a75411e648a1add71217f70ea8ec561a6" },
    { 1,
      "q128_"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",
      "5fb0b92acedd16f3bcb0ef83f5c7b7a9466b5f1e0d8d217421878ea3686f8524",
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
      "0efcfde5898a839b00997fbe40d2ebe950bc81181afbd5cd6b9618aa336c1e8c",
      "6dc2fc04f266c5c27f236a80b14f92ccd051ef1ff027f26a07f8c0f327d8f995" }
};

#define H2CHASH crypto_core_ed25519_H2CSHA512

int
main(void)
{
    unsigned char *expected_xr, *expected_x;
    unsigned char *expected_yr, *expected_y, *y;
    unsigned char  expected_x_sign;
    char *         expected_y_hex, *y_hex;
    char *         oversized_ctx;
    size_t         i, j;
    size_t         oversized_ctx_len = 500U;

    expected_xr = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    expected_x  = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    expected_yr = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    expected_y  = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    y           = (unsigned char *) sodium_malloc(crypto_core_ed25519_BYTES);
    expected_y_hex =
        (char *) sodium_malloc(crypto_core_ed25519_BYTES * 2U + 1U);
    y_hex = (char *) sodium_malloc(crypto_core_ed25519_BYTES * 2U + 1U);
    for (i = 0U; i < (sizeof test_data) / (sizeof test_data[0]); i++) {
        sodium_hex2bin(expected_xr, crypto_core_ed25519_BYTES,
                       test_data[i].tv_x, (size_t) -1U, NULL, NULL, NULL);
        for (j = 0; j < crypto_core_ed25519_BYTES; j++) {
            expected_x[j] = expected_xr[crypto_core_ed25519_BYTES - 1U - j];
        }
        expected_x_sign = expected_x[0] & 1;

        sodium_hex2bin(expected_yr, crypto_core_ed25519_BYTES,
                       test_data[i].tv_y, (size_t) -1U, NULL, NULL, NULL);
        for (j = 0; j < crypto_core_ed25519_BYTES; j++) {
            expected_y[j] = expected_yr[crypto_core_ed25519_BYTES - 1U - j];
        }
        if (test_data[i].ro == 0) {
            if (crypto_core_ed25519_from_string(
                    y,
                    (const unsigned char *) "QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_",
                    sizeof("QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_") - 1U,
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg), H2CHASH) != 0) {
                printf("crypto_core_ed25519_from_string() failed\n");
            }
        } else {
            if (crypto_core_ed25519_from_string_ro(
                    y,
                    (const unsigned char *) "QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_",
                    sizeof("QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_") - 1U,
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg), H2CHASH) != 0) {
                printf("crypto_core_ed25519_from_string_ro() failed\n");
            }
        }
        if (((y[crypto_core_ed25519_BYTES - 1U] >> 7) & 1) != expected_x_sign) {
            printf("Test #%u failed - x sign bit mismatch\n", (unsigned) i);
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

    if (crypto_core_ed25519_from_string(y, NULL, 0U, (const unsigned char *) "msg",
                                        3U, H2CHASH) != 0 ||
        crypto_core_ed25519_from_string(y, (const unsigned char *) "", 0U,
                                        guard_page, 0U, H2CHASH) != 0 ||
        crypto_core_ed25519_from_string_ro(
            y, NULL, 0U, (const unsigned char *) "msg", 3U, H2CHASH) != 0 ||
        crypto_core_ed25519_from_string_ro(y, (const unsigned char *) "", 0U,
                                           guard_page, 0U, H2CHASH) != 0) {
        printf("Failed with empty parameters");
    }

    oversized_ctx = (char *) sodium_malloc(oversized_ctx_len);
    memset(oversized_ctx, 'X', oversized_ctx_len);
    crypto_core_ed25519_from_string(y, (const unsigned char *) oversized_ctx,
                                    oversized_ctx_len - 1U,
                                    (const unsigned char *) "msg", 3U,
                                    H2CHASH);
    sodium_bin2hex(y_hex, crypto_core_ed25519_BYTES * 2U + 1U, y,
                   crypto_core_ed25519_BYTES);
    printf("NU with oversized context: %s\n", y_hex);
    crypto_core_ed25519_from_string_ro(y, (const unsigned char *) oversized_ctx,
                                       oversized_ctx_len - 1U,
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
    sodium_free(expected_x);
    sodium_free(expected_xr);

    printf("OK\n");

    return 0;
}
