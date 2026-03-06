#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "vectors/x25519_vectors.h"

int
main(void)
{
    unsigned char pk[crypto_scalarmult_curve25519_BYTES];
    unsigned char sk[crypto_scalarmult_curve25519_SCALARBYTES];
    unsigned char shared_exp[crypto_scalarmult_curve25519_BYTES];
    unsigned char shared_out[crypto_scalarmult_curve25519_BYTES];
    size_t        pk_len, sk_len;
    int           passed = 0, failed = 0, skipped = 0;
    size_t        i;

    if (sodium_init() < 0) {
        return 1;
    }

    for (i = 0; i < WYCHEPROOF_X25519_COUNT; i++) {
        const struct wycheproof_x25519_test *t = &wycheproof_tests[i];
        int is_valid      = (strcmp(t->result, "valid") == 0);
        int is_acceptable = (strcmp(t->result, "acceptable") == 0);
        int ret;

        pk_len = strlen(t->public_key)  / 2;
        sk_len = strlen(t->private_key) / 2;

        if (pk_len != crypto_scalarmult_curve25519_BYTES ||
            sk_len != crypto_scalarmult_curve25519_SCALARBYTES) {
            if (is_valid) {
                printf("FAIL tc=%d: bad key sizes\n", t->tc_id);
                failed++;
            } else {
                passed++;
            }
            continue;
        }

        sodium_hex2bin(pk, sizeof pk,
                       t->public_key, strlen(t->public_key),
                       NULL, NULL, NULL);
        sodium_hex2bin(sk, sizeof sk,
                       t->private_key, strlen(t->private_key),
                       NULL, NULL, NULL);
        sodium_hex2bin(shared_exp, sizeof shared_exp,
                       t->shared, strlen(t->shared),
                       NULL, NULL, NULL);

        ret = crypto_scalarmult_curve25519(shared_out, sk, pk);

        if (is_valid) {
            if (ret != 0) {
                printf("FAIL tc=%d: valid DH rejected\n",
                       t->tc_id);
                failed++;
            } else if (memcmp(shared_out, shared_exp,
                              sizeof shared_out) != 0) {
                printf("FAIL tc=%d: shared secret mismatch\n",
                       t->tc_id);
                failed++;
            } else {
                passed++;
            }
        } else if (is_acceptable) {
            /*
             * "acceptable" = implementation-defined.
             * libsodium rejects low-order points but accepts
             * non-canonical ones. Either outcome is fine; if
             * accepted the output must match.
             */
            if (ret == 0 &&
                memcmp(shared_out, shared_exp,
                       sizeof shared_out) != 0) {
                printf("FAIL tc=%d: acceptable mismatch\n",
                       t->tc_id);
                failed++;
            } else {
                passed++;
            }
        } else {
            /* invalid */
            if (ret == 0) {
                printf("FAIL tc=%d: invalid DH accepted\n",
                       t->tc_id);
                failed++;
            } else {
                passed++;
            }
        }
    }

    printf("[x25519] %d passed, %d failed, %d skipped"
           " (of %d)\n",
           passed, failed, skipped,
           (int) WYCHEPROOF_X25519_COUNT);
    return failed > 0 ? 1 : 0;
}
