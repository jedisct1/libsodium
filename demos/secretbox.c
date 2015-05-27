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
 * This is a wrapper around stream which does XOR automatically.
 *
 * Note that the buffer must be padded at the front. The same nonce must
 * not be used; it should be safe to use a counter.
 *
 * Encrypted messages will be 16 bytes longer because a 16 byte
 * authentication token will be prepended to the message.
 */
static int
secretbox(void)
{
    unsigned char k[crypto_secretbox_KEYBYTES];                /* secret */
    unsigned char n[crypto_secretbox_NONCEBYTES];              /* nonce */
    unsigned char m[BUFFER_SIZE + crypto_secretbox_ZEROBYTES]; /* plain */
    unsigned char c[BUFFER_SIZE + crypto_secretbox_ZEROBYTES]; /* cipher */
    size_t mlen;                                               /* length */
    int r;

    puts("Example: crypto_secretbox\n");

    sodium_memzero(k, sizeof k);
    prompt_input("Input your key > ", (char*)k, sizeof k);

    /* nonce must be generated per message, safe to send with message */
    puts("Generating nonce...");
    randombytes_buf(n, sizeof n);
    fputs("Nonce: ", stdout);
    print_hex(n, sizeof n);
    putchar('\n');
    putchar('\n');

    mlen = prompt_input("Input your message > ",
                        (char*)m + crypto_secretbox_ZEROBYTES,
                        sizeof m - crypto_secretbox_ZEROBYTES);

    /* must zero at least the padding */
    sodium_memzero(m, crypto_secretbox_ZEROBYTES);

    puts("Notice the 32 bytes of zero");
    print_hex(m, mlen + crypto_box_ZEROBYTES);
    putchar('\n');

    /* encrypting message */
    printf("Encrypting with %s\n", crypto_secretbox_primitive());

    crypto_secretbox(c, m, mlen + crypto_secretbox_ZEROBYTES, n, k);
    putchar('\n');

    puts("Notice the prepended 16 byte authentication token");
    puts("Sending message...");
    puts("Format: nonce::message");
    fputs("Ciphertext: ", stdout);
    print_hex(n, sizeof n);
    fputs("::", stdout);
    print_hex(c, mlen + crypto_secretbox_ZEROBYTES);
    putchar('\n');
    putchar('\n');

    /* decrypting message */
    puts("Opening message...");

    /* must zero at least the padding */
    sodium_memzero(c, crypto_secretbox_BOXZEROBYTES);
    r = crypto_secretbox_open(m, c, mlen + crypto_secretbox_ZEROBYTES, n, k);

    puts("Notice the 32 bytes of zero");
    print_hex(m, mlen + crypto_box_ZEROBYTES);
    putchar('\n');
    putchar('\n');

    print_verification(r);
    if (r == 0)
        printf("Plaintext: %s\n\n", m + crypto_secretbox_ZEROBYTES);

    sodium_memzero(k, sizeof k); /* wipe sensitive data */
    sodium_memzero(n, sizeof n);
    sodium_memzero(m, sizeof m);
    sodium_memzero(c, sizeof c);
    return r;
}

int
main(void)
{
    sodium_init();
    printf("Using LibSodium %s\n", sodium_version_string());

    return secretbox() != 0;
}
