#include <stdio.h>
#include <string.h>
#include "schnorr.h"

int main() {
    unsigned char pk[33], sk[32], sig[65];
    unsigned long long siglen = 65;
    
    // Keygen
    crypto_sign_schnorr_keypair(pk, sk);
    printf("PK[0]: %02x, SK[0]: %02x\n", pk[0], sk[0]);
    
    // Sign "test"
    const char *msg = "test";
    crypto_sign_schnorr(sig, &siglen, (unsigned char*)msg, 4, sk);
    printf("Sig[0..7]: %02x%02x%02x%02x%02x%02x%02x%02x\n",
           sig[0], sig[1], sig[2], sig[3], sig[4], sig[5], sig[6], sig[7]);
    printf("Siglen: %llu\n", siglen);
    
    // Verify with SAME key and message
    unsigned char msg_out[128];
    unsigned long long msg_out_len = 4;  // strlen("test")
    
    int ret = crypto_sign_schnorr_open(msg_out, &msg_out_len, sig, siglen, pk);
    printf("Verify result: %d\n", ret);
    printf("msg_out_len after: %llu\n", msg_out_len);
    
    // Manual verify check
    printf("\n=== MANUAL CHECK ===\n");
    printf("sig[0] (R): %02x\n", sig[0]);
    printf("sig[1..32]: R compressed\n");
    printf("sig[33..64]: s value\n");
    printf("pk[0] (Y): %02x\n", pk[0]);
    
    return ret;
}
