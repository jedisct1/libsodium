#define TEST_NAME "aead_aegis128x2"
#include "cmptest.h"

static struct {
    const char *key_hex;
    const char *nonce_hex;
    const char *message_hex;
    const char *ad_hex;
    const char *ciphertext_hex;
    const char *mac_hex;
} tests[] = {
    { "000102030405060708090a0b0c0d0e0f", "101112131415161718191a1b1c1d1e1f",
      "", "", "",
      "b92c71fdbd358b8a4de70b27631ace90cffd9b9cfba82028412bac41b4f53759" },
};

static int
tv(void)
{
    unsigned char     *ad;
    unsigned char     *ciphertext;
    unsigned char     *decrypted;
    unsigned char     *detached_ciphertext;
    unsigned char     *expected_ciphertext;
    unsigned char     *key;
    unsigned char     *message;
    unsigned char     *mac;
    unsigned char     *nonce;
    char              *hex;
    unsigned long long found_ciphertext_len;
    unsigned long long found_mac_len;
    unsigned long long found_message_len;
    size_t             ad_len;
    size_t             ciphertext_len;
    size_t             detached_ciphertext_len;
    size_t             i = 0U;
    size_t             message_len;

    key   = (unsigned char *) sodium_malloc(crypto_aead_aegis128x2_KEYBYTES);
    nonce = (unsigned char *) sodium_malloc(crypto_aead_aegis128x2_NPUBBYTES);
    mac   = (unsigned char *) sodium_malloc(crypto_aead_aegis128x2_ABYTES);

    do {
        assert(strlen(tests[i].key_hex) == 2 * crypto_aead_aegis128x2_KEYBYTES);
        sodium_hex2bin(key, crypto_aead_aegis128x2_KEYBYTES, tests[i].key_hex,
                       strlen(tests[i].key_hex), NULL, NULL, NULL);
        assert(strlen(tests[i].nonce_hex) == 2 * crypto_aead_aegis128x2_NPUBBYTES);
        sodium_hex2bin(nonce, crypto_aead_aegis128x2_NPUBBYTES, tests[i].nonce_hex,
                       strlen(tests[i].nonce_hex), NULL, NULL, NULL);
        message_len = strlen(tests[i].message_hex) / 2;
        message     = (unsigned char *) sodium_malloc(message_len);
        sodium_hex2bin(message, message_len, tests[i].message_hex, strlen(tests[i].message_hex),
                       NULL, NULL, NULL);
        ad_len = strlen(tests[i].ad_hex) / 2;
        ad     = (unsigned char *) sodium_malloc(ad_len);
        sodium_hex2bin(ad, ad_len, tests[i].ad_hex, strlen(tests[i].ad_hex), NULL, NULL, NULL);
        ciphertext_len          = message_len + crypto_aead_aegis128x2_ABYTES;
        detached_ciphertext_len = message_len;
        expected_ciphertext     = (unsigned char *) sodium_malloc(ciphertext_len);
        assert(strlen(tests[i].ciphertext_hex) == 2 * message_len);
        sodium_hex2bin(expected_ciphertext, message_len, tests[i].ciphertext_hex,
                       strlen(tests[i].ciphertext_hex), NULL, NULL, NULL);
        assert(strlen(tests[i].mac_hex) == 2 * crypto_aead_aegis128x2_ABYTES);
        sodium_hex2bin(expected_ciphertext + message_len, crypto_aead_aegis128x2_ABYTES,
                       tests[i].mac_hex, strlen(tests[i].mac_hex), NULL, NULL, NULL);
        ciphertext          = (unsigned char *) sodium_malloc(ciphertext_len);
        detached_ciphertext = (unsigned char *) sodium_malloc(detached_ciphertext_len);

        crypto_aead_aegis128x2_encrypt_detached(detached_ciphertext, mac, &found_mac_len, message,
                                               message_len, ad, ad_len, NULL, nonce, key);
        assert(found_mac_len == crypto_aead_aegis128x2_ABYTES);
        if (memcmp(detached_ciphertext, expected_ciphertext, detached_ciphertext_len) != 0 ||
            memcmp(mac, expected_ciphertext + message_len, crypto_aead_aegis128x2_ABYTES) != 0) {
            printf("Detached encryption of test vector #%u failed\n", (unsigned int) i);
            hex = (char *) sodium_malloc((size_t) ciphertext_len * 2 + 1);
            sodium_bin2hex(hex, (size_t) ciphertext_len * 2 + 1, ciphertext, ciphertext_len);
            printf("Computed: [%s]\n", hex);
            sodium_free(hex);
        }

        crypto_aead_aegis128x2_encrypt(ciphertext, &found_ciphertext_len, message, message_len, ad,
                                      ad_len, NULL, nonce, key);

        assert((size_t) found_ciphertext_len == ciphertext_len);
        if (memcmp(ciphertext, expected_ciphertext, ciphertext_len) != 0) {
            printf("Encryption of test vector #%u failed\n", (unsigned int) i);
            hex = (char *) sodium_malloc((size_t) found_ciphertext_len * 2 + 1);
            sodium_bin2hex(hex, (size_t) found_ciphertext_len * 2 + 1, ciphertext, ciphertext_len);
            printf("Computed: [%s]\n", hex);
            sodium_free(hex);
        }

        decrypted         = (unsigned char *) sodium_malloc(message_len);
        found_message_len = 1;
        if (crypto_aead_aegis128x2_decrypt(decrypted, &found_message_len, NULL, ciphertext,
                                          randombytes_uniform((uint32_t) ciphertext_len), ad,
                                          ad_len, nonce, key) != -1) {
            printf("Verification of test vector #%u after truncation succeeded\n",
                   (unsigned int) i);
        }
        if (found_message_len != 0) {
            printf("Message length should have been set to zero after a failure\n");
        }
        if (crypto_aead_aegis128x2_decrypt(decrypted, &found_message_len, NULL, guard_page,
                                          randombytes_uniform(crypto_aead_aegis128x2_ABYTES), ad,
                                          ad_len, nonce, key) != -1) {
            printf("Verification of test vector #%u with a truncated tag failed\n",
                   (unsigned int) i);
        }
        if (i == 0 && crypto_aead_aegis128x2_decrypt(NULL, NULL, NULL, ciphertext, ciphertext_len,
                                                    ad, ad_len, nonce, key) != 0) {
            printf("Verification of test vector #%u's tag failed\n", (unsigned int) i);
        }
        if (crypto_aead_aegis128x2_decrypt(decrypted, &found_message_len, NULL, ciphertext,
                                          ciphertext_len, ad, ad_len, nonce, key) != 0) {
            printf("Verification of test vector #%u failed\n", (unsigned int) i);
        }
        assert((size_t) found_message_len == message_len);
        if (memcmp(decrypted, message, message_len) != 0) {
            printf("Incorrect decryption of test vector #%u\n", (unsigned int) i);
        }
        memset(decrypted, 0xd0, message_len);
        if (crypto_aead_aegis128x2_decrypt_detached(decrypted, NULL, detached_ciphertext,
                                                   detached_ciphertext_len, mac, ad, ad_len, nonce,
                                                   key) != 0) {
            printf("Detached verification of test vector #%u failed\n", (unsigned int) i);
        }
        if (memcmp(decrypted, message, message_len) != 0) {
            printf("Incorrect decryption of test vector #%u\n", (unsigned int) i);
        }

        sodium_free(message);
        sodium_free(ad);
        sodium_free(expected_ciphertext);
        sodium_free(ciphertext);
        sodium_free(decrypted);
        sodium_free(detached_ciphertext);
    } while (++i < (sizeof tests) / (sizeof tests[0]));

    sodium_free(key);
    sodium_free(mac);
    sodium_free(nonce);

    return 0;
}

int
main(void)
{
    tv();
    assert(crypto_aead_aegis128x2_keybytes() == crypto_aead_aegis128x2_KEYBYTES);
    assert(crypto_aead_aegis128x2_nsecbytes() == crypto_aead_aegis128x2_NSECBYTES);
    assert(crypto_aead_aegis128x2_npubbytes() == crypto_aead_aegis128x2_NPUBBYTES);
    assert(crypto_aead_aegis128x2_abytes() == crypto_aead_aegis128x2_ABYTES);
    assert(crypto_aead_aegis128x2_messagebytes_max() == crypto_aead_aegis128x2_MESSAGEBYTES_MAX);

    {
        unsigned char k[crypto_aead_aegis128x2_KEYBYTES];
        crypto_aead_aegis128x2_keygen(k);
        assert(!sodium_is_zero(k, sizeof k));
    }
    printf("OK\n");

    return 0;
}
