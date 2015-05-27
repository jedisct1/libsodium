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
 * The library ships with a one-shot SHA-512 implementation. Simply allocate
 * all desired data into a single continuous buffer.
 */
static void
hash(void)
{
    unsigned char h[crypto_hash_BYTES]; /* hash output */
    unsigned char m[MAX_INPUT_SIZE];    /* message */
    size_t mlen;                        /* length */

    puts("Example: crypto_hash\n");

    mlen = prompt_input("Input your message > ", (char*)m, sizeof m);
    putchar('\n');

    printf("Hashing message with %s\n", crypto_hash_primitive());
    crypto_hash(h, m, mlen);
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

    hash();
    return 0;
}
