/*
 * AEGIS-256 based on https://bench.cr.yp.to/supercop/supercop-20190816.tar.xz
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_aead_aegis256.h"
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
crypto_aead_aegis256_update(aes_block_t *const state, const aes_block_t data)
{
    aes_block_t tmp;

    tmp      = AES_ENC(state[5], state[0]);
    state[5] = AES_ENC(state[4], state[5]);
    state[4] = AES_ENC(state[3], state[4]);
    state[3] = AES_ENC(state[2], state[3]);
    state[2] = AES_ENC(state[1], state[2]);
    state[1] = AES_ENC(state[0], state[1]);
    state[0] = AES_BLOCK_XOR(tmp, data);
}

static void
crypto_aead_aegis256_init(const unsigned char *key, const unsigned char *nonce,
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
    aes_block_t       k1, k2;
    aes_block_t       kxn1, kxn2;
    int               i;

    k1   = AES_BLOCK_LOAD((const aes_block_t *) (const void *) &key[0]);
    k2   = AES_BLOCK_LOAD((const aes_block_t *) (const void *) &key[16]);
    kxn1 = AES_BLOCK_XOR(k1, AES_BLOCK_LOAD((aes_block_t *) (void *) &nonce[0]));
    kxn2 = AES_BLOCK_XOR(k2, AES_BLOCK_LOAD((aes_block_t *) (void *) &nonce[16]));

    state[0] = kxn1;
    state[1] = kxn2;
    state[2] = c0;
    state[3] = c1;
    state[4] = AES_BLOCK_XOR(k1, c1);
    state[5] = AES_BLOCK_XOR(k2, c0);

    for (i = 0; i < 4; i++) {
        crypto_aead_aegis256_update(state, k1);
        crypto_aead_aegis256_update(state, k2);
        crypto_aead_aegis256_update(state, kxn1);
        crypto_aead_aegis256_update(state, kxn2);
    }
}

static void
crypto_aead_aegis256_mac(unsigned char *mac, unsigned long long adlen, unsigned long long mlen,
                         aes_block_t *const state)
{
    aes_block_t tmp;
    int         i;

    tmp = AES_BLOCK_LOAD_64x2(mlen << 3, adlen << 3);
    tmp = AES_BLOCK_XOR(tmp, state[3]);

    for (i = 0; i < 7; i++) {
        crypto_aead_aegis256_update(state, tmp);
    }

    tmp = AES_BLOCK_XOR(state[5], state[4]);
    tmp = AES_BLOCK_XOR(tmp, state[3]);
    tmp = AES_BLOCK_XOR(tmp, state[2]);
    tmp = AES_BLOCK_XOR(tmp, state[1]);
    tmp = AES_BLOCK_XOR(tmp, state[0]);

    AES_BLOCK_STORE((aes_block_t *) (void *) mac, tmp);
}

static inline void
crypto_aead_aegis256_absorb(const unsigned char *const src, aes_block_t *const state)
{
    aes_block_t msg;

    msg = AES_BLOCK_LOAD(src);
    crypto_aead_aegis256_update(state, msg);
}

static void
crypto_aead_aegis256_enc(unsigned char *const dst, const unsigned char *const src,
                         aes_block_t *const state)
{
    aes_block_t msg;
    aes_block_t tmp;

    msg = AES_BLOCK_LOAD((const aes_block_t *) (const void *) src);
    tmp = AES_BLOCK_XOR(msg, state[5]);
    tmp = AES_BLOCK_XOR(tmp, state[4]);
    tmp = AES_BLOCK_XOR(tmp, state[1]);
    tmp = AES_BLOCK_XOR(tmp, AES_BLOCK_AND(state[2], state[3]));
    AES_BLOCK_STORE((aes_block_t *) (void *) dst, tmp);

    crypto_aead_aegis256_update(state, msg);
}

static void
crypto_aead_aegis256_dec(unsigned char *const dst, const unsigned char *const src,
                         aes_block_t *const state)
{
    aes_block_t msg;

    msg = AES_BLOCK_LOAD((const aes_block_t *) (const void *) src);
    msg = AES_BLOCK_XOR(msg, state[5]);
    msg = AES_BLOCK_XOR(msg, state[4]);
    msg = AES_BLOCK_XOR(msg, state[1]);
    msg = AES_BLOCK_XOR(msg, AES_BLOCK_AND(state[2], state[3]));
    AES_BLOCK_STORE((aes_block_t *) (void *) dst, msg);

    crypto_aead_aegis256_update(state, msg);
}

int
crypto_aead_aegis256_encrypt_detached(unsigned char *c, unsigned char *mac,
                                      unsigned long long *maclen_p, const unsigned char *m,
                                      unsigned long long mlen, const unsigned char *ad,
                                      unsigned long long adlen, const unsigned char *nsec,
                                      const unsigned char *npub, const unsigned char *k)
{
    aes_block_t                    state[6];
    CRYPTO_ALIGN(16) unsigned char src[16];
    CRYPTO_ALIGN(16) unsigned char dst[16];
    unsigned long long             i;

    (void) nsec;
    crypto_aead_aegis256_init(k, npub, state);

    for (i = 0ULL; i + 16ULL <= adlen; i += 16ULL) {
        crypto_aead_aegis256_absorb(ad + i, state);
    }
    if (adlen & 0xf) {
        memset(src, 0, 16);
        memcpy(src, ad + i, adlen & 0xf);
        crypto_aead_aegis256_absorb(src, state);
    }
    for (i = 0ULL; i + 16ULL <= mlen; i += 16ULL) {
        crypto_aead_aegis256_enc(c + i, m + i, state);
    }
    if (mlen & 0xf) {
        memset(src, 0, 16);
        memcpy(src, m + i, mlen & 0xf);
        crypto_aead_aegis256_enc(dst, src, state);
        memcpy(c + i, dst, mlen & 0xf);
    }

    crypto_aead_aegis256_mac(mac, adlen, mlen, state);
    sodium_memzero(state, sizeof state);
    sodium_memzero(src, sizeof src);
    sodium_memzero(dst, sizeof dst);

    if (maclen_p != NULL) {
        *maclen_p = 16ULL;
    }
    return 0;
}

int
crypto_aead_aegis256_encrypt(unsigned char *c, unsigned long long *clen_p, const unsigned char *m,
                             unsigned long long mlen, const unsigned char *ad,
                             unsigned long long adlen, const unsigned char *nsec,
                             const unsigned char *npub, const unsigned char *k)
{
    unsigned long long clen = 0ULL;
    int                ret;

    if (mlen > crypto_aead_aegis256_MESSAGEBYTES_MAX) {
        sodium_misuse();
    }
    ret =
        crypto_aead_aegis256_encrypt_detached(c, c + mlen, NULL, m, mlen, ad, adlen, nsec, npub, k);
    if (clen_p != NULL) {
        if (ret == 0) {
            clen = mlen + 16ULL;
        }
        *clen_p = clen;
    }
    return ret;
}

int
crypto_aead_aegis256_decrypt_detached(unsigned char *m, unsigned char *nsec, const unsigned char *c,
                                      unsigned long long clen, const unsigned char *mac,
                                      const unsigned char *ad, unsigned long long adlen,
                                      const unsigned char *npub, const unsigned char *k)
{
    aes_block_t                    state[6];
    CRYPTO_ALIGN(16) unsigned char src[16];
    CRYPTO_ALIGN(16) unsigned char dst[16];
    CRYPTO_ALIGN(16) unsigned char computed_mac[16];
    unsigned long long             i;
    unsigned long long             mlen;
    int                            ret;

    (void) nsec;
    mlen = clen;
    crypto_aead_aegis256_init(k, npub, state);

    for (i = 0ULL; i + 16ULL <= adlen; i += 16ULL) {
        crypto_aead_aegis256_absorb(ad + i, state);
    }
    if (adlen & 0xf) {
        memset(src, 0, 16);
        memcpy(src, ad + i, adlen & 0xf);
        crypto_aead_aegis256_absorb(src, state);
    }
    if (m != NULL) {
        for (i = 0ULL; i + 16ULL <= mlen; i += 16ULL) {
            crypto_aead_aegis256_dec(m + i, c + i, state);
        }
    } else {
        for (i = 0ULL; i + 16ULL <= mlen; i += 16ULL) {
            crypto_aead_aegis256_dec(dst, c + i, state);
        }
    }
    if (mlen & 0xf) {
        memset(src, 0, 16);
        memcpy(src, c + i, mlen & 0xf);
        crypto_aead_aegis256_dec(dst, src, state);
        if (m != NULL) {
            memcpy(m + i, dst, mlen & 0xf);
        }
        memset(dst, 0, mlen & 0xf);
        state[0] =
            AES_BLOCK_XOR(state[0], AES_BLOCK_LOAD((const aes_block_t *) (const void *) dst));
    }

    crypto_aead_aegis256_mac(computed_mac, adlen, mlen, state);
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
crypto_aead_aegis256_decrypt(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec,
                             const unsigned char *c, unsigned long long clen,
                             const unsigned char *ad, unsigned long long adlen,
                             const unsigned char *npub, const unsigned char *k)
{
    unsigned long long mlen = 0ULL;
    int                ret  = -1;

    if (clen >= 16ULL) {
        ret = crypto_aead_aegis256_decrypt_detached(m, nsec, c, clen - 16ULL, c + clen - 16ULL, ad,
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
crypto_aead_aegis256_is_available(void)
{
    return sodium_runtime_has_aesni();
}

#endif
