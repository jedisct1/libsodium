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
 * Short hash is a fast algorithm intended for hash tables and anything
 * else that does not require data integrity. There is the added benefit
 * of a key which will alter the output of the hash.
 */
void
shorthash(void)
{
    unsigned char k[crypto_shorthash_KEYBYTES]; /* key */
    unsigned char h[crypto_shorthash_BYTES];    /* hash output */
    unsigned char m[MAX_INPUT_SIZE];            /* message */
    size_t mlen;                                /* length */

    puts("Example: crypto_shorthash\n");

    memset(k, 0, sizeof k);
    prompt_input("Input your key > ", (char*)k, sizeof k);

    mlen = prompt_input("Input your message > ", (char*)m, sizeof m);
    putchar('\n');

    printf("Hashing message with %s\n", crypto_shorthash_primitive());
    crypto_shorthash(h, m, mlen, k);
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

    shorthash();
    return 0;
}
