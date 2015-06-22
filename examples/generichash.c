/*
 * GraxRabble
 * example programs for libsodium.
 */

#include <sodium.h> /* library header */

#include "utils.h" /* utility functions shared by examples */

/*
 * This function computes a fixed-length fingerprint for an arbitrary long message.
 *
 * Sample use cases:
 *
 * File integrity checking
 * Creating unique identifiers to index arbitrary long data
 *
 * The crypto_generichash() function puts a fingerprint of the
 * message in whose length is inlen bytes into out. The output size
 * can be chosen by the application.
 *
 * The minimum recommended output size is crypto_generichash_BYTES.
 * This size makes it practically impossible for two messages to
 * produce the same fingerprint.
 *
 * But for specific use cases, the size can be any value between
 * crypto_generichash_BYTES_MIN (included) and
 * crypto_generichash_BYTES_MAX (included).
 *
 * key can be NULL and keylen can be 0. In this case, a message will
 * always have the same fingerprint, similar to the MD5 or SHA-1
 * functions for which crypto_generichash() is a faster and more
 * secure alternative.
 *
 * But a key can also be specified. A message will always have the
 * same fingerprint for a given key, but different keys used to hash
 * the same message are very likely to produce distinct fingerprints.
 *
 * In particular, the key can be used to make sure that different
 * applications generate different fingerprints even if they process
 * the same data.
 *
 * The recommended key size is crypto_generichash_KEYBYTES bytes.
 *
 * However, the key size can by any value between
 * crypto_generichash_KEYBYTES_MIN (included) and
 * crypto_generichash_KEYBYTES_MAX (included).
 */
void
generichash(void)
{
    unsigned char key[crypto_generichash_KEYBYTES_MAX];
    unsigned char hash[crypto_generichash_BYTES];
    unsigned char message[MAX_INPUT_LEN];
    size_t        message_len;
    size_t        key_len;

    puts("Example: crypto_generichash\n");

    key_len = prompt_input("a key", (char*)key, sizeof key, 1);
    message_len = prompt_input("a message", (char*)message, sizeof message, 1);

    printf("Hashing message with %s\n", crypto_generichash_primitive());
    if (crypto_generichash(hash, sizeof hash, message, message_len,
                           key, key_len) != 0) {
        puts("Couldn't hash the message, probably due to the key length");
    } else {
        printf("Hash: ");
        print_hex(hash, sizeof hash);
    }
}

int
main(void)
{
    init();
    generichash();

    return 0;
}
