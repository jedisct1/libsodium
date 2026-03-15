
#define TEST_NAME "kdf_hkdf3"
#include "cmptest.h"

int
main(void)
{
    unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES];
    unsigned char out[256];
    int           ret;

    crypto_kdf_hkdf_sha512_keygen(prk);
    assert(!sodium_is_zero(prk, sizeof prk));
    printf("sha512 keygen: ok\n");

    ret = crypto_kdf_hkdf_sha512_expand(
        out, crypto_kdf_hkdf_sha512_BYTES_MAX + 1,
        "ctx", 3, prk);
    assert(ret == -1);
    printf("sha512 expand max+1: ok\n");

    crypto_kdf_hkdf_sha512_extract(prk, NULL, 0,
                                   (const unsigned char *) "ikm", 3);

    ret = crypto_kdf_hkdf_sha512_expand(out, 100, "ctx", 3, prk);
    assert(ret == 0);
    printf("sha512 expand partial block: ok\n");

    {
        unsigned char prk256[crypto_kdf_hkdf_sha256_KEYBYTES];

        crypto_kdf_hkdf_sha256_keygen(prk256);
        assert(!sodium_is_zero(prk256, sizeof prk256));
        printf("sha256 keygen: ok\n");

        ret = crypto_kdf_hkdf_sha256_expand(
            out, crypto_kdf_hkdf_sha256_BYTES_MAX + 1,
            "ctx", 3, prk256);
        assert(ret == -1);
        printf("sha256 expand max+1: ok\n");
    }

    return 0;
}
