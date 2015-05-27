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
 * Full featured authentication which is used to verify that the message
 * comes from the expected person. It should be safe to keep the same key
 * for multiple messages.
 */
static int
auth(void)
{
    unsigned char k[crypto_auth_KEYBYTES]; /* key */
    unsigned char a[crypto_auth_BYTES];    /* authentication token */
    unsigned char m[MAX_INPUT_SIZE];       /* message */
    size_t mlen;                           /* message length */
    int r;

    puts("Example: crypto_auth\n");

    /*
     * Keys are entered as ascii values. The key is zeroed to
     * maintain consistency. Input is read through a special
     * function which reads exactly n bytes into a buffer to
     * prevent buffer overflows.
     */
    memset(k, 0, sizeof k);
    prompt_input("Input your key > ", (char*)k, sizeof k);
    puts("Your key that you entered");
    print_hex(k, sizeof k);
    putchar('\n');

    mlen = prompt_input("Input your message > ", (char*)m, sizeof m);
    putchar('\n');

    printf("Generating %s authentication...\n", crypto_auth_primitive());
    crypto_auth(a, m, mlen, k);

    puts("Format: authentication token::message");
    print_hex(a, sizeof a);
    fputs("::", stdout);
    puts((const char*)m);
    putchar('\n');

    puts("Verifying authentication...");
    r = crypto_auth_verify(a, m, mlen, k);
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

    return auth() != 0;
}
