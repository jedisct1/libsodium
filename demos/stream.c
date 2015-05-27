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
 * Stream utilizes a nonce to generate a sequence of bytes. The library has
 * an internal function which XOR data and the stream into an encrypted result.
 *
 * Note that this method does not supply authentication. Try secretbox instead.
 *
 * Note that nonce must be different for each message since it provides
 * change between each operation. It should be safe to use a counter
 * instead of purely random data each time.
 */
static int
stream(void)
{
    unsigned char k[crypto_stream_KEYBYTES];   /* secret key */
    unsigned char n[crypto_stream_NONCEBYTES]; /* message nonce */
    unsigned char m[MAX_INPUT_SIZE];           /* plain-text */
    unsigned char c[MAX_INPUT_SIZE];           /* cipher-text */
    size_t mlen;                               /* length */
    int r;

    puts("Example: crypto_stream\n");

    memset(k, 0, sizeof k);
    prompt_input("Input your key > ", (char*)k, sizeof k);
    putchar('\n');

    /* nonce must be generated per message, safe to send with message */
    puts("Generating nonce...");
    randombytes_buf(n, sizeof n);
    fputs("Nonce: ", stdout);
    print_hex(n, sizeof n);
    putchar('\n');
    putchar('\n');

    mlen = prompt_input("Input your message > ", (char*)m, sizeof m);
    putchar('\n');

    printf("Encrypting with (xor) %s\n", crypto_stream_primitive());
    crypto_stream_xor(c, m, mlen, n, k);
    putchar('\n');

    puts("Sending message...");
    puts("Format: nonce::message");
    fputs("Ciphertext: ", stdout);
    print_hex(n, sizeof n);
    fputs("::", stdout);
    print_hex(c, mlen);
    putchar('\n');
    putchar('\n');

    puts("Opening message...");
    r = crypto_stream_xor(m, c, mlen, n, k);

    print_verification(r);
    if (r == 0)
        printf("Plaintext: %s\n\n", m);

    sodium_memzero(k, sizeof k); /* wipe sensitive data */
    sodium_memzero(m, sizeof m);
    sodium_memzero(c, sizeof c);
    return r;
}

int
main(void)
{
    sodium_init();
    printf("Using LibSodium %s\n", sodium_version_string());

    return stream() != 0;
}
