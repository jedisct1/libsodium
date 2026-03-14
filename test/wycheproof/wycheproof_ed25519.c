#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "vectors/ed25519_vectors.h"

int
main(void)
{
    unsigned char pk[crypto_sign_ed25519_PUBLICKEYBYTES];
    unsigned char sig[crypto_sign_ed25519_BYTES];
    unsigned char msg[4096];
    size_t        pk_len, sig_hex_len, msg_len;
    int           passed = 0, failed = 0, skipped = 0;
    size_t        i;

    if (sodium_init() < 0) {
        return 1;
    }

    for (i = 0; i < WYCHEPROOF_EDDSA_COUNT; i++) {
        const struct wycheproof_eddsa_test *t = &wycheproof_tests[i];
        int is_valid = (strcmp(t->result, "valid") == 0);
        int ret;

        pk_len = strlen(t->pk) / 2;
        sig_hex_len = strlen(t->sig) / 2;
        msg_len = strlen(t->msg) / 2;

        if (pk_len != crypto_sign_ed25519_PUBLICKEYBYTES) {
            if (is_valid) {
                printf("FAIL tc=%d: bad pk length %zu\n",
                       t->tc_id, pk_len);
                failed++;
            } else {
                passed++;
            }
            continue;
        }
        if (sig_hex_len != crypto_sign_ed25519_BYTES) {
            /* Non-standard signature length must be invalid */
            if (is_valid) {
                printf("FAIL tc=%d: bad sig length %zu"
                       " marked valid\n",
                       t->tc_id, sig_hex_len);
                failed++;
            } else {
                passed++;
            }
            continue;
        }
        if (msg_len > sizeof msg) {
            printf("SKIP tc=%d: msg too large\n", t->tc_id);
            skipped++;
            continue;
        }

        sodium_hex2bin(pk, sizeof pk, t->pk, strlen(t->pk),
                       NULL, NULL, NULL);
        sodium_hex2bin(sig, sizeof sig, t->sig, strlen(t->sig),
                       NULL, NULL, NULL);
        sodium_hex2bin(msg, sizeof msg, t->msg, strlen(t->msg),
                       NULL, &msg_len, NULL);

        ret = crypto_sign_ed25519_verify_detached(sig, msg,
                                                  msg_len, pk);
        if (is_valid && ret != 0) {
            printf("FAIL tc=%d: valid sig rejected\n", t->tc_id);
            failed++;
        } else if (!is_valid && ret == 0) {
            printf("FAIL tc=%d: invalid sig accepted\n", t->tc_id);
            failed++;
        } else {
            passed++;
        }
    }

    printf("[ed25519] %d passed, %d failed, %d skipped"
           " (of %d)\n",
           passed, failed, skipped,
           (int) WYCHEPROOF_EDDSA_COUNT);
    return failed > 0 ? 1 : 0;
}
