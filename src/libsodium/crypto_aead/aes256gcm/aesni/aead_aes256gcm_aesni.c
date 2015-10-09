
#include <immintrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "crypto_aead_aes256gcm_aesni.h"
#include "utils.h"

#define AES_BLOCKSIZE  16
#define AES_MAXROUNDS  14
#define GMAC_BLOCKSIZE 16

typedef CRYPTO_ALIGN(128) struct context {
    __m128i ekey[AES_MAXROUNDS + 1];
    unsigned char initial_state[GMAC_BLOCKSIZE];
    unsigned char state[GMAC_BLOCKSIZE];
    unsigned char subkey[GMAC_BLOCKSIZE];
} context;

static inline void
_u64_be_from_ull(unsigned char out[8U], unsigned long long x)
{
    out[7] = (unsigned char) (x & 0xff); x >>= 8;
    out[6] = (unsigned char) (x & 0xff); x >>= 8;
    out[5] = (unsigned char) (x & 0xff); x >>= 8;
    out[4] = (unsigned char) (x & 0xff); x >>= 8;
    out[3] = (unsigned char) (x & 0xff); x >>= 8;
    out[2] = (unsigned char) (x & 0xff); x >>= 8;
    out[1] = (unsigned char) (x & 0xff); x >>= 8;
    out[0] = (unsigned char) (x & 0xff);
}

#define KEY_EXPANSION_A \
    tmp1 = _mm_shuffle_epi32(tmp1, 255); \
    tmp3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(tmp3), \
                                           _mm_castsi128_ps(tmp0), 16)); \
    tmp0 = _mm_xor_si128(tmp0, tmp3); \
    tmp3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(tmp3), \
                                           _mm_castsi128_ps(tmp0), 140)); \
    tmp0 = _mm_xor_si128(_mm_xor_si128(tmp0, tmp3), tmp1); \
    _mm_store_si128((void *) key_ptr, tmp0); \
    key_ptr++

#define KEY_EXPANSION_B \
    tmp1 = _mm_shuffle_epi32(tmp1, 170); \
    tmp3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(tmp3), \
                                           _mm_castsi128_ps(tmp2), 16)); \
    tmp2 = _mm_xor_si128(tmp2, tmp3); \
    tmp3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(tmp3), \
                                           _mm_castsi128_ps(tmp2), 140)); \
    tmp2 = _mm_xor_si128(_mm_xor_si128(tmp2, tmp3), tmp1); \
    _mm_store_si128((void *) key_ptr, tmp2); \
    key_ptr++

#define KEY_EXPANSION_BLOCK_1(rcon) \
    tmp1 = _mm_aeskeygenassist_si128(tmp0, (rcon)); KEY_EXPANSION_A

#define KEY_EXPANSION_BLOCK_2(rcon) \
    tmp1 = _mm_aeskeygenassist_si128(tmp2, rcon); KEY_EXPANSION_A; \
    tmp1 = _mm_aeskeygenassist_si128(tmp0, rcon); KEY_EXPANSION_B

#define KEY_EXPANSION_LAST(rcon) \
    tmp1 = _mm_aeskeygenassist_si128(tmp2, 64); KEY_EXPANSION_A;

static void
_key_setup(context * const ctx, const unsigned char *key)
{
    __m128i *key_ptr = &ctx->ekey[0];
    __m128i  tmp0 = _mm_loadu_si128((const void *) key);
    __m128i  tmp1, tmp2;
    __m128i  tmp3 = _mm_setzero_si128();

    *key_ptr++ = tmp0;
    switch (crypto_aead_aes256gcm_KEYBYTES) {
    case 16:
        KEY_EXPANSION_BLOCK_1(1);
        KEY_EXPANSION_BLOCK_1(2);
        KEY_EXPANSION_BLOCK_1(4);
        KEY_EXPANSION_BLOCK_1(8);
        KEY_EXPANSION_BLOCK_1(16);
        KEY_EXPANSION_BLOCK_1(32);
        KEY_EXPANSION_BLOCK_1(64);
        KEY_EXPANSION_BLOCK_1(128);
        KEY_EXPANSION_BLOCK_1(27);
        KEY_EXPANSION_BLOCK_1(54);
        break;
    case 32:
        tmp2 = _mm_loadu_si128((const void *) (key + 16));
        _mm_store_si128((void *) key_ptr, tmp2);
        key_ptr++;
        KEY_EXPANSION_BLOCK_2(1);
        KEY_EXPANSION_BLOCK_2(2);
        KEY_EXPANSION_BLOCK_2(4);
        KEY_EXPANSION_BLOCK_2(8);
        KEY_EXPANSION_BLOCK_2(16);
        KEY_EXPANSION_BLOCK_2(32);
        KEY_EXPANSION_LAST(64);
        break;
    default:
        abort();
    }
}

