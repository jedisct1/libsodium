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
    unsigned char key[crypto_auth_KEYBYTES];
    unsigned char mac[crypto_auth_BYTES];
    unsigned char message[MAX_INPUT_SIZE];
    size_t        message_len;
    int           ret;

    puts("Example: crypto_auth\n");

    memset(key, 0, sizeof key);
    prompt_input("Enter a key > ", (char*)key, sizeof key);
    puts("Complete key:");
    print_hex(key, sizeof key);
    putchar('\n');

    message_len = prompt_input("Enter a message > ",
                               (char*)message, sizeof message);
    putchar('\n');

    printf("Generating %s authentication...\n", crypto_auth_primitive());
    crypto_auth(mac, message, message_len, key);

    puts("Format: authentication tag::message");
    print_hex(mac, sizeof mac);
    fputs("::", stdout);
    puts((const char*)message);
    putchar('\n');

    puts("Verifying authentication tag...");
    ret = crypto_auth_verify(mac, message, message_len, key);
    print_verification(ret);

    sodium_memzero(key, sizeof key); /* wipe sensitive data */
    sodium_memzero(mac, sizeof mac);
    sodium_memzero(message, sizeof message);

    return ret;
}

int
main(void)
{
    init();

    return auth() != 0;
}
