#define TEST_NAME "kdf_hkdf4"
#include "cmptest.h"

int
main(void)
{
    unsigned char prk256[crypto_kdf_hkdf_sha256_KEYBYTES];
    unsigned char prk512[crypto_kdf_hkdf_sha512_KEYBYTES];
    unsigned char out256[256];
    unsigned char out512[256];
    char          hex[65];

    crypto_kdf_hkdf_sha256_extract(prk256, NULL, 0,
                                   (const unsigned char *) "input key material", 18);

    assert(crypto_kdf_hkdf_sha256_expand(out256, 100, "context", 7, prk256) == 0);
    sodium_bin2hex(hex, sizeof hex, out256, 32);
    printf("sha256 expand 100 first 32: %s\n", hex);
    sodium_bin2hex(hex, sizeof hex, out256 + 64, 32);
    printf("sha256 expand 100 bytes 64-95: %s\n", hex);

    crypto_kdf_hkdf_sha512_extract(prk512, NULL, 0,
                                   (const unsigned char *) "input key material", 18);

    assert(crypto_kdf_hkdf_sha512_expand(out512, 200, "context", 7, prk512) == 0);
    sodium_bin2hex(hex, sizeof hex, out512, 32);
    printf("sha512 expand 200 first 32: %s\n", hex);
    sodium_bin2hex(hex, sizeof hex, out512 + 128, 32);
    printf("sha512 expand 200 bytes 128-159: %s\n", hex);

    assert(crypto_kdf_hkdf_sha512_expand(out512, 128, "ctx2", 4, prk512) == 0);
    sodium_bin2hex(hex, sizeof hex, out512 + 64, 32);
    printf("sha512 expand 128 second block: %s\n", hex);

    assert(crypto_kdf_hkdf_sha512_expand(out512, 64, "ctx3", 4, prk512) == 0);
    sodium_bin2hex(hex, sizeof hex, out512, 32);
    printf("sha512 expand 64: %s\n", hex);

    printf("ok\n");

    return 0;
}
