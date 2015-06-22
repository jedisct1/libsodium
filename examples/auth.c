/*
 * GraxRabble
 * example programs for libsodium.
 */

#include <sodium.h> /* library header */

#include "utils.h" /* utility functions shared by examples */

/*
 * This operation computes an authentication tag for a message and a
 * secret key, and provides a way to verify that a given tag is valid
 * for a given message and a key.
 *
 * The function computing the tag deterministic: the same (message,
 * key) tuple will always produce the same output.
 *
 * However, even if the message is public, knowing the key is
 * required in order to be able to compute a valid tag. Therefore,
 * the key should remain confidential. The tag, however, can be
 * public.
 *
 * A typical use case is:
 *
 * - A prepares a message, add an authentication tag, sends it to B
 * - A doesn't store the message
 * - Later on, B sends the message and the authentication tag to A
 * - A uses the authentication tag to verify that it created this message.
 *
 * This operation does not encrypt the message. It only computes and
 * verifies an authentication tag.
 */
static int
auth(void)
{
    unsigned char key[crypto_auth_KEYBYTES];
    unsigned char mac[crypto_auth_BYTES];
    unsigned char message[MAX_INPUT_LEN];
    size_t        message_len;
    int           ret;

    puts("Example: crypto_auth\n");

    prompt_input("a key", (char*)key, sizeof key, 0);
    message_len = prompt_input("a message", (char*)message, sizeof message, 1);

    printf("Generating %s authentication...\n", crypto_auth_primitive());
    crypto_auth(mac, message, message_len, key);

    printf("Authentication tag: ");
    print_hex(mac, sizeof mac);

    puts("Verifying authentication tag...");
    ret = crypto_auth_verify(mac, message, message_len, key);
    print_verification(ret);

    sodium_memzero(key, sizeof key); /* wipe sensitive data */

    return ret;
}

int
main(void)
{
    init();

    return auth() != 0;
}
