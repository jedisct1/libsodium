
#include <stdlib.h>
#include <string.h>
#include "crypto_hash_sha256.h"
#include "crypto_onetimeauth.h"
#include "crypto_onetimeauth_poly1305.h"
#include "crypto_onetimeauth_poly1305_ref.h"
#include "crypto_onetimeauth_poly1305_53.h"
#include "utils.h"

#define MAXTEST_BYTES 10000
#define CHECKSUM_BYTES 4096

#define CHECKSUM "e836d5ca58cf673fca2b4910f23f3990"

static char checksum[crypto_onetimeauth_BYTES * 2U + 1U];

static unsigned char *h,  *h_;
static unsigned char *m,  *m_;
static unsigned char *k,  *k_;
static unsigned char *h2, *h2_;
static unsigned char *m2, *m2_;
static unsigned char *k2, *k2_;

static int
allocate(void)
{
    h  = _sodium_alignedcalloc(&h_, crypto_onetimeauth_BYTES);
    m  = _sodium_alignedcalloc(&m_, MAXTEST_BYTES);
    k  = _sodium_alignedcalloc(&k_, crypto_onetimeauth_KEYBYTES);
    h2 = _sodium_alignedcalloc(&h2_, crypto_onetimeauth_BYTES);
    m2 = _sodium_alignedcalloc(&m2_, MAXTEST_BYTES + crypto_onetimeauth_BYTES);
    k2 = _sodium_alignedcalloc(&k2_, crypto_onetimeauth_KEYBYTES +
                               crypto_onetimeauth_BYTES);

    return -!(h && m && k && h2 && m2 && k2);
}

static void
deallocate(void)
{
    free(h_);
    free(m_);
    free(k_);
    free(h2_);
    free(m2_);
    free(k2_);
}

static const char *
checksum_compute(void)
{
    long long i;
    long long j;

    for (i = 0;i < CHECKSUM_BYTES;++i) {
        long long mlen = i;
        long long klen = crypto_onetimeauth_KEYBYTES;
        long long hlen = crypto_onetimeauth_BYTES;

        for (j = -16;j < 0;++j) h[j] = rand();
        for (j = -16;j < 0;++j) k[j] = rand();
        for (j = -16;j < 0;++j) m[j] = rand();
        for (j = hlen;j < hlen + 16;++j) h[j] = rand();
        for (j = klen;j < klen + 16;++j) k[j] = rand();
        for (j = mlen;j < mlen + 16;++j) m[j] = rand();
        for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
        for (j = -16;j < klen + 16;++j) k2[j] = k[j];
        for (j = -16;j < mlen + 16;++j) m2[j] = m[j];

        if (crypto_onetimeauth(h,m,mlen,k) != 0) return "crypto_onetimeauth returns nonzero";

        for (j = -16;j < klen + 16;++j) if (k[j] != k2[j]) return "crypto_onetimeauth overwrites k";
        for (j = -16;j < mlen + 16;++j) if (m[j] != m2[j]) return "crypto_onetimeauth overwrites m";
        for (j = -16;j < 0;++j) if (h[j] != h2[j]) return "crypto_onetimeauth writes before output";
        for (j = hlen;j < hlen + 16;++j) if (h[j] != h2[j]) return "crypto_onetimeauth writes after output";

        for (j = -16;j < 0;++j) h[j] = rand();
        for (j = -16;j < 0;++j) k[j] = rand();
        for (j = -16;j < 0;++j) m[j] = rand();
        for (j = hlen;j < hlen + 16;++j) h[j] = rand();
        for (j = klen;j < klen + 16;++j) k[j] = rand();
        for (j = mlen;j < mlen + 16;++j) m[j] = rand();
        for (j = -16;j < hlen + 16;++j) h2[j] = h[j];
        for (j = -16;j < klen + 16;++j) k2[j] = k[j];
        for (j = -16;j < mlen + 16;++j) m2[j] = m[j];

        if (crypto_onetimeauth(m2,m2,mlen,k) != 0) return "crypto_onetimeauth returns nonzero";
        for (j = 0;j < hlen;++j) if (m2[j] != h[j]) return "crypto_onetimeauth does not handle m overlap";
        for (j = 0;j < hlen;++j) m2[j] = m[j];
        if (crypto_onetimeauth(k2,m2,mlen,k2) != 0) return "crypto_onetimeauth returns nonzero";
        for (j = 0;j < hlen;++j) if (k2[j] != h[j]) return "crypto_onetimeauth does not handle k overlap";
        for (j = 0;j < hlen;++j) k2[j] = k[j];

        if (crypto_onetimeauth_verify(h,m,mlen,k) != 0) return "crypto_onetimeauth_verify returns nonzero";

        for (j = -16;j < hlen + 16;++j) if (h[j] != h2[j]) return "crypto_onetimeauth overwrites h";
        for (j = -16;j < klen + 16;++j) if (k[j] != k2[j]) return "crypto_onetimeauth overwrites k";
        for (j = -16;j < mlen + 16;++j) if (m[j] != m2[j]) return "crypto_onetimeauth overwrites m";

        crypto_hash_sha256(h2,h,hlen);
        for (j = 0;j < klen;++j) k[j] ^= h2[j % 32];
        if (crypto_onetimeauth(h,m,mlen,k) != 0) return "crypto_onetimeauth returns nonzero";
        if (crypto_onetimeauth_verify(h,m,mlen,k) != 0) return "crypto_onetimeauth_verify returns nonzero";

        crypto_hash_sha256(h2,h,hlen);
        for (j = 0;j < mlen;++j) m[j] ^= h2[j % 32];
        m[mlen] = h2[0];
    }
    if (crypto_onetimeauth(h,m,CHECKSUM_BYTES,k) != 0) return "crypto_onetimeauth returns nonzero";
    if (crypto_onetimeauth_verify(h,m,CHECKSUM_BYTES,k) != 0) return "crypto_onetimeauth_verify returns nonzero";

    sodium_bin2hex(checksum, sizeof checksum, h, crypto_onetimeauth_BYTES);

    return NULL;
}

crypto_onetimeauth_poly1305_implementation *
crypto_onetimeauth_pick_best_implementation(void)
{
    crypto_onetimeauth_poly1305_implementation *implementations[] = {
#ifdef HAVE_FENV_H
        &crypto_onetimeauth_poly1305_53_implementation,
#endif
        &crypto_onetimeauth_poly1305_ref_implementation,
        NULL
    };
    const char *err;
    size_t      i = (size_t) 0U;

    do {
        if (crypto_onetimeauth_poly1305_set_implementation
            (implementations[i]) != 0) {
            continue;
        }
        if (allocate() != 0) {
            return NULL;
        }
        err = checksum_compute();
        deallocate();
        if (err == NULL && strcmp(checksum, CHECKSUM) == 0) {
            break;
        }
    } while (implementations[++i] != NULL);

    return implementations[i];
}
