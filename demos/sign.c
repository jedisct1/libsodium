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
 * Signs a message with secret key which will authenticate a message.
 * Everybody else can use the public key to ensure that the message is both
 * valid and untampered.
 *
 * Note that both message and signed message must be padded for signature.
 * The padding does not have to be set to zero.
 */
static int
sign(void)
{
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];       /* Bob public */
    unsigned char sk[crypto_sign_SECRETKEYBYTES];       /* Bob secret */
    unsigned char m[BUFFER_SIZE];                       /* message */
    unsigned char sm[BUFFER_SIZE + crypto_sign_BYTES];  /* signed message */
    unsigned long long int mlen;                        /* message length */
    unsigned long long int smlen;                       /* signed length */
    int r;

    puts("Example: crypto_sign\n");

    puts("Generating keypair...");
    crypto_sign_keypair(pk, sk);            /* generate Bob's keys */

    fputs("Public: ", stdout);
    print_hex(pk, sizeof pk);
    putc('\n', stdout);
    fputs("Secret: ", stdout);
    print_hex(sk, sizeof sk);
    puts("\n");

    /* read input */
    mlen = prompt_input("Input your message > ",
            (char*) m, sizeof m - crypto_sign_BYTES);
    putc('\n', stdout);
    
    puts("Notice the message has no prepended padding");
    print_hex(m, mlen);
    putchar('\n');
    putchar('\n');

    printf("Signing message with %s...\n", crypto_sign_primitive());
    crypto_sign(sm, &smlen, m, mlen, sk);
    
    puts("Notice the signed message has prepended signature");
    print_hex(sm, smlen);
    putchar('\n');
    putchar('\n');
    
    puts("Format: signature::message");
    fputs("Signed: ", stdout);
    print_hex(sm, crypto_sign_BYTES);
    fputs("::", stdout);
    puts((const char*) sm + crypto_sign_BYTES);
    putc('\n', stdout);

    puts("Validating message...");
    r = crypto_sign_open(m, &mlen, sm, smlen, pk);

    print_verification(r);
    if (r == 0) printf("Message: %s\n\n", m);
    
    sodium_memzero(pk, sizeof pk);          /* wipe sensitive data */
    sodium_memzero(sk, sizeof sk);
    sodium_memzero(m, sizeof m);
    sodium_memzero(sm, sizeof sm);
    return r;
}

int
main(int argc, char **argv)
{
    int r;

    sodium_init();
    printf("Using LibSodium %s\n", sodium_version_string());

    r = (0 == sign() ? EXIT_SUCCESS : EXIT_FAILURE);
    exit(r);
}

