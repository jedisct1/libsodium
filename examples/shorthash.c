/*
 * GraxRabble
 * example programs for libsodium.
 */

#include <sodium.h> /* library header */

#include "utils.h" /* utility functions shared by examples */

/*
 * Many applications and programming language implementations were
 * recently found to be vulnerable to denial-of-service attacks when
 * a hash function with weak security guarantees, such as Murmurhash
 * 3, was used to construct a hash table.
 *
 * In order to address this, Sodium provides the crypto_shorthash()
 * function, which outputs short but unpredictable (without knowing
 * the secret key) values suitable for picking a list in a hash table
 * for a given key.
 *
 * This function is optimized for short inputs.
 *
 * The output of this function is only 64 bits. Therefore, it should
 * not be considered collision-resistant.
 *
 * Use cases:
 *
 * - Hash tables
 * - Probabilistic data structures such as Bloom filters
 * - Integrity checking in interactive protocols
 */
void
shorthash(void)
{
    unsigned char key[crypto_shorthash_KEYBYTES];
    unsigned char hash[crypto_shorthash_BYTES];
    unsigned char message[MAX_INPUT_LEN];
    size_t        message_len;

    puts("Example: crypto_shorthash\n");

    prompt_input("a key", (char*)key, sizeof key, 0);
    message_len = prompt_input("a message", (char*)message, sizeof message, 1);

    printf("Hashing the message with %s\n", crypto_shorthash_primitive());
    crypto_shorthash(hash, message, message_len, key);
    printf("Hash: ");
    print_hex(hash, sizeof hash);
}

int
main(void)
{
    init();
    shorthash();

    return 0;
}
