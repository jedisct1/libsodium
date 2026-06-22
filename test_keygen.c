#include <stdio.h>
#include "schnorr.h"

int main() {
    unsigned char pk[33], sk[32];
    printf("Testing keygen...\n");
    if (crypto_sign_schnorr_keypair(pk, sk) == 0) {
        printf("Keygen: OK\n");
    } else {
        printf("Keygen: FAIL\n");
    }
    return 0;
}
