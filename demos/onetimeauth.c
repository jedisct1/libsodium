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
 * This method is only effective for a single use per key. The benefit is
 * the algorithm is quicker and output is half the size of auth. It is easy
 * to see how weak the algorithm is when you use a one letter key.
 *
 * Note that the same key must not be used more than once.
 */
static int
onetimeauth(void)
{
    unsigned char k[crypto_onetimeauth_KEYBYTES]; /* key */
    unsigned char a[crypto_onetimeauth_BYTES];    /* authentication */
    unsigned char m[MAX_INPUT_SIZE];              /* message */
    size_t mlen;                                  /* message length */
    int r;

    sodium_memzero(k, sizeof k); /* must zero the key */

    puts("Example: crypto_onetimeauth\n");

    /*
     * Keys are entered as ascii values. The key is zeroed to
     * maintain consistency. Input is read through a special
     * function which reads exactly n bytes into a buffer to
     * prevent buffer overflows.
     */
    sodium_memzero(k, sizeof k);
    prompt_input("Input your key > ", (char*)k, sizeof k);
    puts("Your key that you entered");
    print_hex(k, sizeof k);
    putchar('\n');

    mlen = prompt_input("Input your message > ", (char*)m, sizeof m);
    putchar('\n');

    printf("Generating %s authentication...\n", crypto_onetimeauth_primitive());
    crypto_onetimeauth(a, m, mlen, k);

    puts("Format: authentication token::message");
    print_hex(a, sizeof a);
    fputs("::", stdout);
    puts((const char*)m);
    putchar('\n');

    puts("Verifying authentication...");
    r = crypto_onetimeauth_verify(a, m, mlen, k);
    print_verification(r);

    sodium_memzero(k, sizeof k); /* wipe sensitive data */
    sodium_memzero(a, sizeof a);
    sodium_memzero(m, sizeof m);
    return r;
}

int
main(void)
{
    sodium_init();
    printf("Using LibSodium %s\n", sodium_version_string());

    return onetimeauth() != 0;
}
