/*
 * GraxRabble
 * Demo programs for libsodium.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sodium.h> /* library header */

#include "demo_utils.h" /* utility functions shared by demos */

/*
 * Streaming variant of generic hash. This has the ability to hash
 * data in chunks at a time and compute the same result as hashing
 * all of the data at once.
 */
void
generichashstream(void)
{
    unsigned char k[crypto_generichash_KEYBYTES_MAX]; /* key */
    unsigned char h[crypto_generichash_BYTES_MIN];    /* hash output */
    crypto_generichash_state state;                   /* hash stream */
    unsigned char m[BUFFER_SIZE];                     /* input buffer */
    size_t mlen;                                      /* input length */

    puts("Example: crypto_generichashstream\n");

    sodium_memzero(k, sizeof k);
    prompt_input("Input your key > ", (char*)k, sizeof k);
    putchar('\n');

    printf("Hashing message with %s\n", crypto_generichash_primitive());

    /* initialize the stream */
    crypto_generichash_init(&state, k, sizeof k, sizeof h);

    while (1) {
        mlen = prompt_input("> ", (char*)m, sizeof m);
        if (mlen == 0)
            break;

        /* keep appending data */
        crypto_generichash_update(&state, m, mlen);
    }
    crypto_generichash_final(&state, h, sizeof h);
    putchar('\n');

    fputs("Hash: ", stdout);
    print_hex(h, sizeof h);
    putchar('\n');
    putchar('\n');
}

int
main(void)
{
    sodium_init();
    printf("Using LibSodium %s\n", sodium_version_string());

    generichashstream();
    return 0;
}