#define AESNI_INC \
    ctr = _mm_add_epi64(ctr, inc); \
    ctr_low++; \
    if (ctr_low == 0U) { \
        inc = _mm_slli_si128(inc, 8); \
        ctr = _mm_add_epi64(ctr, inc); \
        inc = _mm_srli_si128(inc, 8); \
    } \
    iv = ctr; \
    iv = _mm_shuffle_epi8(iv, mask)

static const unsigned char
swap_mask[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

static void
_aes_ctr(context *ctx, unsigned char *dst, const unsigned char *src, size_t len,
         unsigned char *ivc_block)
{
    __m128i *key_ptr;
    __m128i  ctr;
    __m128i  in1, in2, in3, in4;
    __m128i  iv;
    __m128i  key;
    __m128i  mask;
    __m128i  state1, state2, state3, state4;

    iv = ctr = _mm_loadu_si128((void *) ivc_block);
    iv = _mm_slli_si128(iv, 8);
    mask = _mm_loadu_si128((const void *) swap_mask);
    ctr = _mm_shuffle_epi8(ctr, mask);
    uint32_t ctr_low = (uint32_t) 1;
    __m128i inc = _mm_cvtsi32_si128((int32_t) ctr_low);
    ctr_low = (uint32_t) _mm_cvtsi128_si32(ctr);

#define ENC4(offset) \
    key = _mm_loadu_si128((void *) (key_ptr + (offset))); \
    state1 = _mm_aesenc_si128(state1, key); \
    state2 = _mm_aesenc_si128(state2, key); \
    state3 = _mm_aesenc_si128(state3, key); \
    state4 = _mm_aesenc_si128(state4, key)

#define ENC4_LAST(offset) \
    key = _mm_loadu_si128((void *) (key_ptr + (offset))); \
    state1 = _mm_aesenclast_si128(state1, key); \
    state2 = _mm_aesenclast_si128(state2, key); \
    state3 = _mm_aesenclast_si128(state3, key); \
    state4 = _mm_aesenclast_si128(state4, key)

    while (len >= 64) {
        AESNI_INC;
        state1 = iv;
        in1 = _mm_loadu_si128((const void *) src);
        AESNI_INC;
        state2 = iv;
        in2 = _mm_loadu_si128((const void *) (src + 16));
        AESNI_INC;
        state3 = iv;
        in3 = _mm_loadu_si128((const void *) (src + 32));
        AESNI_INC;
        state4 = iv;
        in4 = _mm_loadu_si128((const void *) (src + 48));
        key = _mm_loadu_si128((void *) &ctx->ekey[0]);
        key_ptr = &ctx->ekey[0];
        state1 = _mm_xor_si128(state1, key);
        state2 = _mm_xor_si128(state2, key);
        state3 = _mm_xor_si128(state3, key);
        state4 = _mm_xor_si128(state4, key);
        key_ptr += 3;
        switch (crypto_aead_aes256gcm_KEYBYTES) {
        case 32:
            key_ptr += 4;
            ENC4(-6);
            ENC4(-5);
        case 16:
            ENC4(-4);
            ENC4(-3);
            ENC4(-2);
            ENC4(-1);
            ENC4(0);
            ENC4(1);
            ENC4(2);
            ENC4(3);
            ENC4(4);
            ENC4(5);
            ENC4(6);
            ENC4_LAST(7);
            break;
        default:
            abort();
        }
        state1 = _mm_xor_si128(state1, in1);
        _mm_storeu_si128((void *) dst, state1);
        state2 = _mm_xor_si128(state2, in2);
        _mm_storeu_si128((void *) (dst + 16), state2);
        state3 = _mm_xor_si128(state3, in3);
        _mm_storeu_si128((void *) (dst + 32), state3);
        state4 = _mm_xor_si128(state4, in4);
        _mm_storeu_si128((void *) (dst + 48), state4);
        len -= 64;
        src += 64;
        dst += 64;
    }

#define ENC(offset) \
    key = _mm_loadu_si128((void *) (key_ptr + (offset))); \
    state1 = _mm_aesenc_si128(state1, key)

#define ENC_LAST(offset) \
    key = _mm_loadu_si128((void *) (key_ptr + (offset))); \
    state1 = _mm_aesenclast_si128(state1, key);

#define ENC_ONE \
    key_ptr = &ctx->ekey[0]; \
    key = _mm_loadu_si128((void *) &ctx->ekey[0]); \
    state1 = _mm_xor_si128(state1, key); \
    key_ptr += 3; \
    switch (crypto_aead_aes256gcm_KEYBYTES) { \
    case 32: \
        key_ptr += 4; \
        ENC(-6); \
        ENC(-5); \
        ENC(-4); \
        ENC(-3); \
    case 16: \
        ENC(-2); \
        ENC(-1); \
        ENC(0); \
        ENC(1); \
        ENC(2); \
        ENC(3); \
        ENC(4); \
        ENC(5); \
        ENC(6); \
        ENC_LAST(7); \
        break; \
    default: \
        abort(); \
    }
    while (len >= 16) {
        AESNI_INC;
        state1 = iv;
        in1 = _mm_loadu_si128((const void *) src);
        ENC_ONE;
        state1 = _mm_xor_si128(state1, in1);
        _mm_storeu_si128((void *) dst, state1);
        len -= 16;
        src += 16;
        dst += 16;
    }
    if (len > 0) {
        unsigned char padded[16];
        memset(padded, 0, sizeof padded);
        memcpy(padded, src, len);
        src = padded;
        AESNI_INC;
        state1 = iv;
        in1 = _mm_loadu_si128((const void *) src);
        ENC_ONE;
        state1 = _mm_xor_si128(state1, in1);
        _mm_storeu_si128((void *) padded, state1);
        memcpy(dst, padded, len);
    }
    _mm_storel_epi64((void *) ivc_block, iv);
}

static void
_aes_enc_one(context *ctx, unsigned char *dst, unsigned char *src)
{
    __m128i *key_ptr;
    __m128i  key;
    __m128i  state1 = _mm_loadu_si128((const void *) src);

    ENC_ONE;
    _mm_storeu_si128((void *) dst, state1);
}

#define GMAC_UPDATE \
    tmp2 = _mm_loadu_si128((const void *) src); \
    tmp2 = _mm_shuffle_epi8(tmp2, mask); \
    tmp0 = _mm_xor_si128(tmp6, tmp2); \
    \
    tmp4 = _mm_xor_si128(_mm_clmulepi64_si128(tmp0, tmp1, 0x10), \
                         _mm_clmulepi64_si128(tmp0, tmp1, 0x01)); \
    tmp5 = _mm_slli_si128(tmp4, 8); \
    tmp3 = _mm_xor_si128(_mm_clmulepi64_si128(tmp0, tmp1, 0x00), tmp5); \
    tmp6 = _mm_xor_si128(_mm_clmulepi64_si128(tmp0, tmp1, 0x11), \
                         _mm_srli_si128(tmp4, 8)); \
    tmp7 = _mm_srli_epi32(tmp3, 31); \
    tmp8 = _mm_srli_epi32(tmp6, 31); \
    tmp3 = _mm_slli_epi32(tmp3, 1); \
    tmp6 = _mm_slli_epi32(tmp6, 1); \
    tmp8 = _mm_slli_si128(tmp8, 4); \
    tmp9 = _mm_srli_si128(tmp7, 12); \
    tmp7 = _mm_slli_si128(tmp7, 4); \
    tmp3 = _mm_or_si128(tmp3, tmp7); \
    tmp6 = _mm_or_si128(_mm_or_si128(tmp6, tmp8), tmp9); \
    \
    tmp8 = _mm_slli_epi32(tmp3, 30); \
    tmp9 = _mm_slli_epi32(tmp3, 25); \
    tmp7 = _mm_xor_si128(_mm_xor_si128(_mm_slli_epi32(tmp3, 31), tmp8), tmp9); \
    tmp3 = _mm_xor_si128(tmp3, _mm_slli_si128(tmp7, 12)); \
    \
    tmp4 = _mm_srli_epi32(tmp3, 2); \
    tmp5 = _mm_srli_epi32(tmp3, 7); \
    tmp6 = _mm_xor_si128(tmp6, _mm_xor_si128(tmp3, \
        _mm_xor_si128(_mm_xor_si128(_mm_xor_si128(_mm_srli_epi32(tmp3, 1), tmp4), tmp5), \
                      _mm_srli_si128(tmp7, 4))))

static void
_gmac_update(context *ctx, const unsigned char *src, size_t len)
{
    __m128i mask = _mm_loadu_si128((const void *) swap_mask);
    __m128i tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;

    tmp1 = _mm_shuffle_epi8(_mm_loadu_si128((void *) ctx->subkey), mask);
    tmp6 = _mm_shuffle_epi8(_mm_loadu_si128((void *) ctx->initial_state), mask);
    while (len >= 16) {
        GMAC_UPDATE;
        len -= 16;
        src += 16;
    }
    if (len > 0) {
        unsigned char padded[16];
        memset(padded, 0, sizeof padded);
        memcpy(padded, src, len);
        src = padded;
        GMAC_UPDATE;
    }
    tmp6 = _mm_shuffle_epi8(tmp6, mask);
    _mm_storeu_si128((void *) ctx->state, tmp6);
    _mm_storeu_si128((void *) ctx->initial_state, tmp6);
}

static void
_gmac_final(context *ctx, unsigned char *tag, unsigned char *ivc_block, unsigned char *hashstate)
{
    __m128i *key_ptr;
    __m128i  in;
    __m128i  key;
    __m128i  state1 = _mm_loadu_si128((const void *) ivc_block);

    ENC_ONE;
    in = _mm_loadu_si128((void *) hashstate);
    state1 = _mm_xor_si128(state1, in);
    _mm_storeu_si128((void *) tag, state1);
}

int
crypto_aead_aes256gcm_aesni_beforenm(crypto_aead_aes256gcm_aesni_state *ctx_,
                                     const unsigned char *k)
{
    context *ctx = (context *) ctx_;

    (void) sizeof(int[(sizeof *ctx_) >= (sizeof *ctx) ? 1 : -1]);
    memset(ctx, 0, sizeof *ctx);
    _key_setup(ctx, k);
    _aes_enc_one(ctx, ctx->subkey, ctx->subkey);

    return 0;
}

int
crypto_aead_aes256gcm_aesni_encrypt_afternm(unsigned char *c,
                                            unsigned long long *clen_p,
                                            const unsigned char *m,
                                            unsigned long long mlen,
                                            const unsigned char *ad,
                                            unsigned long long adlen,
                                            const unsigned char *nsec,
                                            const unsigned char *npub,
                                            crypto_aead_aes256gcm_aesni_state *ctx_)
{
    context        *ctx = (context *) ctx_;
    unsigned char  *mac;
    unsigned char   ivc_block[AES_BLOCKSIZE];

    (void) nsec;
    memset(ivc_block, 0, sizeof ivc_block);
    memcpy(ivc_block, npub, crypto_aead_aes256gcm_NPUBBYTES);
    ivc_block[AES_BLOCKSIZE - 1U] = 1U;
    _gmac_update(ctx, ad, adlen);
    _aes_ctr(ctx, c, m, mlen, ivc_block);
    _gmac_update(ctx, c, mlen);
    mac = c + mlen;
    _u64_be_from_ull(mac, adlen * 8ULL);
    _u64_be_from_ull(mac + 8U, mlen * 8ULL);
    _gmac_update(ctx, mac, GMAC_BLOCKSIZE);
    _gmac_final(ctx, mac, ivc_block, ctx->state);
    sodium_memzero(ctx, sizeof *ctx);
    if (clen_p != NULL) {
        *clen_p = mlen + crypto_aead_aes256gcm_ABYTES;
    }
    return 0;
}

int
crypto_aead_aes256gcm_aesni_decrypt_afternm(unsigned char *m,
                                            unsigned long long *mlen_p,
                                            unsigned char *nsec,
                                            const unsigned char *c,
                                            unsigned long long clen,
                                            const unsigned char *ad,
                                            unsigned long long adlen,
                                            const unsigned char *npub,
                                            crypto_aead_aes256gcm_aesni_state *ctx_)
{
    context       *ctx = (context *) ctx_;
    unsigned char  mac[GMAC_BLOCKSIZE];
    unsigned char  ivc_block[AES_BLOCKSIZE];
    size_t         mlen;

    (void) nsec;
    if (mlen_p != NULL) {
        *mlen_p = 0;
    }
    if (clen < crypto_aead_aes256gcm_ABYTES) {
        return -1;
    }
    mlen = clen - crypto_aead_aes256gcm_ABYTES;
    memset(ivc_block, 0, sizeof ivc_block);
    memcpy(ivc_block, npub, crypto_aead_aes256gcm_NPUBBYTES);
    ivc_block[AES_BLOCKSIZE - 1U] = 1U;

    _gmac_update(ctx, ad, adlen);
    _gmac_update(ctx, c, mlen);
    _u64_be_from_ull(mac, adlen * 8ULL);
    _u64_be_from_ull(mac + 8U, mlen * 8ULL);
    _gmac_update(ctx, mac, GMAC_BLOCKSIZE);
    _gmac_final(ctx, mac, ivc_block, ctx->state);
    if (sodium_memcmp(c + mlen, mac, crypto_aead_aes256gcm_ABYTES) != 0) {
        sodium_memzero(ctx, sizeof *ctx);
        return -1;
    }
    _aes_ctr(ctx, m, c, mlen, ivc_block);
    sodium_memzero(ctx, sizeof *ctx);
    if (mlen_p != NULL) {
        *mlen_p = mlen;
    }
    return 0;
}

int
crypto_aead_aes256gcm_aesni_encrypt(unsigned char *c,
                                    unsigned long long *clen_p,
                                    const unsigned char *m,
                                    unsigned long long mlen,
                                    const unsigned char *ad,
                                    unsigned long long adlen,
                                    const unsigned char *nsec,
                                    const unsigned char *npub,
                                    const unsigned char *k)
{
    crypto_aead_aes256gcm_aesni_state ctx;

    crypto_aead_aes256gcm_aesni_beforenm(&ctx, k);

    return crypto_aead_aes256gcm_aesni_encrypt_afternm
        (c, clen_p, m, mlen, ad, adlen, nsec, npub, &ctx);
}

int
crypto_aead_aes256gcm_aesni_decrypt(unsigned char *m,
                                    unsigned long long *mlen_p,
                                    unsigned char *nsec,
                                    const unsigned char *c,
                                    unsigned long long clen,
                                    const unsigned char *ad,
                                    unsigned long long adlen,
                                    const unsigned char *npub,
                                    const unsigned char *k)
{
    crypto_aead_aes256gcm_aesni_state ctx;

    crypto_aead_aes256gcm_aesni_beforenm(&ctx, k);

    return crypto_aead_aes256gcm_aesni_decrypt_afternm
        (m, mlen_p, nsec, c, clen, ad, adlen, npub, &ctx);
}
