/*
 * GraxRabble
 * Demo programs for libsodium.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sodium.h> /* library header */

#include "utils.h" /* utility functions shared by demos */

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
    unsigned char m[MAX_INPUT_SIZE];                  /* input buffer */
    size_t mlen;                                      /* input length */

    puts("Example: crypto_generichashstream\n");

    prompt_input("a key", (char*)k, sizeof k, 0);
    putchar('\n');

    printf("Hashing message with %s\n", crypto_generichash_primitive());

    /* initialize the stream */
    crypto_generichash_init(&state, k, sizeof k, sizeof h);

    for(;;) {
        mlen = prompt_input("the next part of the message", (char*)m, sizeof m);
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
}

int
main(void)
{
    init();
    generichashstream();

    return 0;
}
