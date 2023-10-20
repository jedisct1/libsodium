#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_aead_aes256gcm.h"
#include "crypto_verify_16.h"
#include "export.h"
#include "private/common.h"
#include "randombytes.h"
#include "runtime.h"
#include "utils.h"

#if defined(HAVE_ARMCRYPTO) && defined(__clang__) && defined(NATIVE_LITTLE_ENDIAN)

#if !defined(MSC_VER) || _MSC_VER < 1800
#define __vectorcall
#endif

#ifndef __ARM_FEATURE_CRYPTO
#define __ARM_FEATURE_CRYPTO 1
#endif
#ifndef __ARM_FEATURE_AES
#define __ARM_FEATURE_AES 1
#endif

#include <arm_neon.h>

#ifdef __clang__
#pragma clang attribute push(__attribute__((target("neon,crypto,aes"))), apply_to = function)
#elif defined(__GNUC__)
#pragma GCC target("+simd+crypto")
#endif

#define ABYTES    crypto_aead_aes256gcm_ABYTES
#define NPUBBYTES crypto_aead_aes256gcm_NPUBBYTES
#define KEYBYTES  crypto_aead_aes256gcm_KEYBYTES

#define PARALLEL_BLOCKS 6
#undef USE_KARATSUBA_MULTIPLICATION

typedef uint64x2_t BlockVec;

#define LOAD128(a)     vld1q_u64((const uint64_t *) (const void *) (a))
#define STORE128(a, b) vst1q_u64((uint64_t *) (void *) (a), (b))
#define AES_XENCRYPT(block_vec, rkey) \
    vreinterpretq_u64_u8(vaesmcq_u8(vaeseq_u8(vreinterpretq_u8_u64(block_vec), rkey)))
#define AES_XENCRYPTLAST(block_vec, rkey) \
    vreinterpretq_u64_u8(vaeseq_u8(vreinterpretq_u8_u64(block_vec), rkey))
#define XOR128(a, b)  veorq_u64((a), (b))
#define AND128(a, b)  vandq_u64((a), (b))
#define OR128(a, b)   vorrq_u64((a), (b))
#define SET64x2(a, b) vsetq_lane_u64((uint64_t) (a), vmovq_n_u64((uint64_t) (b)), 1)
#define ZERO128       vmovq_n_u8(0)
#define ONE128        SET64x2(0, 1)
#define ADD64x2(a, b) vaddq_u64((a), (b))
#define SUB64x2(a, b) vsubq_u64((a), (b))
#define SHL64x2(a, b) vshlq_n_u64((a), (b))
#define SHR64x2(a, b) vshrq_n_u64((a), (b))
#define REV128(x)                                                                                  \
    vreinterpretq_u64_u8(__builtin_shufflevector(vreinterpretq_u8_u64(x), vreinterpretq_u8_u64(x), \
                                                 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2,   \
                                                 1, 0))
#define SHUFFLE32x4(x, a, b, c, d)                                          \
    vreinterpretq_u64_u32(__builtin_shufflevector(vreinterpretq_u32_u64(x), \
                                                  vreinterpretq_u32_u64(x), (a), (b), (c), (d)))
#define BYTESHL128(a, b) vreinterpretq_u64_u8(vextq_s8(vdupq_n_s8(0), (int8x16_t) a, 16 - (b)))
#define BYTESHR128(a, b) vreinterpretq_u64_u8(vextq_s8((int8x16_t) a, vdupq_n_s8(0), (b)))

#define SHL128(a, b) OR128(SHL64x2((a), (b)), SHR64x2(BYTESHL128((a), 8), 64 - (b)))
#define CLMULLO128(a, b) \
    vreinterpretq_u64_p128(vmull_p64((poly64_t) vget_low_u64(a), (poly64_t) vget_low_u64(b)))
#define CLMULHI128(a, b) \
    vreinterpretq_u64_p128(vmull_high_p64(vreinterpretq_p64_s64(a), vreinterpretq_p64_s64(b)))
#define CLMULLOHI128(a, b) \
    vreinterpretq_u64_p128(vmull_p64((poly64_t) vget_low_u64(a), (poly64_t) vget_high_u64(b)))
#define CLMULHILO128(a, b) \
    vreinterpretq_u64_p128(vmull_p64((poly64_t) vget_high_u64(a), (poly64_t) vget_low_u64(b)))
#define PREFETCH_READ(x)  __builtin_prefetch((x), 0, 2)
#define PREFETCH_WRITE(x) __builtin_prefetch((x), 1, 2);

static inline BlockVec
AES_KEYGEN(BlockVec block_vec, const int rc)
{
    uint8x16_t       a = vaeseq_u8(vreinterpretq_u8_u64(block_vec), vmovq_n_u8(0));
    const uint8x16_t b =
        __builtin_shufflevector(a, a, 4, 1, 14, 11, 1, 14, 11, 4, 12, 9, 6, 3, 9, 6, 3, 12);
    const uint64x2_t c = SET64x2((uint64_t) rc << 32, (uint64_t) rc << 32);
    return XOR128(b, c);
}

#define ROUNDS 14

#define PC_COUNT (2 * PARALLEL_BLOCKS)

