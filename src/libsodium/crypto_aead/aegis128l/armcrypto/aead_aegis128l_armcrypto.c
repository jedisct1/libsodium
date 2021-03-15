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

#ifdef HAVE_ARMCRYPTO

# include <arm_neon.h>

static inline void
crypto_aead_aegis128l_update(uint8x16_t *const state,
                             const uint8x16_t d1, const uint8x16_t d2)
{
    const uint8x16_t zero = vmovq_n_u8(0);
    uint8x16_t       tmp, tmp2;

    tmp      = state[7];
    state[7] = veorq_u8(vaesmcq_u8(vaeseq_u8(state[6], zero)), state[7]);
    state[6] = veorq_u8(vaesmcq_u8(vaeseq_u8(state[5], zero)), state[6]);
    state[5] = veorq_u8(vaesmcq_u8(vaeseq_u8(state[4], zero)), state[5]);
    state[4] = veorq_u8(vaesmcq_u8(vaeseq_u8(state[3], zero)), state[4]);
    state[3] = veorq_u8(vaesmcq_u8(vaeseq_u8(state[2], zero)), state[3]);
    state[2] = veorq_u8(vaesmcq_u8(vaeseq_u8(state[1], zero)), state[2]);
    state[1] = veorq_u8(vaesmcq_u8(vaeseq_u8(state[0], zero)), state[1]);
    state[0] = veorq_u8(vaesmcq_u8(vaeseq_u8(tmp, zero)), state[0]);

    state[0] = veorq_u8(state[0], d1);
    state[4] = veorq_u8(state[4], d2);
}

static void
crypto_aead_aegis128l_init(const unsigned char *key, const unsigned char *nonce,
                           uint8x16_t *const state)
{
    static CRYPTO_ALIGN(16) const unsigned char c1_[] = {
        0xdb, 0x3d, 0x18, 0x55, 0x6d, 0xc2, 0x2f, 0xf1, 0x20, 0x11, 0x31, 0x42,
        0x73, 0xb5, 0x28, 0xdd
    };
    static CRYPTO_ALIGN(16) const unsigned char c2_[] = {
        0x00, 0x01, 0x01, 0x02, 0x03, 0x05, 0x08, 0x0d, 0x15, 0x22, 0x37, 0x59,
        0x90, 0xe9, 0x79, 0x62
    };
    const uint8x16_t c1 = vld1q_u8(c1_);
    const uint8x16_t c2 = vld1q_u8(c2_);
    uint8x16_t       k;
    uint8x16_t       n;
    int              i;

    k = vld1q_u8(key);
    n = vld1q_u8(nonce);

    state[0] = veorq_u8(k, n);
    state[1] = c1;
    state[2] = c2;
    state[3] = c1;
    state[4] = veorq_u8(k, n);
    state[5] = veorq_u8(k, c2);
    state[6] = veorq_u8(k, c1);
    state[7] = veorq_u8(k, c2);
    for (i = 0; i < 10; i++) {
        crypto_aead_aegis128l_update(state, n, k);
    }
}

static void
crypto_aead_aegis128l_mac(unsigned char *mac, unsigned long long adlen,
                          unsigned long long mlen, uint8x16_t *const state)
{
    uint8x16_t tmp;
    int        i;

    tmp = vreinterpretq_u8_u64(vsetq_lane_u64(mlen << 3,
                                              vmovq_n_u64(adlen << 3), 1));
    tmp = veorq_u8(tmp, state[2]);

    for (i = 0; i < 7; i++) {
        crypto_aead_aegis128l_update(state, tmp, tmp);
    }

    tmp = veorq_u8(state[6], state[5]);
    tmp = veorq_u8(tmp, state[4]);
    tmp = veorq_u8(tmp, state[3]);
    tmp = veorq_u8(tmp, state[2]);
    tmp = veorq_u8(tmp, state[1]);
    tmp = veorq_u8(tmp, state[0]);

    vst1q_u8(mac, tmp);
}

