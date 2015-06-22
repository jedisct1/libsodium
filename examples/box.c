/*
 * GraxRabble
 * example programs for libsodium.
 */

#include <sodium.h> /* library header */

#include "utils.h" /* utility functions shared by examples */

/*
 * Using public-key authenticated encryption, Bob can encrypt a
 * confidential message specifically for Alice, using Alice's public
 * key.
 *
 * Using Bob's public key, Alice can verify that the encrypted
 * message was actually created by Bob and was not tampered with,
 * before eventually decrypting it.
 *
 * Alice only needs Bob's public key, the nonce and the ciphertext.
 * Bob should never ever share his secret key, even with Alice.
 *
 * And in order to send messages to Alice, Bob only needs Alice's
 * public key. Alice should never ever share her secret key either,
 * even with Bob.
 *
 * Alice can reply to Bob using the same system, without having to
 * generate a distinct key pair.
 *
 * The nonce doesn't have to be confidential, but it should be used
 * with just one invokation of crypto_box_open_easy() for a
 * particular pair of public and secret keys.
 *
 * One easy way to generate a nonce is to use randombytes_buf(),
 * considering the size of nonces the risk of any random collisions
 * is negligible. For some applications, if you wish to use nonces to
 * detect missing messages or to ignore replayed messages, it is also
 * ok to use a simple incrementing counter as a nonce.
 *
 * When doing so you must ensure that the same value can never be
 * re-used (for example you may have multiple threads or even hosts
 * generating messages using the same key pairs).
 *
 * This system provides mutual authentication. However, a typical use
 * case is to secure communications between a server, whose public
 * key is known in advance, and clients connecting anonymously.
 */
static int
box(void)
{
    unsigned char bob_pk[crypto_box_PUBLICKEYBYTES]; /* Bob's public key */
    unsigned char bob_sk[crypto_box_SECRETKEYBYTES]; /* Bob's secret key */

    unsigned char alice_pk[crypto_box_PUBLICKEYBYTES]; /* Alice's public key */
    unsigned char alice_sk[crypto_box_SECRETKEYBYTES]; /* Alice's secret key */

    unsigned char nonce[crypto_box_NONCEBYTES];
    unsigned char message[MAX_INPUT_LEN];
    unsigned char ciphertext[crypto_box_MACBYTES + MAX_INPUT_LEN];
    size_t        message_len;
    size_t        ciphertext_len;
    int           ret;

    puts("Example: crypto_box_easy\n");

    puts("Generating keypairs...\n");
    crypto_box_keypair(bob_pk, bob_sk);     /* generate Bob's keys */
    crypto_box_keypair(alice_pk, alice_sk); /* generate Alice's keys */

    puts("Bob");
    printf("Public key: ");
    print_hex(bob_pk, sizeof bob_pk);
    printf("Secret key: ");
    print_hex(bob_sk, sizeof bob_sk);

    puts("Alice");
    printf("Public key: ");
    print_hex(alice_pk, sizeof alice_pk);
    printf("Secret key: ");
    print_hex(alice_sk, sizeof alice_sk);

    /* nonce must be unique per (key, message) - it can be public and deterministic */
    puts("Generating nonce...");
    randombytes_buf(nonce, sizeof nonce);
    printf("Nonce: ");
    print_hex(nonce, sizeof nonce);

    /* read input */
    message_len = prompt_input("a message", (char*)message, sizeof message, 1);

    print_hex(message, message_len);

    /* encrypt and authenticate the message */
    printf("Encrypting and authenticating with %s\n\n", crypto_box_primitive());
    crypto_box_easy(ciphertext, message, message_len, nonce, alice_pk, bob_sk);
    ciphertext_len = crypto_box_MACBYTES + message_len;

    /* send the nonce and the ciphertext */
    puts("Bob sends the nonce and the ciphertext...\n");
    printf("Ciphertext len: %zu bytes - Original message length: %zu bytes\n",
           ciphertext_len, message_len);
    puts("Notice the prepended 16 byte authentication token\n");
    printf("Nonce: ");
    print_hex(nonce, sizeof nonce);
    printf("Ciphertext: ");
    print_hex(ciphertext, ciphertext_len);

    /* decrypt the message */
    puts("Alice verifies and decrypts the ciphertext...");
    ret = crypto_box_open_easy(message, ciphertext, ciphertext_len, nonce, bob_pk,
                               alice_sk);
    print_hex(message, message_len);

    print_verification(ret);
    if (ret == 0) {
        printf("Plaintext: ");
        fwrite(message, 1U, message_len, stdout);
        putchar('\n');
    }
    sodium_memzero(bob_sk, sizeof bob_sk); /* wipe sensitive data */
    sodium_memzero(alice_sk, sizeof alice_sk);
    sodium_memzero(message, sizeof message);
    sodium_memzero(ciphertext, sizeof ciphertext);

    return ret;
}

int
main(void)
{
    init();

    return box() != 0;
}
