#include <stdio.h>
#include "schnorr.h"

int main() {
    unsigned char pk[33], sk[32];
    unsigned char sig[65];
    unsigned long long siglen = 65;
    
    printf("Testing keygen+sign...\n");
    if (crypto_sign_schnorr_keypair(pk, sk) != 0) {
        printf("Keygen FAIL\n");
        return 1;
    }
    printf("Keygen OK, signing...\n");
    
    if (crypto_sign_schnorr(sig, &siglen, (unsigned char*)"test", 4, sk) == 0) {
        printf("Sign: OK (siglen=%llu)\n", siglen);
    } else {
        printf("Sign: FAIL\n");
    }
    return 0;
}
