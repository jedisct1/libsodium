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
 * Generic hash is intended as a variable output hash with enough strength
 * to ensure data integrity. The hash out put is also able to vary in size.
 * Key is optional and is able to vary in size.
 *
 * Note that it is recommended to stay within the range of MIN and MAX
 * output because larger output will produce gaps.
 */
void
generichash(void)
{
    unsigned char k[crypto_generichash_KEYBYTES_MAX]; /* key */
    unsigned char h[crypto_generichash_BYTES_MIN];    /* hash output */
    unsigned char m[MAX_INPUT_SIZE];                  /* message */
    size_t mlen;                                      /* length */

    puts("Example: crypto_generichash\n");

    memset(k, 0, sizeof k);
    prompt_input("Input your key > ", (char*)k, sizeof k);

    mlen = prompt_input("Input your message > ", (char*)m, sizeof m);
    putchar('\n');

    printf("Hashing message with %s\n", crypto_generichash_primitive());
    crypto_generichash(h, sizeof h, m, mlen, k, sizeof k);
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

    generichash();
    return 0;
}
