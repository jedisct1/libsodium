#include <stdio.h>
#include <string.h>
#include "schnorr.h"

int main() {
    unsigned char pk[33], sk[32], sig[65];
    unsigned long long siglen = 65;
    
    printf("Keygen...\n"); fflush(stdout);
    if (crypto_sign_schnorr_keypair(pk, sk) != 0) {
        printf("Keygen FAIL\n");
        return 1;
    }
    printf("Keygen OK\n"); fflush(stdout);
    
    // Call sign directly with NULL check bypass
    printf("Calling schnorr_sign...\n"); fflush(stdout);
    
    // Use the raw schnorr_sign from schnorr.c
    extern int schnorr_sign(const unsigned char *msg, size_t msg_len,
                            const unsigned char *priv_key,
                            unsigned char *sig, size_t *sig_len);
    
    size_t len = 65;
    int ret = schnorr_sign((const unsigned char*)"test", 4, sk, sig, &len);
    printf("schnorr_sign returned: %d, len: %zu\n", ret, len);
    fflush(stdout);
    
    return 0;
}
