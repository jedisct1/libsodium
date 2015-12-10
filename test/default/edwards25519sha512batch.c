
#define TEST_NAME "edwards25519sha512batch"
#include "cmptest.h"
#include "crypto_sign_edwards25519sha512batch.h"

#define MESSAGE_LEN 16

int main(void)
{
    unsigned char pk[crypto_sign_edwards25519sha512batch_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_edwards25519sha512batch_SECRETKEYBYTES];
    if (crypto_sign_edwards25519sha512batch_keypair(pk, sk) != 0) {
        printf("Failed keypair\n");
    }

    const unsigned char message[MESSAGE_LEN]
        = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa,
            0xbb, 0xcc, 0xdd, 0xee, 0xff };
    unsigned char signed_message[crypto_sign_edwards25519sha512batch_BYTES +
                                 MESSAGE_LEN];
    unsigned long long signed_message_len;
    if (crypto_sign_edwards25519sha512batch(signed_message,
                                            &signed_message_len, message,
                                            MESSAGE_LEN, sk) != 0) {
        printf("Failed sign\n");
    }
    if (signed_message_len != crypto_sign_edwards25519sha512batch_BYTES +
                              MESSAGE_LEN) {
        printf("Failed signed len\n");
    }

    unsigned char unsigned_message[MESSAGE_LEN];
    unsigned long long unsigned_message_len;
    if (crypto_sign_edwards25519sha512batch_open(unsigned_message,
                                                 &unsigned_message_len,
                                                 signed_message,
                                                 signed_message_len,
                                                 pk) != 0) {
        printf("Failed open\n");
    }
    if (unsigned_message_len != MESSAGE_LEN) {
        printf("Failed unsigned len\n");
    }
    if (memcmp(unsigned_message, message, MESSAGE_LEN) != 0) {
        printf("Failed unsigned compare\n");
    }

    return 0;
}
