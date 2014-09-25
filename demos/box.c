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
 * Shows how crypto_box works using Bob and Alice with a simple message.
 * Both clients must generate their own key pair and swap public key. The
 * library will perform Diffie-Hellman to generate a shared key for
 * symmetric encryption.
 *
 * Encrypted messages will be 16 bytes longer because a 16 byte
 * authentication token will be prepended to the message.
 *
 * Note the same nonce must not be used; it should be safe to use a counter.
 */
static int
box(void)
{
    unsigned char bob_pk[crypto_box_PUBLICKEYBYTES];    /* Bob public */
    unsigned char bob_sk[crypto_box_SECRETKEYBYTES];    /* Bob secret */

    unsigned char alice_pk[crypto_box_PUBLICKEYBYTES];  /* Alice public */
    unsigned char alice_sk[crypto_box_SECRETKEYBYTES];  /* Alice secret */

    unsigned char n[crypto_box_NONCEBYTES];             /* message nonce */
    unsigned char m[BUFFER_SIZE];                       /* plaintext */
    unsigned char c[BUFFER_SIZE + crypto_box_MACBYTES]; /* ciphertext */
    size_t mlen;                                        /* length */
    int r;

    puts("Example: crypto_box_easy\n");

    puts("Generating keypairs...\n");
    crypto_box_keypair(bob_pk, bob_sk);     /* generate Bob's keys */
    crypto_box_keypair(alice_pk, alice_sk); /* generate Alice's keys */

    puts("Bob");
    fputs("Public: ", stdout);
    print_hex(bob_pk, sizeof bob_pk);
    putchar('\n');
    fputs("Secret: ", stdout);
    print_hex(bob_sk, sizeof bob_sk);
    putchar('\n');
    putchar('\n');

    puts("Alice");
    fputs("Public: ", stdout);
    print_hex(alice_pk, sizeof alice_pk);
    putchar('\n');
    fputs("Secret: ", stdout);
    print_hex(alice_sk, sizeof alice_sk);
    putchar('\n');
    putchar('\n');

    /* nonce must be generated per message, safe to send with message */
    puts("Generating nonce...");
    randombytes_buf(n, sizeof n);
    fputs("Nonce: ", stdout);
    print_hex(n, sizeof n);
    putchar('\n');
    putchar('\n');
    
    /* read input */
    mlen = prompt_input("Input your message > ", (char*) m, sizeof m);
    
    puts("Notice there is no padding");
    print_hex(m, mlen);
    putchar('\n');
    putchar('\n');

    /* encrypt the message */
    printf("Encrypting with %s\n\n", crypto_box_primitive());
    crypto_box_easy(c, m, mlen, n, alice_pk, bob_sk);
    
    /* sent message */
    puts("Bob sending message...\n");
    puts("Notice the prepended 16 byte authentication token");
    puts("Format: nonce::message");
    fputs("Ciphertext: ", stdout);
    print_hex(n, sizeof n);
    fputs("::", stdout);
    print_hex(c, mlen + crypto_box_MACBYTES);
    putchar('\n');
    putchar('\n');

    /* decrypt the message */
    puts("Alice opening message...");
    r = crypto_box_open_easy(m, c, mlen + crypto_box_MACBYTES,
            n, bob_pk, alice_sk);

    puts("Notice there is no padding");
    print_hex(m, mlen);
    putchar('\n');

    print_verification(r);
    if (r == 0) printf("Plaintext: %s\n\n", m);

    sodium_memzero(bob_pk, sizeof bob_pk);      /* wipe sensitive data */
    sodium_memzero(bob_sk, sizeof bob_sk);
    sodium_memzero(alice_pk, sizeof alice_pk);
    sodium_memzero(alice_sk, sizeof alice_sk);
    sodium_memzero(n, sizeof n);
    sodium_memzero(m, sizeof m);
    sodium_memzero(c, sizeof c);
    return r;
}

int
main(int argc, char **argv)
{
    int r;

    sodium_init();
    printf("Using LibSodium %s\n", sodium_version_string());

    r = (0 == box() ? EXIT_SUCCESS : EXIT_FAILURE);
    exit(r);
}

