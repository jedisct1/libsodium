#include <stdio.h>
#include <string.h>
#include "schnorr.h"

int main() {
    unsigned char pk[crypto_sign_schnorr_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_schnorr_SECRETKEYBYTES];
    unsigned char sig[crypto_sign_schnorr_BYTES];
    unsigned long long siglen = sizeof(sig);

    if (crypto_sign_schnorr_keypair(pk, sk) != 0) {
        printf("Keygen: FAIL\n");
        return 1;
    }

    const char *msg = "Schnorr test message";
    if (crypto_sign_schnorr(sig, &siglen, (unsigned char*)msg, strlen(msg), sk) != 0) {
        printf("Sign: FAIL\n");
        return 1;
    }

    if (crypto_sign_schnorr_verify((unsigned char*)msg, strlen(msg), sig, siglen, pk) == 0) {
        printf("Schnorr: PASS\n");
        return 0;
    } else {
        printf("Schnorr: FAIL\n");
        return 1;
    }
}
