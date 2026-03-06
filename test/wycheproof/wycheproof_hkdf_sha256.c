#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "vectors/hkdf_sha256_vectors.h"

#define MAX_BUF 8192

int
main(void)
{
    unsigned char ikm[MAX_BUF], salt[MAX_BUF], info[MAX_BUF];
    unsigned char okm_exp[MAX_BUF], okm_out[MAX_BUF];
    unsigned char prk[crypto_kdf_hkdf_sha256_KEYBYTES];
    size_t        ikm_len, salt_len, info_len;
    int           passed = 0, failed = 0, skipped = 0;
    size_t        i;

    if (sodium_init() < 0) {
        return 1;
    }

    for (i = 0; i < WYCHEPROOF_HKDF_COUNT; i++) {
        const struct wycheproof_hkdf_test *t = &wycheproof_tests[i];
        int is_valid = (strcmp(t->result, "valid") == 0);
        int ret;

        ikm_len  = strlen(t->ikm)  / 2;
        salt_len = strlen(t->salt) / 2;
        info_len = strlen(t->info) / 2;

        if (ikm_len > MAX_BUF || salt_len > MAX_BUF ||
            info_len > MAX_BUF ||
            (size_t) t->size > MAX_BUF) {
            printf("SKIP tc=%d: field too large\n", t->tc_id);
            skipped++;
            continue;
        }

        sodium_hex2bin(ikm, sizeof ikm, t->ikm, strlen(t->ikm),
                       NULL, &ikm_len, NULL);
        sodium_hex2bin(salt, sizeof salt, t->salt, strlen(t->salt),
                       NULL, &salt_len, NULL);
        sodium_hex2bin(info, sizeof info, t->info, strlen(t->info),
                       NULL, &info_len, NULL);
        sodium_hex2bin(okm_exp, sizeof okm_exp,
                       t->okm, strlen(t->okm), NULL, NULL, NULL);

        /* Extract */
        ret = crypto_kdf_hkdf_sha256_extract(
            prk,
            salt_len > 0 ? salt : NULL, salt_len,
            ikm, ikm_len);

        if (ret != 0) {
            if (!is_valid) {
                passed++;
            } else {
                printf("FAIL tc=%d: extract error\n", t->tc_id);
                failed++;
            }
            continue;
        }

        /* Expand — ctx is const char* in libsodium API */
        ret = crypto_kdf_hkdf_sha256_expand(
            okm_out, (size_t) t->size,
            (const char *) info, info_len, prk);

        if (is_valid) {
            if (ret != 0) {
                printf("FAIL tc=%d: expand error\n", t->tc_id);
                failed++;
            } else if (memcmp(okm_out, okm_exp,
                              (size_t) t->size) != 0) {
                printf("FAIL tc=%d: OKM mismatch\n", t->tc_id);
                failed++;
            } else {
                passed++;
            }
        } else {
            if (ret == 0) {
                printf("FAIL tc=%d: invalid expand accepted\n",
                       t->tc_id);
                failed++;
            } else {
                passed++;
            }
        }
    }

    printf("[hkdf_sha256] %d passed, %d failed, %d skipped"
           " (of %d)\n",
           passed, failed, skipped,
           (int) WYCHEPROOF_HKDF_COUNT);
    return failed > 0 ? 1 : 0;
}
