#include <stdio.h>
#include <string.h>
#include <sodium.h>

int main(void)
{
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];
    unsigned char sig[crypto_sign_schnorr_BYTES];
    unsigned char msg[64] = "Schnorr test message";
    unsigned long long siglen, msglen;

    if (sodium_init() < 0) return 1;

    crypto_sign_schnorr_keypair(pk, sk);
    crypto_sign_schnorr(sig, &siglen, msg, strlen((char *)msg), sk);

    if (crypto_sign_schnorr_open(msg, &msglen, sig, siglen, pk) == 0) {
        printf("Schnorr: PASS\n");
        return 0;
    } else {
        printf("Schnorr: FAIL\n");
        return 1;
    }
}
