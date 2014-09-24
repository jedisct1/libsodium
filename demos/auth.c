/*
 * GraxRabble
 * 05 May 2014
 * Demo programs for libsodium.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sodium.h>             /* library header */

#include "demo_utils.h"         /* utility functions shared by demos */



/*
 * Full featured authentication which is used to verify that the message
 * comes from the expected person. It should be safe to keep the same key
 * for multiple messages.
 */
static int
auth(void)
{
    unsigned char k[crypto_auth_KEYBYTES];      /* key */
    unsigned char a[crypto_auth_BYTES];         /* authentication token */
    unsigned char m[BUFFER_SIZE];               /* message */
    size_t mlen;                                /* message length */
    int r;

    sodium_memzero(k, sizeof k);                /* must zero the key */

    puts("Example: crypto_auth\n");

    prompt_input("Input your key > ", (char*) k, sizeof k);
    mlen = prompt_input("Input your message > ", (char*) m, sizeof m);
    putchar('\n');

    printf("Generating %s authentication...\n", crypto_auth_primitive());
    crypto_auth(a, m, mlen, k);

    puts("Format: authentication token::message");
    print_hex(a, sizeof a);
    fputs("::", stdout);
    puts((const char*) m);
    putchar('\n');

    puts("Verifying authentication...");
    r = crypto_auth_verify(a, m, mlen, k);
    print_verification(r);

    sodium_memzero(k, sizeof k);    /* wipe sensitive data */
    sodium_memzero(a, sizeof a);
    sodium_memzero(m, sizeof m);
    return r;
}

int
main(int argc, char **argv)
{
    int r;

    sodium_init();
    printf("Using LibSodium %s\n", sodium_version_string());

    r = (0 == auth() ? EXIT_SUCCESS : EXIT_FAILURE);
    exit(r);
}

