#include <stdio.h>
#include "schnorr.h"

int main() {
    unsigned char pk[33], sk[32];
    unsigned char sig[65];
    unsigned long long siglen = 65;
    
    printf("Keygen...\n");
    if (crypto_sign_schnorr_keypair(pk, sk) != 0) {
        printf("Keygen FAIL\n");
        return 1;
    }
    printf("SK[0..3]: %02x%02x%02x%02x\n", sk[0], sk[1], sk[2], sk[3]);
    printf("PK[0]: %02x\n", pk[0]);
    
    printf("Signing 'test'...\n");
    fflush(stdout);
    
    int ret = crypto_sign_schnorr(sig, &siglen, (unsigned char*)"test", 4, sk);
    printf("Sign result: %d\n", ret);
    
    return 0;
}