typedef struct I256 {
    BlockVec hi;
    BlockVec lo;
    BlockVec mid;
} I256;

typedef BlockVec Precomp;

typedef struct GHash {
    BlockVec acc;
} GHash;

typedef struct State {
    BlockVec rkeys[ROUNDS + 1];
    Precomp  hx[PC_COUNT];
} State;

static void __vectorcall expand256(const unsigned char key[KEYBYTES], BlockVec rkeys[1 + ROUNDS])
{
    BlockVec t1, t2, s;
    size_t   i = 0;

#define EXPAND_KEY_1(RC)                        \
    rkeys[i++] = t2;                            \
    s          = AES_KEYGEN(t2, RC);            \
    t1         = XOR128(t1, BYTESHL128(t1, 4)); \
    t1         = XOR128(t1, BYTESHL128(t1, 8)); \
    t1         = XOR128(t1, SHUFFLE32x4(s, 3, 3, 3, 3));

#define EXPAND_KEY_2(RC)                        \
    rkeys[i++] = t1;                            \
    s          = AES_KEYGEN(t1, RC);            \
    t2         = XOR128(t2, BYTESHL128(t2, 4)); \
    t2         = XOR128(t2, BYTESHL128(t2, 8)); \
    t2         = XOR128(t2, SHUFFLE32x4(s, 2, 2, 2, 2));

    t1 = LOAD128(&key[0]);
    t2 = LOAD128(&key[16]);

    rkeys[i++] = t1;
    EXPAND_KEY_1(0x01);
    EXPAND_KEY_2(0x01);
    EXPAND_KEY_1(0x02);
    EXPAND_KEY_2(0x02);
    EXPAND_KEY_1(0x04);
    EXPAND_KEY_2(0x04);
    EXPAND_KEY_1(0x08);
    EXPAND_KEY_2(0x08);
    EXPAND_KEY_1(0x10);
    EXPAND_KEY_2(0x10);
    EXPAND_KEY_1(0x20);
    EXPAND_KEY_2(0x20);
    EXPAND_KEY_1(0x40);
    rkeys[i++] = t1;
}

/* Encrypt a single AES block */

static inline void
encrypt(const State *st, unsigned char dst[16], const unsigned char src[16])
{
    BlockVec t;

    size_t i;

    t = AES_XENCRYPT(LOAD128(src), st->rkeys[0]);
    for (i = 1; i < ROUNDS - 1; i++) {
        t = AES_XENCRYPT(t, st->rkeys[i]);
    }
    t = AES_XENCRYPTLAST(t, st->rkeys[i]);
    t = XOR128(t, st->rkeys[ROUNDS]);
    STORE128(dst, t);
}

/* Encrypt and add a single AES block */

static inline void __vectorcall encrypt_xor_block(const State *st, unsigned char dst[16],
                                                  const unsigned char src[16],
                                                  const BlockVec      counter)
{
    BlockVec ts;
    size_t   i;

    ts = AES_XENCRYPT(counter, st->rkeys[0]);
    for (i = 1; i < ROUNDS - 1; i++) {
        ts = AES_XENCRYPT(ts, st->rkeys[i]);
    }
    ts = AES_XENCRYPTLAST(ts, st->rkeys[i]);
    ts = XOR128(ts, XOR128(st->rkeys[ROUNDS], LOAD128(src)));
    STORE128(dst, ts);
}

/* Encrypt and add PARALLEL_BLOCKS AES blocks */

static inline void __vectorcall encrypt_xor_wide(const State        *st,
                                                 unsigned char       dst[16 * PARALLEL_BLOCKS],
                                                 const unsigned char src[16 * PARALLEL_BLOCKS],
                                                 const BlockVec      counters[PARALLEL_BLOCKS])
{
    BlockVec ts[PARALLEL_BLOCKS];
    size_t   i, j;

    for (j = 0; j < PARALLEL_BLOCKS; j++) {
        ts[j] = AES_XENCRYPT(counters[j], st->rkeys[0]);
    }
    for (i = 1; i < ROUNDS - 1; i++) {
        for (j = 0; j < PARALLEL_BLOCKS; j++) {
            ts[j] = AES_XENCRYPT(ts[j], st->rkeys[i]);
        }
    }
    for (j = 0; j < PARALLEL_BLOCKS; j++) {
        ts[j] = AES_XENCRYPTLAST(ts[j], st->rkeys[i]);
        ts[j] = XOR128(ts[j], XOR128(st->rkeys[ROUNDS], LOAD128(&src[16 * j])));
    }
    for (j = 0; j < PARALLEL_BLOCKS; j++) {
        STORE128(&dst[16 * j], ts[j]);
    }
}

/* Square a field element */

static inline I256 __vectorcall clsq128(const BlockVec x)
{
    const BlockVec r_lo = CLMULLO128(x, x);
    const BlockVec r_hi = CLMULHI128(x, x);

    return (I256) {
        SODIUM_C99(.hi =) r_hi,
        SODIUM_C99(.lo =) r_lo,
        SODIUM_C99(.mid =) ZERO128,
    };
}

/* Multiply two field elements -- Textbook multiplication is faster than Karatsuba on some recent
 * CPUs */

