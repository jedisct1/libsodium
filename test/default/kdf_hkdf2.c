
#define TEST_NAME "kdf_hkdf2"
#include "cmptest.h"

static void
tv_kdf_hkdf_incremental(void)
{
    unsigned char *master_key;
    size_t         master_key_len = 66;
    unsigned char *salt;
    size_t         salt_len = 77;
    char          *context;
    size_t         context_len = 88;
    unsigned char  prk_oneshot[crypto_kdf_hkdf_sha512_KEYBYTES];
    unsigned char  prk_incremental[crypto_kdf_hkdf_sha512_KEYBYTES];
    unsigned char  prk256_oneshot[crypto_kdf_hkdf_sha256_KEYBYTES];
    unsigned char  prk256_incremental[crypto_kdf_hkdf_sha256_KEYBYTES];
    unsigned char  out_oneshot[99];
    unsigned char  out_incremental[99];
    char           hex[99 * 2 + 1];
    size_t         i;

    master_key = (unsigned char *) sodium_malloc(master_key_len);
    salt = (unsigned char *) sodium_malloc(salt_len);
    context = (char *) sodium_malloc(context_len);
    for (i = 0; i < master_key_len; i++) {
        master_key[i] = (unsigned char) i;
    }
    for (i = 0; i < salt_len; i++) {
        salt[i] = (unsigned char) ~i;
    }
    for (i = 0; i < context_len; i++) {
        context[i] = (unsigned char) (i + 111);
    }

    crypto_kdf_hkdf_sha512_extract(prk_oneshot, salt, salt_len,
                                   master_key, master_key_len);

    {
        crypto_kdf_hkdf_sha512_state state;

        crypto_kdf_hkdf_sha512_extract_init(&state, salt, salt_len);
        crypto_kdf_hkdf_sha512_extract_update(&state, master_key,
                                              master_key_len);
        crypto_kdf_hkdf_sha512_extract_final(&state, prk_incremental);
    }
    assert(memcmp(prk_oneshot, prk_incremental, sizeof prk_oneshot) == 0);
    printf("sha512 incremental (single update): ok\n");

    {
        crypto_kdf_hkdf_sha512_state state;

        crypto_kdf_hkdf_sha512_extract_init(&state, salt, salt_len);
        for (i = 0; i < master_key_len; i++) {
            crypto_kdf_hkdf_sha512_extract_update(&state, &master_key[i], 1);
        }
        crypto_kdf_hkdf_sha512_extract_final(&state, prk_incremental);
    }
    assert(memcmp(prk_oneshot, prk_incremental, sizeof prk_oneshot) == 0);
    printf("sha512 incremental (byte-by-byte): ok\n");

    {
        crypto_kdf_hkdf_sha512_state state;
        unsigned char prk_null_salt_oneshot[crypto_kdf_hkdf_sha512_KEYBYTES];
        unsigned char prk_null_salt_incr[crypto_kdf_hkdf_sha512_KEYBYTES];

        crypto_kdf_hkdf_sha512_extract(prk_null_salt_oneshot, NULL, 0,
                                       master_key, master_key_len);
        crypto_kdf_hkdf_sha512_extract_init(&state, NULL, 0);
        crypto_kdf_hkdf_sha512_extract_update(&state, master_key,
                                              master_key_len);
        crypto_kdf_hkdf_sha512_extract_final(&state, prk_null_salt_incr);
        assert(memcmp(prk_null_salt_oneshot, prk_null_salt_incr,
                      sizeof prk_null_salt_oneshot) == 0);
    }
    printf("sha512 incremental (null salt): ok\n");

    crypto_kdf_hkdf_sha512_expand(out_oneshot, 99, context, context_len,
                                  prk_oneshot);
    crypto_kdf_hkdf_sha512_expand(out_incremental, 99, context, context_len,
                                  prk_incremental);
    assert(memcmp(out_oneshot, out_incremental, 99) == 0);
    printf("sha512 expand consistency: ok\n");

    sodium_bin2hex(hex, sizeof hex, prk_oneshot, sizeof prk_oneshot);
    printf("sha512 prk: %s\n", hex);

    crypto_kdf_hkdf_sha256_extract(prk256_oneshot, salt, salt_len,
                                   master_key, master_key_len);

    {
        crypto_kdf_hkdf_sha256_state state;
        size_t half = master_key_len / 2;

        crypto_kdf_hkdf_sha256_extract_init(&state, salt, salt_len);
        crypto_kdf_hkdf_sha256_extract_update(&state, master_key, half);
        crypto_kdf_hkdf_sha256_extract_update(&state, master_key + half,
                                              master_key_len - half);
        crypto_kdf_hkdf_sha256_extract_final(&state, prk256_incremental);
    }
    assert(memcmp(prk256_oneshot, prk256_incremental,
                  sizeof prk256_oneshot) == 0);
    printf("sha256 incremental (two chunks): ok\n");

    {
        crypto_kdf_hkdf_sha256_state state;
        unsigned char prk_null_oneshot[crypto_kdf_hkdf_sha256_KEYBYTES];
        unsigned char prk_null_incr[crypto_kdf_hkdf_sha256_KEYBYTES];

        crypto_kdf_hkdf_sha256_extract(prk_null_oneshot, NULL, 0,
                                       master_key, master_key_len);
        crypto_kdf_hkdf_sha256_extract_init(&state, NULL, 0);
        crypto_kdf_hkdf_sha256_extract_update(&state, master_key,
                                              master_key_len);
        crypto_kdf_hkdf_sha256_extract_final(&state, prk_null_incr);
        assert(memcmp(prk_null_oneshot, prk_null_incr,
                      sizeof prk_null_oneshot) == 0);
    }
    printf("sha256 incremental (null salt): ok\n");

    sodium_bin2hex(hex, sizeof hex, prk256_oneshot, sizeof prk256_oneshot);
    printf("sha256 prk: %s\n", hex);

    sodium_free(context);
    sodium_free(salt);
    sodium_free(master_key);
}

int
main(void)
{
    tv_kdf_hkdf_incremental();

    return 0;
}
