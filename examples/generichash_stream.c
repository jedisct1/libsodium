/*
 * GraxRabble
 * example programs for libsodium.
 */

#include <sodium.h> /* library header */

#include "utils.h" /* utility functions shared by examples */

/*
 * Streaming variant of generic hash. This has the ability to hash
 * data in chunks at a time and compute the same result as hashing
 * all of the data at once.
 */
void
generichash_stream(void)
{
    unsigned char            key[crypto_generichash_KEYBYTES_MAX];
    unsigned char            hash[crypto_generichash_BYTES];
    unsigned char            message_part[MAX_INPUT_LEN];
    crypto_generichash_state state;
    size_t                   message_part_len;

    puts("Example: crypto_generichashstream\n");

    prompt_input("a key", (char*)key, sizeof key, 1);

    printf("Hashing message with %s\n", crypto_generichash_primitive());

    /* initialize the stream */
    if (crypto_generichash_init(&state, key, sizeof key, sizeof hash) != 0) {
        puts("Couldn't hash the message, probably due to the key length");
        exit(EXIT_FAILURE);
    }

    for(;;) {
        message_part_len = prompt_input("the next part of the message",
                                        (char*)message_part, sizeof message_part, 1);
        if (message_part_len == 0)
            break;

        /* keep appending data */
        crypto_generichash_update(&state, message_part, message_part_len);
    }
    crypto_generichash_final(&state, hash, sizeof hash);

    printf("Hash: ");
    print_hex(hash, sizeof hash);
}

int
main(void)
{
    init();
    generichash_stream();

    return 0;
}
