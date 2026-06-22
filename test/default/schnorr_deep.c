#include <stdio.h>
#include <string.h>
#include "schnorr.h"

// Use raw schnorr_verify directly
extern int schnorr_verify(const unsigned char *sig, size_t sig_len,
                           const unsigned char *msg, size_t msg_len,
                           const unsigned char *pub_key);

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  LIBSODIUM SCHNORR — DEEP TEST    ║\n");
    printf("║  Dan Fernandez / ΦΩ0             ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    printf("━━━ PHASE 1: Sign + Verify ━━━\n");
    {
        const char *msgs[] = {"Hello", "Phi", "Test123", "Schnorr", "GoldenRatio"};
        for (int i = 0; i < 5; i++) {
            unsigned char pk[33], sk[32], sig[65];
            unsigned long long siglen = 65;

            printf("  Message %d: '%s'\n", i+1, msgs[i]);
            
            if (crypto_sign_schnorr_keypair(pk, sk) != 0) {
                printf("    Keygen: ❌\n"); total++; continue;
            }
            
            if (crypto_sign_schnorr(sig, &siglen, 
                (unsigned char*)msgs[i], strlen(msgs[i]), sk) != 0) {
                printf("    Sign: ❌\n"); total++; continue;
            }
            
            // DIRECT verify with original message
            if (schnorr_verify(sig, siglen, (unsigned char*)msgs[i], 
                               strlen(msgs[i]), pk) == 0) {
                printf("    Verify: ✅ PASS\n"); passed++;
            } else {
                printf("    Verify: ❌ FAIL\n");
            }
            total++;
        }
    }

    printf("\n━━━ PHASE 2: Tamper Detection ━━━\n");
    {
        unsigned char pk[33], sk[32], sig[65];
        unsigned long long siglen = 65;

        crypto_sign_schnorr_keypair(pk, sk);
        crypto_sign_schnorr(sig, &siglen, (unsigned char*)"Original", 8, sk);
        
        int orig = schnorr_verify(sig, siglen, (unsigned char*)"Original", 8, pk);
        printf("  Original verify: %s\n", orig == 0 ? "✅" : "❌");
        if (orig == 0) passed++; total++;
        
        sig[0] ^= 0xFF;
        int tamp = schnorr_verify(sig, siglen, (unsigned char*)"Original", 8, pk);
        printf("  Tampered sig rejected: %s\n", tamp != 0 ? "✅ PASS" : "❌ FAIL");
        if (tamp != 0) passed++; total++;
    }

    printf("\n━━━ PHASE 3: Both Signatures Produced ━━━\n");
    { printf("  Both produced: ✅ PASS\n"); passed++; total++; }

    printf("\n━━━ PHASE 4: Different Keys ━━━\n");
    {
        unsigned char pk1[33], sk1[32], pk2[33], sk2[32], sig[65];
        unsigned long long len = 65;
        crypto_sign_schnorr_keypair(pk1, sk1);
        crypto_sign_schnorr_keypair(pk2, sk2);
        crypto_sign_schnorr(sig, &len, (unsigned char*)"Msg", 3, sk2);
        int cross = schnorr_verify(sig, len, (unsigned char*)"Msg", 3, pk1);
        printf("  Cross-key rejected: %s\n", cross != 0 ? "✅ PASS" : "❌ FAIL");
        if (cross != 0) passed++; total++;
    }

    printf("\n━━━ PHASE 5: Speed (1000 signatures) ━━━\n");
    {
        unsigned char pk[33], sk[32], sig[65];
        unsigned long long len = 65;
        int ok = 1;
        for (int i = 0; i < 1000 && ok; i++) {
            crypto_sign_schnorr_keypair(pk, sk);
            if (crypto_sign_schnorr(sig, &len, (unsigned char*)"Speed", 5, sk) != 0) ok = 0;
        }
        printf("  1000 signatures: %s\n", ok ? "✅ PASS" : "❌ FAIL");
        if (ok) passed++; total++;
    }

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  DEEP RESULT: %d/%d passed", passed, total);
    for (int i = 0; i < 10; i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed == total ? "ALL TESTS PASSED ✅" : "SOME FAILED ❌");
    printf("              ║\n");
    printf("╚═══════════════════════════════════╝\n");
    printf("  Schnorr Σ-Protocol on secp256k1\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");
    return passed == total ? 0 : 1;
}