static inline I256 __vectorcall clmul128(const BlockVec x, const BlockVec y)
{
#ifdef USE_KARATSUBA_MULTIPLICATION
    const BlockVec x_hi  = BYTESHR128(x, 8);
    const BlockVec y_hi  = BYTESHR128(y, 8);
    const BlockVec r_lo  = CLMULLO128(x, y);
    const BlockVec r_hi  = CLMULHI128(x, y);
    const BlockVec r_mid = XOR128(CLMULLO128(XOR128(x, x_hi), XOR128(y, y_hi)), XOR128(r_lo, r_hi));

    return (I256) {
        SODIUM_C99(.hi =) r_hi,
        SODIUM_C99(.lo =) r_lo,
        SODIUM_C99(.mid =) r_mid,
    };
#else
    const BlockVec r_hi  = CLMULHI128(x, y);
    const BlockVec r_lo  = CLMULLO128(x, y);
    const BlockVec r_mid = XOR128(CLMULHILO128(x, y), CLMULLOHI128(x, y));

    return (I256) {
        SODIUM_C99(.hi =) r_hi,
        SODIUM_C99(.lo =) r_lo,
        SODIUM_C99(.mid =) r_mid,
    };
#endif
}

/* Merge the middle word and reduce a field element */

static inline BlockVec __vectorcall gcm_reduce(const I256 x)
{
    const BlockVec hi = XOR128(x.hi, BYTESHR128(x.mid, 8));
    const BlockVec lo = XOR128(x.lo, BYTESHL128(x.mid, 8));

    const BlockVec p64 = SET64x2(0, 0xc200000000000000);
    const BlockVec a   = CLMULLO128(lo, p64);
    const BlockVec b   = XOR128(SHUFFLE32x4(lo, 2, 3, 0, 1), a);
    const BlockVec c   = CLMULLO128(b, p64);
    const BlockVec d   = XOR128(SHUFFLE32x4(b, 2, 3, 0, 1), c);

    return XOR128(d, hi);
}

/* Precompute powers of H from `from` to `to` */

static inline void __vectorcall precomp(Precomp hx[PC_COUNT], const size_t from, const size_t to)
{
    const Precomp h = hx[0];
    size_t        i;

    for (i = from & ~1U; i < to; i += 2) {
        hx[i]     = gcm_reduce(clmul128(hx[i - 1], h));
        hx[i + 1] = gcm_reduce(clsq128(hx[i / 2]));
    }
}

/* Precompute powers of H given a key and a block count */

static void __vectorcall precomp_for_block_count(Precomp             hx[PC_COUNT],
                                                 const unsigned char gh_key[16],
                                                 const size_t        block_count)
{
    const BlockVec h0    = REV128(LOAD128(gh_key));
    BlockVec       carry = SET64x2(0xc200000000000000, 1);
    BlockVec       mask  = SUB64x2(ZERO128, SHR64x2(h0, 63));
    BlockVec       h0_shifted;
    BlockVec       h;

    mask       = SHUFFLE32x4(mask, 3, 3, 3, 3);
    carry      = AND128(carry, mask);
    h0_shifted = SHL128(h0, 1);
    h          = XOR128(h0_shifted, carry);

    hx[0] = h;
    hx[1] = gcm_reduce(clsq128(hx[0]));

    if (block_count >= PC_COUNT) {
        precomp(hx, 2, PC_COUNT);
    } else {
        precomp(hx, 2, block_count);
    }
}

/* Initialize a GHash */

static inline void
gh_init(GHash *sth)
{
    sth->acc = ZERO128;
}

static inline I256 __vectorcall gh_update0(const GHash *const sth, const unsigned char *const p,
                                           const Precomp hn)
{
    const BlockVec m = REV128(LOAD128(p));
    return clmul128(XOR128(sth->acc, m), hn);
}

static inline void __vectorcall gh_update(I256 *const u, const unsigned char *p, const Precomp hn)
{
    const BlockVec m = REV128(LOAD128(p));
    const I256     t = clmul128(m, hn);
    *u = (I256) { SODIUM_C99(.hi =) XOR128(u->hi, t.hi), SODIUM_C99(.lo =) XOR128(u->lo, t.lo),
                  SODIUM_C99(.mid =) XOR128(u->mid, t.mid) };
}

/* Absorb ad_len bytes of associated data. There has to be no partial block. */

