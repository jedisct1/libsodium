/*
 * Shared AEAD test runner for Wycheproof vectors.
 *
 * Include after defining:
 *   AEAD_KEYBYTES, AEAD_NPUBBYTES, AEAD_ABYTES
 *   AEAD_ENCRYPT(c, clen_p, m, mlen, ad, adlen, nsec, npub, k)
 *   AEAD_DECRYPT(m, mlen_p, nsec, c, clen, ad, adlen, npub, k)
 *   TEST_NAME  (string for diagnostics)
 *   Wycheproof vectors header (defines wycheproof_tests[], WYCHEPROOF_AEAD_COUNT)
 */

#ifndef AEAD_TESTS_H
#define AEAD_TESTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sodium.h>

#define MAX_TEST_BUF 1024

static int
run_aead_tests(void)
{
    unsigned char key[AEAD_KEYBYTES];
    unsigned char nonce[AEAD_NPUBBYTES];
    unsigned char aad[MAX_TEST_BUF];
    unsigned char msg[MAX_TEST_BUF];
    unsigned char ct_exp[MAX_TEST_BUF];
    unsigned char tag_exp[AEAD_ABYTES];
    unsigned char combined[MAX_TEST_BUF + AEAD_ABYTES];
    unsigned char ct_out[MAX_TEST_BUF + AEAD_ABYTES];
    unsigned char pt_out[MAX_TEST_BUF];
    size_t        aad_len, msg_len, ct_len, tag_len;
    unsigned long long out_len;
    int           passed = 0, failed = 0, skipped = 0;
    size_t        i;

    for (i = 0; i < WYCHEPROOF_AEAD_COUNT; i++) {
        const struct wycheproof_aead_test *t = &wycheproof_tests[i];
        int is_valid = (strcmp(t->result, "valid") == 0);

        msg_len = strlen(t->msg) / 2;
        aad_len = strlen(t->aad) / 2;
        ct_len  = strlen(t->ct)  / 2;
        tag_len = strlen(t->tag) / 2;

        if (msg_len > MAX_TEST_BUF || aad_len > MAX_TEST_BUF ||
            ct_len > MAX_TEST_BUF) {
            printf("SKIP tc=%d: field too large\n", t->tc_id);
            skipped++;
            continue;
        }

        sodium_hex2bin(key, sizeof key, t->key, strlen(t->key),
                       NULL, NULL, NULL);
        sodium_hex2bin(nonce, sizeof nonce, t->iv, strlen(t->iv),
                       NULL, NULL, NULL);
        sodium_hex2bin(aad, sizeof aad, t->aad, strlen(t->aad),
                       NULL, &aad_len, NULL);
        sodium_hex2bin(msg, sizeof msg, t->msg, strlen(t->msg),
                       NULL, &msg_len, NULL);
        sodium_hex2bin(ct_exp, sizeof ct_exp, t->ct, strlen(t->ct),
                       NULL, &ct_len, NULL);
        sodium_hex2bin(tag_exp, sizeof tag_exp, t->tag, strlen(t->tag),
                       NULL, &tag_len, NULL);

        /* Build ct||tag for decryption */
        memcpy(combined, ct_exp, ct_len);
        memcpy(combined + ct_len, tag_exp, tag_len);

        if (is_valid) {
            /* Encrypt and compare ct||tag */
            if (AEAD_ENCRYPT(ct_out, &out_len,
                             msg, (unsigned long long) msg_len,
                             aad, (unsigned long long) aad_len,
                             NULL, nonce, key) != 0) {
                printf("FAIL tc=%d: encrypt error\n", t->tc_id);
                failed++;
                continue;
            }
            if (out_len != ct_len + tag_len ||
                memcmp(ct_out, combined, ct_len + tag_len) != 0) {
                printf("FAIL tc=%d: encrypt mismatch\n", t->tc_id);
                failed++;
                continue;
            }

            /* Decrypt and compare plaintext */
            if (AEAD_DECRYPT(pt_out, &out_len, NULL,
                             combined,
                             (unsigned long long) (ct_len + tag_len),
                             aad, (unsigned long long) aad_len,
                             nonce, key) != 0) {
                printf("FAIL tc=%d: decrypt error\n", t->tc_id);
                failed++;
                continue;
            }
            if (out_len != msg_len ||
                (msg_len > 0 && memcmp(pt_out, msg, msg_len) != 0)) {
                printf("FAIL tc=%d: decrypt mismatch\n", t->tc_id);
                failed++;
                continue;
            }
            passed++;
        } else {
            /* Invalid: decrypt must fail */
            if (AEAD_DECRYPT(pt_out, &out_len, NULL,
                             combined,
                             (unsigned long long) (ct_len + tag_len),
                             aad, (unsigned long long) aad_len,
                             nonce, key) == 0) {
                printf("FAIL tc=%d: invalid accepted\n", t->tc_id);
                failed++;
                continue;
            }
            passed++;
        }
    }

    printf("[%s] %d passed, %d failed, %d skipped"
           " (of %d)\n",
           TEST_NAME, passed, failed, skipped,
           (int) WYCHEPROOF_AEAD_COUNT);
    return failed > 0 ? 1 : 0;
}

#endif /* AEAD_TESTS_H */
