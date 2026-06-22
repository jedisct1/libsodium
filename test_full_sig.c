#include <stdio.h>
#include <string.h>
#include "schnorr.h"

int main() {
    unsigned char pk[33], sk[32], sig[65];
    unsigned long long siglen = 65;
    
    crypto_sign_schnorr_keypair(pk, sk);
    crypto_sign_schnorr(sig, &siglen, (unsigned char*)"test", 4, sk);
    
    printf("Full signature (65 bytes):\n");
    for (int i = 0; i < 65; i++) {
        printf("%02x", sig[i]);
        if ((i+1) % 16 == 0) printf("\n");
    }
    printf("\n\n");
    
    printf("R (first 33 bytes):\n");
    for (int i = 0; i < 33; i++) printf("%02x", sig[i]);
    printf("\n");
    
    printf("s (last 32 bytes):\n");
    for (int i = 33; i < 65; i++) printf("%02x", sig[i]);
    printf("\n");
    
    // Manual verify
    printf("\nTrying manual verify...\n");
    unsigned char msg_out[128];
    unsigned long long msg_out_len = 4;
    int ret = crypto_sign_schnorr_open(msg_out, &msg_out_len, sig, 65, pk);
    printf("Verify result: %d\n", ret);
    
    return 0;
}
