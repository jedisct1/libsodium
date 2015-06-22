/*
 * GraxRabble
 * example programs for libsodium.
 */

#include <sodium.h> /* library header */

#include "utils.h" /* utility functions shared by examples */

/*
 * Signs a message with secret key which will authenticate a message.
 * Everybody else can use the public key to ensure that the message is both
 * valid and untampered.
 *
 * Note that the signed message will have 16 bytes of signature prepended.
 * Ensure that the signed buffer is at least crypto_sign_BYTES longer then
 * the actual message.
 */
static int
sign(void)
{
    unsigned char      pk[crypto_sign_PUBLICKEYBYTES]; /* Bob's public key */
    unsigned char      sk[crypto_sign_SECRETKEYBYTES]; /* Bob's secret key */
    unsigned char      message[MAX_INPUT_LEN];
    unsigned char      message_signed[crypto_sign_BYTES + MAX_INPUT_LEN];
    unsigned long long message_len;
    unsigned long long message_signed_len;
    int                ret;

    puts("Example: crypto_sign\n");

    puts("Generating keypair...");
    crypto_sign_keypair(pk, sk); /* generate Bob's keys */

    printf("Public key: ");
    print_hex(pk, sizeof pk);
    printf("Secret key: ");
    print_hex(sk, sizeof sk);
    puts("The secret key, as returned by crypto_sign_keypair(), actually includes "
         "a copy of the public key, in order to avoid a scalar multiplication "
         "when signing messages.");

    message_len = prompt_input("a message", (char*)message, sizeof message, 1);

    printf("Signing message with %s...\n", crypto_sign_primitive());
    crypto_sign(message_signed, &message_signed_len, message, message_len, sk);

    printf("Signed message:");
    print_hex(message_signed, message_signed_len);
    printf("A %u bytes signature was prepended to the message\n",
           crypto_sign_BYTES);

    printf("Signature: ");
    print_hex(message_signed, crypto_sign_BYTES);
    printf("Message: ");
    fwrite(message_signed + crypto_sign_BYTES, 1U,
           message_signed_len - crypto_sign_BYTES, stdout);
    putchar('\n');

    puts("Validating message...");
    ret = crypto_sign_open(message, &message_len, message_signed,
                           message_signed_len, pk);
    print_verification(ret);
    if (ret == 0)
        printf("Message: %s\n", message);

    sodium_memzero(sk, sizeof sk); /* wipe sensitive data */

    return ret;
}

int
main(void)
{
    init();

    return sign() != 0;
}
