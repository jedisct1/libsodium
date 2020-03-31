
#define TEST_NAME "kdf"
#include "cmptest.h"

static void
tv_kdf(void)
{
    unsigned char *master_key;
    unsigned char *subkey;
    char          *context;
    char           hex[crypto_kdf_BYTES_MAX * 2 + 1];
    uint64_t       i;
    int            ret;

    context = (char *) sodium_malloc(crypto_kdf_CONTEXTBYTES);
    master_key = (unsigned char *) sodium_malloc(crypto_kdf_KEYBYTES);

    memcpy(context, "KDF test", sizeof "KDF test" - 1U);
    for (i = 0; i < crypto_kdf_KEYBYTES; i++) {
        master_key[i] = i;
    }
    subkey = (unsigned char *) sodium_malloc(crypto_kdf_BYTES_MAX);
    for (i = 0; i < 10; i++) {
        ret = crypto_kdf_derive_from_key(subkey, crypto_kdf_BYTES_MAX,
                                         i, context, master_key);
        assert(ret == 0);
        sodium_bin2hex(hex, sizeof hex, subkey, crypto_kdf_BYTES_MAX);
        printf("%s\n", hex);
    }
    sodium_free(subkey);

    for (i = 0; i < crypto_kdf_BYTES_MAX + 2; i++) {
        subkey = (unsigned char *) sodium_malloc(crypto_kdf_BYTES_MAX);
        if (crypto_kdf_derive_from_key(subkey, (size_t) i,
                                       i, context, master_key) == 0) {
            sodium_bin2hex(hex, sizeof hex, subkey, (size_t) i);
            printf("%s\n", hex);
        } else {
            printf("Failure -- probably expected for output length=%u\n",
                   (unsigned int) i);
        }
        sodium_free(subkey);
    }

    sodium_free(master_key);
    sodium_free(context);

    assert(strcmp(crypto_kdf_primitive(), crypto_kdf_PRIMITIVE) == 0);
    assert(crypto_kdf_BYTES_MAX > 0);
    assert(crypto_kdf_BYTES_MIN <= crypto_kdf_BYTES_MAX);
    assert(crypto_kdf_bytes_min() == crypto_kdf_BYTES_MIN);
    assert(crypto_kdf_bytes_max() == crypto_kdf_BYTES_MAX);
    assert(crypto_kdf_CONTEXTBYTES > 0);
    assert(crypto_kdf_contextbytes() == crypto_kdf_CONTEXTBYTES);
    assert(crypto_kdf_KEYBYTES >= 16);
    assert(crypto_kdf_keybytes() == crypto_kdf_KEYBYTES);
    assert(crypto_kdf_bytes_min() == crypto_kdf_blake2b_bytes_min());
    assert(crypto_kdf_bytes_max() == crypto_kdf_blake2b_bytes_max());
    assert(crypto_kdf_contextbytes() == crypto_kdf_blake2b_contextbytes());
    assert(crypto_kdf_keybytes() == crypto_kdf_blake2b_keybytes());

    printf("tv_kdf: ok\n");
}

#ifndef SODIUM_LIBRARY_MINIMAL
static void
tv_kdf_hkdf(void)
{
    unsigned char *master_key;
    size_t         master_key_len = 66;
    unsigned char *prk256;
    size_t         prk256_len = crypto_kdf_hkdf_sha256_KEYBYTES;
    unsigned char *prk512;
    size_t         prk512_len = crypto_kdf_hkdf_sha512_KEYBYTES;
    unsigned char *salt;
    size_t         salt_len = 77;
    char          *context;
    size_t         context_len = 88;
    unsigned char *out;
    size_t         out_len = 99;
    char           hex[99 * 2 + 1];
    size_t         i;
    int            ret;

    master_key = (unsigned char *) sodium_malloc(master_key_len);
    prk256 = (unsigned char *) sodium_malloc(prk256_len);
    prk512 = (unsigned char *) sodium_malloc(prk512_len);
    salt = (unsigned char *) sodium_malloc(salt_len);
    context = (char *) sodium_malloc(context_len);
    out = (unsigned char *) sodium_malloc(out_len);
    for (i = 0; i < master_key_len; i++) {
        master_key[i] = i;
    }
    for (i = 0; i < salt_len; i++) {
        salt[i] = (unsigned char) ~i;
    }
    for (i = 0; i < context_len; i++) {
        context[i] = (unsigned char) (i + 111);
    }

    printf("\nHKDF/SHA-256:\n");
    crypto_kdf_hkdf_sha256_keygen(prk256);
    if (crypto_kdf_hkdf_sha256_extract(prk256, salt, salt_len,
                                       master_key, master_key_len) != 0) {
        printf("hkdf_sha256_extract() failed\n");
    }
    printf("PRK: %s\n", sodium_bin2hex(hex, sizeof hex, prk256, prk256_len));

    for (i = 0; i < out_len; i++) {
        context[0] = i;
        if (crypto_kdf_hkdf_sha256_expand(out, i,
                                          context, context_len, prk256) != 0) {
            printf("hkdf_sha256_expand() failed\n");
        }
        printf("%s\n", sodium_bin2hex(hex, sizeof hex, out, i));
    }

    printf("\nHKDF/SHA-512:\n");
    crypto_kdf_hkdf_sha256_keygen(prk512);
    if (crypto_kdf_hkdf_sha512_extract(prk512, salt, salt_len,
                                       master_key, master_key_len) != 0) {
        printf("hkdf_sha512_extract() failed\n");
    }
    printf("PRK: %s\n", sodium_bin2hex(hex, sizeof hex, prk512, prk512_len));

    for (i = 0; i < out_len; i++) {
        context[0] = i;
        if (crypto_kdf_hkdf_sha512_expand(out, i,
                                          context, context_len, prk512) != 0) {
            printf("hkdf_sha512_expand() failed\n");
        }
        printf("%s\n", sodium_bin2hex(hex, sizeof hex, out, i));
    }

    sodium_free(out);
    sodium_free(context);
    sodium_free(salt);
    sodium_free(master_key);
    sodium_free(prk512);
    sodium_free(prk256);

    assert(crypto_kdf_hkdf_sha512_bytes_min() == crypto_kdf_hkdf_sha512_BYTES_MIN);
    assert(crypto_kdf_hkdf_sha512_bytes_max() == crypto_kdf_hkdf_sha512_BYTES_MAX);
    assert(crypto_kdf_hkdf_sha512_keybytes() == crypto_kdf_hkdf_sha512_KEYBYTES);

    assert(crypto_kdf_hkdf_sha256_bytes_min() == crypto_kdf_hkdf_sha256_BYTES_MIN);
    assert(crypto_kdf_hkdf_sha256_bytes_max() == crypto_kdf_hkdf_sha256_BYTES_MAX);
    assert(crypto_kdf_hkdf_sha256_keybytes() == crypto_kdf_hkdf_sha256_KEYBYTES);

    assert(crypto_kdf_hkdf_sha256_KEYBYTES < crypto_kdf_hkdf_sha512_KEYBYTES);

    printf("tv_kdf_hkdf: ok\n");
}
#endif

int
main(void)
{
    tv_kdf();

#ifndef SODIUM_LIBRARY_MINIMAL
    tv_kdf_hkdf();
#endif

    return 0;
}
