/*
 * AEGIS-128l based on https://bench.cr.yp.to/supercop/supercop-20200409.tar.xz
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_aead_aegis128l.h"
#include "crypto_verify_16.h"
#include "export.h"
#include "randombytes.h"
#include "runtime.h"
#include "utils.h"

#include "private/common.h"

#if defined(HAVE_TMMINTRIN_H) && defined(HAVE_WMMINTRIN_H)

#ifdef __GNUC__
#pragma GCC target("ssse3")
#pragma GCC target("aes")
#endif

#include "private/sse2_64_32.h"
#include <tmmintrin.h>
#include <wmmintrin.h>

typedef __m128i aes_block_t;
#define AES_BLOCK_XOR(A, B)       _mm_xor_si128((A), (B))
#define AES_BLOCK_AND(A, B)       _mm_and_si128((A), (B))
#define AES_BLOCK_LOAD(A)         _mm_loadu_si128((const aes_block_t *) (const void *) (A))
#define AES_BLOCK_LOAD_64x2(A, B) _mm_set_epi64x((A), (B))
#define AES_BLOCK_STORE(A, B)     _mm_storeu_si128((aes_block_t *) (void *) (A), (B))
#define AES_ENC(A, B)             _mm_aesenc_si128((A), (B))

static inline void
crypto_aead_aegis128l_update(aes_block_t *const state, const aes_block_t d1, const aes_block_t d2)
{
    aes_block_t tmp;

    tmp      = state[7];
    state[7] = AES_ENC(state[6], state[7]);
    state[6] = AES_ENC(state[5], state[6]);
    state[5] = AES_ENC(state[4], state[5]);
    state[4] = AES_ENC(state[3], state[4]);
    state[3] = AES_ENC(state[2], state[3]);
    state[2] = AES_ENC(state[1], state[2]);
    state[1] = AES_ENC(state[0], state[1]);
    state[0] = AES_ENC(tmp, state[0]);

    state[0] = AES_BLOCK_XOR(state[0], d1);
    state[4] = AES_BLOCK_XOR(state[4], d2);
}

static void
crypto_aead_aegis128l_init(const unsigned char *key, const unsigned char *nonce,
                           aes_block_t *const state)
{
    static CRYPTO_ALIGN(16)
        const uint8_t c0_[] = { 0xdb, 0x3d, 0x18, 0x55, 0x6d, 0xc2, 0x2f, 0xf1,
                                0x20, 0x11, 0x31, 0x42, 0x73, 0xb5, 0x28, 0xdd };
    static CRYPTO_ALIGN(16)
        const uint8_t c1_[] = { 0x00, 0x01, 0x01, 0x02, 0x03, 0x05, 0x08, 0x0d,
                                0x15, 0x22, 0x37, 0x59, 0x90, 0xe9, 0x79, 0x62 };
    const aes_block_t c0    = AES_BLOCK_LOAD(c0_);
    const aes_block_t c1    = AES_BLOCK_LOAD(c1_);
    aes_block_t       k;
    aes_block_t       n;
    int               i;

    k = AES_BLOCK_LOAD((const aes_block_t *) (const void *) key);
    n = AES_BLOCK_LOAD((const aes_block_t *) (const void *) nonce);

    state[0] = AES_BLOCK_XOR(k, n);
    state[1] = c0;
    state[2] = c1;
    state[3] = c0;
    state[4] = AES_BLOCK_XOR(k, n);
    state[5] = AES_BLOCK_XOR(k, c1);
    state[6] = AES_BLOCK_XOR(k, c0);
    state[7] = AES_BLOCK_XOR(k, c1);
    for (i = 0; i < 10; i++) {
        crypto_aead_aegis128l_update(state, n, k);
    }
}

static void
crypto_aead_aegis128l_mac(unsigned char *mac, unsigned long long adlen, unsigned long long mlen,
                          aes_block_t *const state)
{
    aes_block_t tmp;
    int         i;

    tmp = AES_BLOCK_LOAD_64x2(mlen << 3, adlen << 3);
    tmp = AES_BLOCK_XOR(tmp, state[2]);

    for (i = 0; i < 7; i++) {
        crypto_aead_aegis128l_update(state, tmp, tmp);
    }

    tmp = AES_BLOCK_XOR(state[6], state[5]);
    tmp = AES_BLOCK_XOR(tmp, state[4]);
    tmp = AES_BLOCK_XOR(tmp, state[3]);
    tmp = AES_BLOCK_XOR(tmp, state[2]);
    tmp = AES_BLOCK_XOR(tmp, state[1]);
    tmp = AES_BLOCK_XOR(tmp, state[0]);

    AES_BLOCK_STORE((aes_block_t *) (void *) mac, tmp);
}

static inline void
crypto_aead_aegis128l_absorb(const unsigned char *const src, aes_block_t *const state)
{
    aes_block_t msg0, msg1;

    msg0 = AES_BLOCK_LOAD(src);
    msg1 = AES_BLOCK_LOAD(src + 16);
    crypto_aead_aegis128l_update(state, msg0, msg1);
}

static void
crypto_aead_aegis128l_enc(unsigned char *const dst, const unsigned char *const src,
                          aes_block_t *const state)
{
    aes_block_t msg0, msg1;
    aes_block_t tmp0, tmp1;

    msg0 = AES_BLOCK_LOAD((const aes_block_t *) (const void *) src);
    msg1 = AES_BLOCK_LOAD((const aes_block_t *) (const void *) (src + 16));
    tmp0 = AES_BLOCK_XOR(msg0, state[6]);
    tmp0 = AES_BLOCK_XOR(tmp0, state[1]);
    tmp1 = AES_BLOCK_XOR(msg1, state[2]);
    tmp1 = AES_BLOCK_XOR(tmp1, state[5]);
    tmp0 = AES_BLOCK_XOR(tmp0, AES_BLOCK_AND(state[2], state[3]));
    tmp1 = AES_BLOCK_XOR(tmp1, AES_BLOCK_AND(state[6], state[7]));
    AES_BLOCK_STORE((aes_block_t *) (void *) dst, tmp0);
    AES_BLOCK_STORE((aes_block_t *) (void *) (dst + 16), tmp1);

    crypto_aead_aegis128l_update(state, msg0, msg1);
}

static void
crypto_aead_aegis128l_dec(unsigned char *const dst, const unsigned char *const src,
                          aes_block_t *const state)
{
    aes_block_t msg0, msg1;

    msg0 = AES_BLOCK_LOAD((const aes_block_t *) (const void *) src);
    msg1 = AES_BLOCK_LOAD((const aes_block_t *) (const void *) (src + 16));
    msg0 = AES_BLOCK_XOR(msg0, state[6]);
    msg0 = AES_BLOCK_XOR(msg0, state[1]);
    msg1 = AES_BLOCK_XOR(msg1, state[2]);
    msg1 = AES_BLOCK_XOR(msg1, state[5]);
    msg0 = AES_BLOCK_XOR(msg0, AES_BLOCK_AND(state[2], state[3]));
    msg1 = AES_BLOCK_XOR(msg1, AES_BLOCK_AND(state[6], state[7]));
    AES_BLOCK_STORE((aes_block_t *) (void *) dst, msg0);
    AES_BLOCK_STORE((aes_block_t *) (void *) (dst + 16), msg1);

    crypto_aead_aegis128l_update(state, msg0, msg1);
}

int
crypto_aead_aegis128l_encrypt_detached(unsigned char *c, unsigned char *mac,
                                       unsigned long long *maclen_p, const unsigned char *m,
                                       unsigned long long mlen, const unsigned char *ad,
                                       unsigned long long adlen, const unsigned char *nsec,
                                       const unsigned char *npub, const unsigned char *k)
{
    aes_block_t                    state[8];
    CRYPTO_ALIGN(16) unsigned char src[32];
    CRYPTO_ALIGN(16) unsigned char dst[32];
    unsigned long long             i;

    (void) nsec;
    crypto_aead_aegis128l_init(k, npub, state);

    for (i = 0ULL; i + 32ULL <= adlen; i += 32ULL) {
        crypto_aead_aegis128l_absorb(ad + i, state);
    }
    if (adlen & 0x1f) {
        memset(src, 0, 32);
        memcpy(src, ad + i, adlen & 0x1f);
        crypto_aead_aegis128l_absorb(src, state);
    }
    for (i = 0ULL; i + 32ULL <= mlen; i += 32ULL) {
        crypto_aead_aegis128l_enc(c + i, m + i, state);
    }
    if (mlen & 0x1f) {
        memset(src, 0, 32);
        memcpy(src, m + i, mlen & 0x1f);
        crypto_aead_aegis128l_enc(dst, src, state);
        memcpy(c + i, dst, mlen & 0x1f);
    }

    crypto_aead_aegis128l_mac(mac, adlen, mlen, state);
    sodium_memzero(state, sizeof state);
    sodium_memzero(src, sizeof src);
    sodium_memzero(dst, sizeof dst);

    if (maclen_p != NULL) {
        *maclen_p = 16ULL;
    }
    return 0;
}

int
crypto_aead_aegis128l_encrypt(unsigned char *c, unsigned long long *clen_p, const unsigned char *m,
                              unsigned long long mlen, const unsigned char *ad,
                              unsigned long long adlen, const unsigned char *nsec,
                              const unsigned char *npub, const unsigned char *k)
{
    unsigned long long clen = 0ULL;
    int                ret;

    if (mlen > crypto_aead_aegis128l_MESSAGEBYTES_MAX) {
        sodium_misuse();
    }
    ret = crypto_aead_aegis128l_encrypt_detached(c, c + mlen, NULL, m, mlen, ad, adlen, nsec, npub,
                                                 k);
    if (clen_p != NULL) {
        if (ret == 0) {
            clen = mlen + 16ULL;
        }
        *clen_p = clen;
    }
    return ret;
}

int
crypto_aead_aegis128l_decrypt_detached(unsigned char *m, unsigned char *nsec,
                                       const unsigned char *c, unsigned long long clen,
                                       const unsigned char *mac, const unsigned char *ad,
                                       unsigned long long adlen, const unsigned char *npub,
                                       const unsigned char *k)
{
    aes_block_t                    state[8];
    CRYPTO_ALIGN(16) unsigned char src[32];
    CRYPTO_ALIGN(16) unsigned char dst[32];
    CRYPTO_ALIGN(16) unsigned char computed_mac[16];
    unsigned long long             i;
    unsigned long long             mlen;
    int                            ret;

    (void) nsec;
    mlen = clen;
    crypto_aead_aegis128l_init(k, npub, state);

    for (i = 0ULL; i + 32ULL <= adlen; i += 32ULL) {
        crypto_aead_aegis128l_absorb(ad + i, state);
    }
    if (adlen & 0x1f) {
        memset(src, 0, 32);
        memcpy(src, ad + i, adlen & 0x1f);
        crypto_aead_aegis128l_absorb(src, state);
    }
    if (m != NULL) {
        for (i = 0ULL; i + 32ULL <= mlen; i += 32ULL) {
            crypto_aead_aegis128l_dec(m + i, c + i, state);
        }
    } else {
        for (i = 0ULL; i + 32ULL <= mlen; i += 32ULL) {
            crypto_aead_aegis128l_dec(dst, c + i, state);
        }
    }
    if (mlen & 0x1f) {
        memset(src, 0, 32);
        memcpy(src, c + i, mlen & 0x1f);
        crypto_aead_aegis128l_dec(dst, src, state);
        if (m != NULL) {
            memcpy(m + i, dst, mlen & 0x1f);
        }
        memset(dst, 0, mlen & 0x1f);
        state[0] =
            AES_BLOCK_XOR(state[0], AES_BLOCK_LOAD((const aes_block_t *) (const void *) dst));
        state[4] = AES_BLOCK_XOR(state[4],
                                 AES_BLOCK_LOAD((const aes_block_t *) (const void *) (dst + 16)));
    }

    crypto_aead_aegis128l_mac(computed_mac, adlen, mlen, state);
    sodium_memzero(state, sizeof state);
    sodium_memzero(src, sizeof src);
    sodium_memzero(dst, sizeof dst);
    ret = crypto_verify_16(computed_mac, mac);
    sodium_memzero(computed_mac, sizeof computed_mac);
    if (m == NULL) {
        return ret;
    }
    if (ret != 0) {
        memset(m, 0, mlen);
        return -1;
    }
    return 0;
}

int
crypto_aead_aegis128l_decrypt(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec,
                              const unsigned char *c, unsigned long long clen,
                              const unsigned char *ad, unsigned long long adlen,
                              const unsigned char *npub, const unsigned char *k)
{
    unsigned long long mlen = 0ULL;
    int                ret  = -1;

    if (clen >= 16ULL) {
        ret = crypto_aead_aegis128l_decrypt_detached(m, nsec, c, clen - 16ULL, c + clen - 16ULL, ad,
                                                     adlen, npub, k);
    }
    if (mlen_p != NULL) {
        if (ret == 0) {
            mlen = clen - 16ULL;
        }
        *mlen_p = mlen;
    }
    return ret;
}

int
crypto_aead_aegis128l_is_available(void)
{
    return sodium_runtime_has_aesni();
}

#endif