static void
crypto_aead_aegis128l_enc(unsigned char *const dst,
                          const unsigned char *const src,
                          uint8x16_t *const state)
{
    uint8x16_t msg0, msg1;
    uint8x16_t tmp0, tmp1;

    msg0 = vld1q_u8(src);
    msg1 = vld1q_u8(src + 16);
    tmp0 = veorq_u8(msg0, state[6]);
    tmp0 = veorq_u8(tmp0, state[1]);
    tmp1 = veorq_u8(msg1, state[2]);
    tmp1 = veorq_u8(tmp1, state[5]);
    tmp0 = veorq_u8(tmp0, vandq_u8(state[2], state[3]));
    tmp1 = veorq_u8(tmp1, vandq_u8(state[6], state[7]));
    vst1q_u8(dst, tmp0);
    vst1q_u8(dst + 16, tmp1);

    crypto_aead_aegis128l_update(state, msg0, msg1);
}


static void
crypto_aead_aegis128l_dec(unsigned char *const dst,
                          const unsigned char *const src,
                          uint8x16_t *const state)
{
    uint8x16_t msg0, msg1;

    msg0 = vld1q_u8(src);
    msg1 = vld1q_u8(src + 16);
    msg0 = veorq_u8(msg0, state[6]);
    msg0 = veorq_u8(msg0, state[1]);
    msg1 = veorq_u8(msg1, state[2]);
    msg1 = veorq_u8(msg1, state[5]);
    msg0 = veorq_u8(msg0, vandq_u8(state[2], state[3]));
    msg1 = veorq_u8(msg1, vandq_u8(state[6], state[7]));
    vst1q_u8(dst, msg0);
    vst1q_u8(dst + 16, msg1);

    crypto_aead_aegis128l_update(state, msg0, msg1);
}

int
crypto_aead_aegis128l_encrypt_detached(unsigned char *c, unsigned char *mac,
                                       unsigned long long *maclen_p, const unsigned char *m,
                                       unsigned long long mlen, const unsigned char *ad,
                                       unsigned long long adlen, const unsigned char *nsec,
                                       const unsigned char *npub, const unsigned char *k)
{
    uint8x16_t                     state[8];
    CRYPTO_ALIGN(16) unsigned char src[32];
    CRYPTO_ALIGN(16) unsigned char dst[32];
    unsigned long long i;

    (void) nsec;
    crypto_aead_aegis128l_init(k, npub, state);

    for (i = 0ULL; i + 32ULL <= adlen; i += 32ULL) {
        crypto_aead_aegis128l_enc(dst, ad + i, state);
    }
    if (adlen & 0x1f) {
        memset(src, 0, 32);
        memcpy(src, ad + i, adlen & 0x1f);
        crypto_aead_aegis128l_enc(dst, src, state);
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
    ret = crypto_aead_aegis128l_encrypt_detached(c, c + mlen, NULL, m, mlen,
                                                 ad, adlen, nsec, npub, k);
    if (clen_p != NULL) {
        if (ret == 0) {
            clen = mlen + 16ULL;
        }
        *clen_p = clen;
    }
    return ret;
}

int
crypto_aead_aegis128l_decrypt_detached(unsigned char *m, unsigned char *nsec, const unsigned char *c,
                                       unsigned long long clen, const unsigned char *mac,
                                       const unsigned char *ad, unsigned long long adlen,
                                       const unsigned char *npub, const unsigned char *k)
{
    uint8x16_t                     state[8];
    CRYPTO_ALIGN(16) unsigned char src[32];
    CRYPTO_ALIGN(16) unsigned char dst[32];
    CRYPTO_ALIGN(16) unsigned char computed_mac[16];
    unsigned long long i;
    unsigned long long mlen;
    int                ret;

    (void) nsec;
    mlen = clen;
    crypto_aead_aegis128l_init(k, npub, state);

    for (i = 0ULL; i + 32ULL <= adlen; i += 32ULL) {
        crypto_aead_aegis128l_enc(dst, ad + i, state);
    }
    if (adlen & 0x1f) {
        memset(src, 0, 32);
        memcpy(src, ad + i, adlen & 0x1f);
        crypto_aead_aegis128l_enc(dst, src, state);
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
        state[0] = veorq_u8(state[0], vld1q_u8(dst));
        state[4] = veorq_u8(state[4], vld1q_u8(dst + 16));
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
        ret = crypto_aead_aegis128l_decrypt_detached
            (m, nsec, c, clen - 16ULL, c + clen - 16ULL, ad, adlen, npub, k);
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
    return sodium_runtime_has_armcrypto();
}

#endif