static inline void
gh_ad_blocks(const State *st, GHash *sth, const unsigned char *ad, size_t ad_len)
{
    size_t i;

    i = (size_t) 0U;
    for (; i + PC_COUNT * 16 <= ad_len; i += PC_COUNT * 16) {
        I256   u = gh_update0(sth, ad + i, st->hx[PC_COUNT - 1 - 0]);
        size_t j;

        for (j = 1; j < PC_COUNT; j += 1) {
            gh_update(&u, ad + i + j * 16, st->hx[PC_COUNT - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }
    for (; i + PC_COUNT * 16 / 2 <= ad_len; i += PC_COUNT * 16 / 2) {
        I256   u = gh_update0(sth, ad + i, st->hx[PC_COUNT / 2 - 1 - 0]);
        size_t j;

        for (j = 1; j < PC_COUNT / 2; j += 1) {
            gh_update(&u, ad + i + j * 16, st->hx[PC_COUNT / 2 - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }
    for (; i + 4 * 16 <= ad_len; i += 4 * 16) {
        size_t j;
        I256   u = gh_update0(sth, ad + i, st->hx[4 - 1 - 0]);

        for (j = 1; j < 4; j += 1) {
            gh_update(&u, ad + i + j * 16, st->hx[4 - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }
    for (; i + 2 * 16 <= ad_len; i += 2 * 16) {
        size_t j;
        I256   u = gh_update0(sth, ad + i, st->hx[2 - 1 - 0]);

        for (j = 1; j < 2; j += 1) {
            gh_update(&u, ad + i + j * 16, st->hx[2 - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }
    if (i < ad_len) {
        I256 u   = gh_update0(sth, ad + i, st->hx[0]);
        sth->acc = gcm_reduce(u);
    }
}

/* Increment counters */

static inline BlockVec __vectorcall incr_counters(BlockVec rev_counters[], BlockVec counter,
                                                  const size_t n)
{
    size_t i;

    const BlockVec one = ONE128;
    for (i = 0; i < n; i++) {
        rev_counters[i] = REV128(counter);
        counter         = ADD64x2(counter, one);
    }
    return counter;
}

/* Compute the number of required blocks to encrypt and authenticate `ad_len` of associated data,
 * and `m_len` of encrypted bytes. Return `0` if limits would be exceeded.*/

static inline size_t
required_blocks(const size_t ad_len, const size_t m_len)
{
    const size_t ad_blocks = (ad_len + 15) / 16;
    const size_t m_blocks  = (m_len + 15) / 16;

    if (ad_len > SIZE_MAX - 2 * PARALLEL_BLOCKS * 16 ||
        m_len > SIZE_MAX - 2 * PARALLEL_BLOCKS * 16 || ad_len < ad_blocks || m_len < m_blocks ||
        m_blocks >= (1ULL << 32) - 2) {
        return 0;
    }
    return ad_blocks + m_blocks + 1;
}

/* Generic AES-GCM encryption. "Generic" as it can handle arbitrary input sizes,
unlike a length-limited version that would precompute all the required powers of H */

static void
aes_gcm_encrypt_generic(const State *st, GHash *sth, unsigned char mac[ABYTES], unsigned char *dst,
                        const unsigned char *src, size_t src_len, const unsigned char *ad,
                        size_t ad_len, unsigned char counter_[16])
{
    CRYPTO_ALIGN(32) I256          u;
    CRYPTO_ALIGN(16) unsigned char last_blocks[2 * 16];
    const BlockVec                 one = ONE128;
    BlockVec                       final_block;
    BlockVec                       rev_counters[PARALLEL_BLOCKS];
    BlockVec                       counter;
    size_t                         i;
    size_t                         j;
    size_t                         left;
    size_t                         pi;

    COMPILER_ASSERT(PC_COUNT % PARALLEL_BLOCKS == 0);

    /* Associated data */

    if (ad != NULL && ad_len != 0) {
        gh_ad_blocks(st, sth, ad, ad_len & ~15);
        left = ad_len & 15;
        if (left != 0) {
            unsigned char pad[16];

            memset(pad, 0, sizeof pad);
            memcpy(pad, ad + ad_len - left, left);
            gh_ad_blocks(st, sth, pad, sizeof pad);
        }
    }

    /* Encrypted data */

    counter = REV128(LOAD128(counter_));
    i       = 0;

    /* 2*PARALLEL_BLOCKS aggregation */

    if (src_len - i >= 2 * PARALLEL_BLOCKS * 16) {
        counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);
        encrypt_xor_wide(st, dst + i, src + i, rev_counters);
        i += PARALLEL_BLOCKS * 16;

        for (; i + 2 * PARALLEL_BLOCKS * 16 <= src_len; i += 2 * PARALLEL_BLOCKS * 16) {
            counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);
            encrypt_xor_wide(st, dst + i, src + i, rev_counters);

            pi = i - PARALLEL_BLOCKS * 16;
            u  = gh_update0(sth, dst + pi, st->hx[2 * PARALLEL_BLOCKS - 1 - 0]);
            for (j = 1; j < PARALLEL_BLOCKS; j += 1) {
                gh_update(&u, dst + pi + j * 16, st->hx[2 * PARALLEL_BLOCKS - 1 - j]);
            }

            counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);
            encrypt_xor_wide(st, dst + i + PARALLEL_BLOCKS * 16, src + i + PARALLEL_BLOCKS * 16,
                             rev_counters);

            pi = i;
            for (j = 0; j < PARALLEL_BLOCKS; j += 1) {
                gh_update(&u, dst + pi + j * 16, st->hx[PARALLEL_BLOCKS - 1 - j]);
            }
            sth->acc = gcm_reduce(u);
        }

        pi = i - PARALLEL_BLOCKS * 16;
        u  = gh_update0(sth, dst + pi, st->hx[PARALLEL_BLOCKS - 1 - 0]);
        for (j = 1; j < PARALLEL_BLOCKS; j += 1) {
            gh_update(&u, dst + pi + j * 16, st->hx[PARALLEL_BLOCKS - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }

    /* PARALLEL_BLOCKS aggregation */

    if (src_len - i >= PARALLEL_BLOCKS * 16) {
        counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);
        encrypt_xor_wide(st, dst + i, src + i, rev_counters);
        i += PARALLEL_BLOCKS * 16;

        for (; i + PARALLEL_BLOCKS * 16 <= src_len; i += PARALLEL_BLOCKS * 16) {
            counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);
            encrypt_xor_wide(st, dst + i, src + i, rev_counters);

            pi = i - PARALLEL_BLOCKS * 16;
            u  = gh_update0(sth, dst + pi, st->hx[PARALLEL_BLOCKS - 1 - 0]);
            for (j = 1; j < PARALLEL_BLOCKS; j += 1) {
                gh_update(&u, dst + pi + j * 16, st->hx[PARALLEL_BLOCKS - 1 - j]);
            }
            sth->acc = gcm_reduce(u);
        }

        pi = i - PARALLEL_BLOCKS * 16;
        u  = gh_update0(sth, dst + pi, st->hx[PARALLEL_BLOCKS - 1 - 0]);
        for (j = 1; j < PARALLEL_BLOCKS; j += 1) {
            gh_update(&u, dst + pi + j * 16, st->hx[PARALLEL_BLOCKS - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }

    /* 4-blocks aggregation */

    for (; i + 4 * 16 <= src_len; i += 4 * 16) {
        counter = incr_counters(rev_counters, counter, 4);
        for (j = 0; j < 4; j++) {
            encrypt_xor_block(st, dst + i + j * 16, src + i + j * 16, rev_counters[j]);
        }

        u = gh_update0(sth, dst + i, st->hx[4 - 1 - 0]);
        for (j = 1; j < 4; j += 1) {
            gh_update(&u, dst + i + j * 16, st->hx[4 - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }

    /* 2-blocks aggregation */

    for (; i + 2 * 16 <= src_len; i += 2 * 16) {
        counter = incr_counters(rev_counters, counter, 2);
        for (j = 0; j < 2; j++) {
            encrypt_xor_block(st, dst + i + j * 16, src + i + j * 16, rev_counters[j]);
        }

        u = gh_update0(sth, dst + i, st->hx[2 - 1 - 0]);
        for (j = 1; j < 2; j += 1) {
            gh_update(&u, dst + i + j * 16, st->hx[2 - 1 - j]);
        }
        sth->acc = gcm_reduce(u);
    }

    /* Remaining *partial* blocks; if we have 16 bytes left, we want to keep the
    full block authenticated along with the final block, hence < and not <= */

    for (; i + 16 < src_len; i += 16) {
        encrypt_xor_block(st, dst + i, src + i, REV128(counter));
        u        = gh_update0(sth, dst + i, st->hx[1 - 1 - 0]);
        sth->acc = gcm_reduce(u);
        counter  = ADD64x2(counter, one);
    }

    /* Authenticate both the last block of the message and the final block */

    final_block = REV128(SET64x2(ad_len * 8, src_len * 8));
    STORE32_BE(counter_ + NPUBBYTES, 1);
    encrypt(st, mac, counter_);
    left = src_len - i;
    if (left != 0) {
        for (j = 0; j < left; j++) {
            last_blocks[j] = src[i + j];
        }
        STORE128(last_blocks + 16, final_block);
        encrypt_xor_block(st, last_blocks, last_blocks, REV128(counter));
        for (; j < 16; j++) {
            last_blocks[j] = 0;
        }
        for (j = 0; j < left; j++) {
            dst[i + j] = last_blocks[j];
        }
        gh_ad_blocks(st, sth, last_blocks, 32);
    } else {
        STORE128(last_blocks, final_block);
        gh_ad_blocks(st, sth, last_blocks, 16);
    }
    STORE128(mac, XOR128(LOAD128(mac), REV128(sth->acc)));
}

/* Generic AES-GCM decryption. "Generic" as it can handle arbitrary input sizes,
unlike a length-limited version that would precompute all the required powers of H */

static void
aes_gcm_decrypt_generic(const State *st, GHash *sth, unsigned char mac[ABYTES], unsigned char *dst,
                        const unsigned char *src, size_t src_len, const unsigned char *ad,
                        size_t ad_len, unsigned char counter_[16])
{
    CRYPTO_ALIGN(32) I256          u;
    CRYPTO_ALIGN(16) unsigned char last_blocks[2 * 16];
    const BlockVec                 one = ONE128;
    BlockVec                       final_block;
    BlockVec                       rev_counters[PARALLEL_BLOCKS];
    BlockVec                       counter;
    size_t                         i;
    size_t                         j;
    size_t                         left;

    COMPILER_ASSERT(PC_COUNT % PARALLEL_BLOCKS == 0);

    /* Associated data */

    if (ad != NULL && ad_len != 0) {
        gh_ad_blocks(st, sth, ad, ad_len & ~15);
        left = ad_len & 15;
        if (left != 0) {
            unsigned char pad[16];

            memset(pad, 0, sizeof pad);
            memcpy(pad, ad + ad_len - left, left);
            gh_ad_blocks(st, sth, pad, sizeof pad);
        }
    }

    /* Encrypted data */

    counter = REV128(LOAD128(counter_));
    i       = 0;

    /* 2*PARALLEL_BLOCKS aggregation */

    while (i + 2 * PARALLEL_BLOCKS * 16 <= src_len) {
        counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);

        u = gh_update0(sth, src + i, st->hx[2 * PARALLEL_BLOCKS - 1 - 0]);
        for (j = 1; j < PARALLEL_BLOCKS; j += 1) {
            gh_update(&u, src + i + j * 16, st->hx[2 * PARALLEL_BLOCKS - 1 - j]);
        }

        encrypt_xor_wide(st, dst + i, src + i, rev_counters);

        counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);

        i += PARALLEL_BLOCKS * 16;
        for (j = 0; j < PARALLEL_BLOCKS; j += 1) {
            gh_update(&u, src + i + j * 16, st->hx[PARALLEL_BLOCKS - 1 - j]);
        }
        sth->acc = gcm_reduce(u);

        encrypt_xor_wide(st, dst + i, src + i, rev_counters);
        i += PARALLEL_BLOCKS * 16;
    }

    /* PARALLEL_BLOCKS aggregation */

    for (; i + PARALLEL_BLOCKS * 16 <= src_len; i += PARALLEL_BLOCKS * 16) {
        counter = incr_counters(rev_counters, counter, PARALLEL_BLOCKS);

        u = gh_update0(sth, src + i, st->hx[PARALLEL_BLOCKS - 1 - 0]);
        for (j = 1; j < PARALLEL_BLOCKS; j += 1) {
            gh_update(&u, src + i + j * 16, st->hx[PARALLEL_BLOCKS - 1 - j]);
        }
        sth->acc = gcm_reduce(u);

        encrypt_xor_wide(st, dst + i, src + i, rev_counters);
    }

    /* 4-blocks aggregation */

    for (; i + 4 * 16 <= src_len; i += 4 * 16) {
        counter = incr_counters(rev_counters, counter, 4);

        u = gh_update0(sth, src + i, st->hx[4 - 1 - 0]);
        for (j = 1; j < 4; j += 1) {
            gh_update(&u, src + i + j * 16, st->hx[4 - 1 - j]);
        }
        sth->acc = gcm_reduce(u);

        for (j = 0; j < 4; j++) {
            encrypt_xor_block(st, dst + i + j * 16, src + i + j * 16, rev_counters[j]);
        }
    }

    /* 2-blocks aggregation */

    for (; i + 2 * 16 <= src_len; i += 2 * 16) {
        counter = incr_counters(rev_counters, counter, 2);

        u = gh_update0(sth, src + i, st->hx[2 - 1 - 0]);
        for (j = 1; j < 2; j += 1) {
            gh_update(&u, src + i + j * 16, st->hx[2 - 1 - j]);
        }
        sth->acc = gcm_reduce(u);

        for (j = 0; j < 2; j++) {
            encrypt_xor_block(st, dst + i + j * 16, src + i + j * 16, rev_counters[j]);
        }
    }

    /* Remaining *partial* blocks; if we have 16 bytes left, we want to keep the
    full block authenticated along with the final block, hence < and not <= */

    for (; i + 16 < src_len; i += 16) {
        u        = gh_update0(sth, src + i, st->hx[1 - 1 - 0]);
        sth->acc = gcm_reduce(u);
        encrypt_xor_block(st, dst + i, src + i, REV128(counter));
        counter = ADD64x2(counter, one);
    }

    /* Authenticate both the last block of the message and the final block */

    final_block = REV128(SET64x2(ad_len * 8, src_len * 8));
    STORE32_BE(counter_ + NPUBBYTES, 1);
    encrypt(st, mac, counter_);
    left = src_len - i;
    if (left != 0) {
        for (j = 0; j < left; j++) {
            last_blocks[j] = src[i + j];
        }
        for (; j < 16; j++) {
            last_blocks[j] = 0;
        }
        STORE128(last_blocks + 16, final_block);
        gh_ad_blocks(st, sth, last_blocks, 32);
        encrypt_xor_block(st, last_blocks, last_blocks, REV128(counter));
        for (j = 0; j < left; j++) {
            dst[i + j] = last_blocks[j];
        }
    } else {
        STORE128(last_blocks, final_block);
        gh_ad_blocks(st, sth, last_blocks, 16);
    }
    STORE128(mac, XOR128(LOAD128(mac), REV128(sth->acc)));
}

int
crypto_aead_aes256gcm_beforenm(crypto_aead_aes256gcm_state *st_, const unsigned char *k)
{
    State                         *st = (State *) (void *) st_;
    CRYPTO_ALIGN(16) unsigned char h[16];

    COMPILER_ASSERT(sizeof *st_ >= sizeof *st);

    expand256(k, st->rkeys);
    memset(h, 0, sizeof h);
    encrypt(st, h, h);

    precomp_for_block_count(st->hx, h, PC_COUNT);

    return 0;
}

int
crypto_aead_aes256gcm_encrypt_detached_afternm(unsigned char *c, unsigned char *mac,
                                               unsigned long long *maclen_p, const unsigned char *m,
                                               unsigned long long m_len_, const unsigned char *ad,
                                               unsigned long long   ad_len_,
                                               const unsigned char *nsec, const unsigned char *npub,
                                               const crypto_aead_aes256gcm_state *st_)
{
    const State                   *st = (const State *) (const void *) st_;
    GHash                          sth;
    CRYPTO_ALIGN(16) unsigned char j[16];
    size_t                         gh_required_blocks;
    const size_t                   ad_len = (size_t) ad_len_;
    const size_t                   m_len  = (size_t) m_len_;

    (void) nsec;
    if (maclen_p != NULL) {
        *maclen_p = 0;
    }
    if (ad_len_ > SODIUM_SIZE_MAX || m_len_ > SODIUM_SIZE_MAX) {
        sodium_misuse();
    }
    gh_required_blocks = required_blocks(ad_len, m_len);
    if (gh_required_blocks == 0) {
        memset(mac, 0xd0, ABYTES);
        memset(c, 0, m_len);
        return -1;
    }

    gh_init(&sth);

    memcpy(j, npub, NPUBBYTES);
    STORE32_BE(j + NPUBBYTES, 2);

    aes_gcm_encrypt_generic(st, &sth, mac, c, m, m_len, ad, ad_len, j);

    if (maclen_p != NULL) {
        *maclen_p = ABYTES;
    }
    return 0;
}

int
crypto_aead_aes256gcm_encrypt(unsigned char *c, unsigned long long *clen_p, const unsigned char *m,
                              unsigned long long m_len, const unsigned char *ad,
                              unsigned long long ad_len, const unsigned char *nsec,
                              const unsigned char *npub, const unsigned char *k)
{
    const int ret = crypto_aead_aes256gcm_encrypt_detached(c, c + m_len, NULL, m, m_len, ad, ad_len,
                                                           nsec, npub, k);
    if (clen_p != NULL) {
        if (ret == 0) {
            *clen_p = m_len + crypto_aead_aes256gcm_ABYTES;
        } else {
            *clen_p = 0;
        }
    }
    return ret;
}

int
crypto_aead_aes256gcm_encrypt_detached(unsigned char *c, unsigned char *mac,
                                       unsigned long long *maclen_p, const unsigned char *m,
                                       unsigned long long m_len, const unsigned char *ad,
                                       unsigned long long ad_len, const unsigned char *nsec,
                                       const unsigned char *npub, const unsigned char *k)
{
    CRYPTO_ALIGN(16) crypto_aead_aes256gcm_state st;
    int                                          ret;

    PREFETCH_WRITE(c);
    PREFETCH_READ(m);
    PREFETCH_READ(ad);

    crypto_aead_aes256gcm_beforenm(&st, k);
    ret = crypto_aead_aes256gcm_encrypt_detached_afternm(c, mac, maclen_p, m, m_len, ad, ad_len,
                                                         nsec, npub, &st);
    sodium_memzero(&st, sizeof st);

    return ret;
}

int
crypto_aead_aes256gcm_encrypt_afternm(unsigned char *c, unsigned long long *clen_p,
                                      const unsigned char *m, unsigned long long mlen,
                                      const unsigned char *ad, unsigned long long adlen,
                                      const unsigned char *nsec, const unsigned char *npub,
                                      const crypto_aead_aes256gcm_state *st_)
{
    int ret = crypto_aead_aes256gcm_encrypt_detached_afternm(c, c + mlen, NULL, m, mlen, ad, adlen,
                                                             nsec, npub, st_);
    if (clen_p != NULL) {
        *clen_p = mlen + crypto_aead_aes256gcm_ABYTES;
    }
    return ret;
}

static int
crypto_aead_aes256gcm_verify_mac(unsigned char *nsec, const unsigned char *c,
                                 unsigned long long c_len_, const unsigned char *mac,
                                 const unsigned char *ad, unsigned long long ad_len_,
                                 const unsigned char *npub, const crypto_aead_aes256gcm_state *st_)
{
    const State                   *st = (const State *) (const void *) st_;
    GHash                          sth;
    BlockVec                       final_block;
    CRYPTO_ALIGN(16) unsigned char j[16];
    CRYPTO_ALIGN(16) unsigned char computed_mac[16];
    CRYPTO_ALIGN(16) unsigned char last_block[16];
    size_t                         gh_required_blocks;
    size_t                         left;
    const size_t                   ad_len = (size_t) ad_len_;
    const size_t                   c_len  = (size_t) c_len_;
    int                            ret;

    (void) nsec;
    if (ad_len_ > SODIUM_SIZE_MAX || c_len_ > SODIUM_SIZE_MAX) {
        sodium_misuse();
    }
    gh_required_blocks = required_blocks(ad_len, c_len);
    if (gh_required_blocks == 0) {
        return -1;
    }

    gh_init(&sth);

    memcpy(j, npub, NPUBBYTES);
    STORE32_BE(j + NPUBBYTES, 2);

    gh_ad_blocks(st, &sth, ad, ad_len & ~15);
    left = ad_len & 15;
    if (left != 0) {
        unsigned char pad[16];

        memset(pad, 0, sizeof pad);
        memcpy(pad, ad + ad_len - left, left);
        gh_ad_blocks(st, &sth, pad, sizeof pad);
    }

    gh_ad_blocks(st, &sth, c, c_len & ~15);
    left = c_len & 15;
    if (left != 0) {
        unsigned char pad[16];

        memset(pad, 0, sizeof pad);
        memcpy(pad, c + c_len - left, left);
        gh_ad_blocks(st, &sth, pad, sizeof pad);
    }
    final_block = REV128(SET64x2(ad_len * 8, c_len * 8));
    STORE32_BE(j + NPUBBYTES, 1);
    encrypt(st, computed_mac, j);
    STORE128(last_block, final_block);
    gh_ad_blocks(st, &sth, last_block, 16);
    STORE128(computed_mac, XOR128(LOAD128(computed_mac), REV128(sth.acc)));

    ret = crypto_verify_16(mac, computed_mac);
    sodium_memzero(computed_mac, sizeof computed_mac);

    return ret;
}

int
crypto_aead_aes256gcm_decrypt_detached_afternm(unsigned char *m, unsigned char *nsec,
                                               const unsigned char *c, unsigned long long c_len_,
                                               const unsigned char *mac, const unsigned char *ad,
                                               unsigned long long                 ad_len_,
                                               const unsigned char               *npub,
                                               const crypto_aead_aes256gcm_state *st_)
{
    const State                   *st = (const State *) (const void *) st_;
    GHash                          sth;
    CRYPTO_ALIGN(16) unsigned char j[16];
    unsigned char                  computed_mac[16];
    size_t                         gh_required_blocks;
    const size_t                   ad_len = (size_t) ad_len_;
    const size_t                   c_len  = (size_t) c_len_;
    const size_t                   m_len  = c_len;

    (void) nsec;
    if (ad_len_ > SODIUM_SIZE_MAX || c_len_ > SODIUM_SIZE_MAX) {
        sodium_misuse();
    }
    if (m == NULL) {
        return crypto_aead_aes256gcm_verify_mac(nsec, c, c_len, mac, ad, ad_len, npub, st_);
    }
    gh_required_blocks = required_blocks(ad_len, m_len);
    if (gh_required_blocks == 0) {
        return -1;
    }

    gh_init(&sth);

    memcpy(j, npub, NPUBBYTES);
    STORE32_BE(j + NPUBBYTES, 2);

    aes_gcm_decrypt_generic(st, &sth, computed_mac, m, c, m_len, ad, ad_len, j);

    if (crypto_verify_16(mac, computed_mac) != 0) {
        sodium_memzero(computed_mac, sizeof computed_mac);
        memset(m, 0xd0, m_len);
        return -1;
    }
    return 0;
}

int
crypto_aead_aes256gcm_decrypt_afternm(unsigned char *m, unsigned long long *mlen_p,
                                      unsigned char *nsec, const unsigned char *c,
                                      unsigned long long clen, const unsigned char *ad,
                                      unsigned long long adlen, const unsigned char *npub,
                                      const crypto_aead_aes256gcm_state *st_)
{
    unsigned long long mlen = 0ULL;
    int                ret  = -1;

    if (clen >= ABYTES) {
        ret = crypto_aead_aes256gcm_decrypt_detached_afternm(
            m, nsec, c, clen - ABYTES, c + clen - ABYTES, ad, adlen, npub, st_);
    }
    if (mlen_p != NULL) {
        if (ret == 0) {
            mlen = clen - ABYTES;
        }
        *mlen_p = mlen;
    }
    return ret;
}

int
crypto_aead_aes256gcm_decrypt_detached(unsigned char *m, unsigned char *nsec,
                                       const unsigned char *c, unsigned long long clen,
                                       const unsigned char *mac, const unsigned char *ad,
                                       unsigned long long adlen, const unsigned char *npub,
                                       const unsigned char *k)
{
    CRYPTO_ALIGN(16) crypto_aead_aes256gcm_state st;

    PREFETCH_WRITE(m);
    PREFETCH_READ(c);
    PREFETCH_READ(ad);

    crypto_aead_aes256gcm_beforenm(&st, k);

    return crypto_aead_aes256gcm_decrypt_detached_afternm(
        m, nsec, c, clen, mac, ad, adlen, npub, (const crypto_aead_aes256gcm_state *) &st);
}

int
crypto_aead_aes256gcm_decrypt(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec,
                              const unsigned char *c, unsigned long long clen,
                              const unsigned char *ad, unsigned long long adlen,
                              const unsigned char *npub, const unsigned char *k)
{
    CRYPTO_ALIGN(16) crypto_aead_aes256gcm_state st;
    int                                          ret;

    PREFETCH_WRITE(m);
    PREFETCH_READ(c);
    PREFETCH_READ(ad);

    crypto_aead_aes256gcm_beforenm(&st, k);

    ret = crypto_aead_aes256gcm_decrypt_afternm(m, mlen_p, nsec, c, clen, ad, adlen, npub,
                                                (const crypto_aead_aes256gcm_state *) &st);
    sodium_memzero(&st, sizeof st);

    return ret;
}

int
crypto_aead_aes256gcm_is_available(void)
{
    return sodium_runtime_has_armcrypto();
}

#ifdef __clang__
#pragma clang attribute pop
#endif

#endif
