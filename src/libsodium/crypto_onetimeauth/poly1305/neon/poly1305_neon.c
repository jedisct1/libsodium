
#include <stdint.h>

#include "../onetimeauth_poly1305.h"
#include "crypto_verify_16.h"
#include "poly1305_neon.h"
#include "private/common.h"
#include "utils.h"

#if (defined(__aarch64__) || defined(_M_ARM64)) && \
    (defined(__ARM_NEON) || defined(__ARM_NEON__)) && \
    defined(HAVE_TI_MODE) && defined(NATIVE_LITTLE_ENDIAN)

# include <arm_neon.h>

# if defined(_MSC_VER)
#  define POLY1305_NOINLINE __declspec(noinline)
# elif defined(__clang__) || defined(__GNUC__)
#  define POLY1305_NOINLINE __attribute__((noinline))
# else
#  define POLY1305_NOINLINE
# endif

# define poly1305_block_size 16

/*
 * Below these sizes, the scalar code is faster.
 * The first vector call needs more data, since it also computes r^2..r^8.
 */
# define MIN_VECTOR_BYTES      128
# define MIN_VECTOR_BYTES_WARM 64

typedef struct poly1305_state_internal_t {
    uint64_t           r[3];
    uint64_t           h[3];
    uint64_t           pad[2];
    unsigned long long leftover;
    unsigned char      buffer[poly1305_block_size];
    uint32_t           rpow[5][8]; /* rpow[i][j] is limb i of r^(8 - j) */
    uint32_t           rpow_ready;
    unsigned char      final;
} poly1305_state_internal_t;

static void
poly1305_init(poly1305_state_internal_t *st, const unsigned char key[32])
{
    uint64_t t0, t1;

    t0 = LOAD64_LE(&key[0]);
    t1 = LOAD64_LE(&key[8]);

    st->r[0] = t0 & 0xffc0fffffff;
    st->r[1] = ((t0 >> 44) | (t1 << 20)) & 0xfffffc0ffff;
    st->r[2] = (t1 >> 24) & 0x00ffffffc0f;

    st->h[0] = 0;
    st->h[1] = 0;
    st->h[2] = 0;

    st->pad[0] = LOAD64_LE(&key[16]);
    st->pad[1] = LOAD64_LE(&key[24]);

    st->leftover   = 0;
    st->rpow_ready = 0;
    st->final      = 0;
}

static inline void
poly1305_carry(uint64_t out[3], uint128_t d0, uint128_t d1, uint128_t d2)
{
    uint64_t h0, h1, h2, c;

    c  = (uint64_t) (d0 >> 44);
    h0 = (uint64_t) d0 & 0xfffffffffff;
    d1 += c;
    c  = (uint64_t) (d1 >> 44);
    h1 = (uint64_t) d1 & 0xfffffffffff;
    d2 += c;
    c  = (uint64_t) (d2 >> 42);
    h2 = (uint64_t) d2 & 0x3ffffffffff;
    h0 += c * 5;
    c  = h0 >> 44;
    h0 &= 0xfffffffffff;
    h1 += c;

    out[0] = h0;
    out[1] = h1;
    out[2] = h2;
}

static inline void
poly1305_mul(uint64_t out[3], const uint64_t a[3], const uint64_t b[3])
{
    const uint64_t s1 = b[1] * (5 << 2);
    const uint64_t s2 = b[2] * (5 << 2);

    poly1305_carry(out,
                   (uint128_t) a[0] * b[0] + (uint128_t) a[1] * s2 +
                       (uint128_t) a[2] * s1,
                   (uint128_t) a[0] * b[1] + (uint128_t) a[1] * b[0] +
                       (uint128_t) a[2] * s2,
                   (uint128_t) a[0] * b[2] + (uint128_t) a[1] * b[1] +
                       (uint128_t) a[2] * b[0]);
}

static inline void
poly1305_sqr(uint64_t out[3], const uint64_t a[3])
{
    const uint64_t a0_2 = a[0] * 2;
    const uint64_t a1_2 = a[1] * 2;
    const uint64_t s2   = a[2] * (5 << 2);

    poly1305_carry(out,
                   (uint128_t) a[0] * a[0] + (uint128_t) a1_2 * s2,
                   (uint128_t) a0_2 * a[1] + (uint128_t) a[2] * s2,
                   (uint128_t) a0_2 * a[2] + (uint128_t) a[1] * a[1]);
}

/* h[0] must be below 2^44 */
static inline void
poly1305_radix26_from_44(uint32_t t[5], const uint64_t h[3])
{
    const uint64_t h1 = h[1] & 0xfffffffffff;
    const uint64_t h2 = h[2] + (h[1] >> 44);

    t[0] = (uint32_t) (h[0] & 0x3ffffff);
    t[1] = (uint32_t) (((h[0] >> 26) | (h1 << 18)) & 0x3ffffff);
    t[2] = (uint32_t) ((h1 >> 8) & 0x3ffffff);
    t[3] = (uint32_t) (((h1 >> 34) | (h2 << 10)) & 0x3ffffff);
    t[4] = (uint32_t) (h2 >> 16);
}

static inline void
poly1305_store_power(poly1305_state_internal_t *st, const uint64_t p[3],
                     int n)
{
    uint32_t t[5];
    int      i;

    poly1305_radix26_from_44(t, p);
    for (i = 0; i < 5; i++) {
        st->rpow[i][8 - n] = t[i];
    }
}

/* lane j of a[i] is limb i of block j */
static inline void
poly1305_load_4blocks(uint32x4_t a[5], const unsigned char *m)
{
    const uint32x4_t mask  = vdupq_n_u32(0x3ffffff);
    const uint32x4_t hibit = vdupq_n_u32(1U << 24);
    uint32x4_t       b0, b1, b2, b3;
    uint32x4_t       e01, e23, o01, o23;
    uint32x4_t       w0, w1, w2, w3;

    b0  = vreinterpretq_u32_u8(vld1q_u8(m));
    b1  = vreinterpretq_u32_u8(vld1q_u8(m + 16));
    b2  = vreinterpretq_u32_u8(vld1q_u8(m + 32));
    b3  = vreinterpretq_u32_u8(vld1q_u8(m + 48));
    e01 = vuzp1q_u32(b0, b1);
    o01 = vuzp2q_u32(b0, b1);
    e23 = vuzp1q_u32(b2, b3);
    o23 = vuzp2q_u32(b2, b3);
    w0  = vuzp1q_u32(e01, e23);
    w1  = vuzp1q_u32(o01, o23);
    w2  = vuzp2q_u32(e01, e23);
    w3  = vuzp2q_u32(o01, o23);

    a[0] = vandq_u32(w0, mask);
    a[1] = vandq_u32(vsliq_n_u32(vshrq_n_u32(w0, 26), w1, 6), mask);
    a[2] = vandq_u32(vsliq_n_u32(vshrq_n_u32(w1, 20), w2, 12), mask);
    a[3] = vandq_u32(vsliq_n_u32(vshrq_n_u32(w2, 14), w3, 18), mask);
    a[4] = vsraq_n_u32(hibit, w3, 8);
}

static inline void
poly1305_load_2blocks(uint32x2_t a[5], const unsigned char *m)
{
    const uint32x2_t mask  = vdup_n_u32(0x3ffffff);
    const uint32x2_t hibit = vdup_n_u32(1U << 24);
    uint32x4_t       b0, b1, t02, t13;
    uint32x2_t       w0, w1, w2, w3;

    b0  = vreinterpretq_u32_u8(vld1q_u8(m));
    b1  = vreinterpretq_u32_u8(vld1q_u8(m + 16));
    t02 = vtrn1q_u32(b0, b1);
    t13 = vtrn2q_u32(b0, b1);
    w0  = vget_low_u32(t02);
    w1  = vget_low_u32(t13);
    w2  = vget_high_u32(t02);
    w3  = vget_high_u32(t13);

    a[0] = vand_u32(w0, mask);
    a[1] = vand_u32(vsli_n_u32(vshr_n_u32(w0, 26), w1, 6), mask);
    a[2] = vand_u32(vsli_n_u32(vshr_n_u32(w1, 20), w2, 12), mask);
    a[3] = vand_u32(vsli_n_u32(vshr_n_u32(w2, 14), w3, 18), mask);
    a[4] = vsra_n_u32(hibit, w3, 8);
}

/* T += a * r, with r taken from lane L of R, and S = 5 * R */
# define POLY1305_MLAL_LANE(T, a, R, S, L)                 \
    do {                                                   \
        T[0] = vmlal_laneq_u32(T[0], a[0], R[0], L);       \
        T[1] = vmlal_laneq_u32(T[1], a[0], R[1], L);       \
        T[2] = vmlal_laneq_u32(T[2], a[0], R[2], L);       \
        T[3] = vmlal_laneq_u32(T[3], a[0], R[3], L);       \
        T[4] = vmlal_laneq_u32(T[4], a[0], R[4], L);       \
        T[0] = vmlal_laneq_u32(T[0], a[1], S[4], L);       \
        T[1] = vmlal_laneq_u32(T[1], a[1], R[0], L);       \
        T[2] = vmlal_laneq_u32(T[2], a[1], R[1], L);       \
        T[3] = vmlal_laneq_u32(T[3], a[1], R[2], L);       \
        T[4] = vmlal_laneq_u32(T[4], a[1], R[3], L);       \
        T[0] = vmlal_laneq_u32(T[0], a[2], S[3], L);       \
        T[1] = vmlal_laneq_u32(T[1], a[2], S[4], L);       \
        T[2] = vmlal_laneq_u32(T[2], a[2], R[0], L);       \
        T[3] = vmlal_laneq_u32(T[3], a[2], R[1], L);       \
        T[4] = vmlal_laneq_u32(T[4], a[2], R[2], L);       \
        T[0] = vmlal_laneq_u32(T[0], a[3], S[2], L);       \
        T[1] = vmlal_laneq_u32(T[1], a[3], S[3], L);       \
        T[2] = vmlal_laneq_u32(T[2], a[3], S[4], L);       \
        T[3] = vmlal_laneq_u32(T[3], a[3], R[0], L);       \
        T[4] = vmlal_laneq_u32(T[4], a[3], R[1], L);       \
        T[0] = vmlal_laneq_u32(T[0], a[4], S[1], L);       \
        T[1] = vmlal_laneq_u32(T[1], a[4], S[2], L);       \
        T[2] = vmlal_laneq_u32(T[2], a[4], S[3], L);       \
        T[3] = vmlal_laneq_u32(T[3], a[4], S[4], L);       \
        T[4] = vmlal_laneq_u32(T[4], a[4], R[0], L);       \
    } while (0)

/* loads [r^(8 - idx), r^(7 - idx)], and 5 times that */
static inline void
poly1305_load_rpow(uint32x2_t r[5], uint32x2_t s[5],
                   const poly1305_state_internal_t *st, size_t idx)
{
    int i;

    for (i = 0; i < 5; i++) {
        r[i] = vld1_u32(&st->rpow[i][idx]);
        s[i] = vmul_n_u32(r[i], 5);
    }
}

/* T += a * r, with a different r in each lane */
static inline void
poly1305_mlal(uint64x2_t T[5], const uint32x2_t a[5], const uint32x2_t r[5],
              const uint32x2_t s[5])
{
    T[0] = vmlal_u32(T[0], a[0], r[0]);
    T[1] = vmlal_u32(T[1], a[0], r[1]);
    T[2] = vmlal_u32(T[2], a[0], r[2]);
    T[3] = vmlal_u32(T[3], a[0], r[3]);
    T[4] = vmlal_u32(T[4], a[0], r[4]);
    T[0] = vmlal_u32(T[0], a[1], s[4]);
    T[1] = vmlal_u32(T[1], a[1], r[0]);
    T[2] = vmlal_u32(T[2], a[1], r[1]);
    T[3] = vmlal_u32(T[3], a[1], r[2]);
    T[4] = vmlal_u32(T[4], a[1], r[3]);
    T[0] = vmlal_u32(T[0], a[2], s[3]);
    T[1] = vmlal_u32(T[1], a[2], s[4]);
    T[2] = vmlal_u32(T[2], a[2], r[0]);
    T[3] = vmlal_u32(T[3], a[2], r[1]);
    T[4] = vmlal_u32(T[4], a[2], r[2]);
    T[0] = vmlal_u32(T[0], a[3], s[2]);
    T[1] = vmlal_u32(T[1], a[3], s[3]);
    T[2] = vmlal_u32(T[2], a[3], s[4]);
    T[3] = vmlal_u32(T[3], a[3], r[0]);
    T[4] = vmlal_u32(T[4], a[3], r[1]);
    T[0] = vmlal_u32(T[0], a[4], s[1]);
    T[1] = vmlal_u32(T[1], a[4], s[2]);
    T[2] = vmlal_u32(T[2], a[4], s[3]);
    T[3] = vmlal_u32(T[3], a[4], s[4]);
    T[4] = vmlal_u32(T[4], a[4], r[0]);
}

/*
 * Partial reduction.
 * The result leaves room to add a message limb and still fit in 32 bits.
 */
static inline void
poly1305_reduce(uint32x2_t H[5], uint64x2_t T[5])
{
    const uint64x2_t mask = vdupq_n_u64(0x3ffffff);
    uint64x2_t       c;

    T[1] = vsraq_n_u64(T[1], T[0], 26);
    T[4] = vsraq_n_u64(T[4], T[3], 26);
    T[0] = vandq_u64(T[0], mask);
    T[3] = vandq_u64(T[3], mask);
    T[2] = vsraq_n_u64(T[2], T[1], 26);
    c    = vshrq_n_u64(T[4], 26);
    T[1] = vandq_u64(T[1], mask);
    T[4] = vandq_u64(T[4], mask);
    T[0] = vaddq_u64(T[0], vaddq_u64(c, vshlq_n_u64(c, 2)));
    T[3] = vsraq_n_u64(T[3], T[2], 26);
    T[2] = vandq_u64(T[2], mask);
    T[1] = vsraq_n_u64(T[1], T[0], 26);
    T[0] = vandq_u64(T[0], mask);
    T[4] = vsraq_n_u64(T[4], T[3], 26);
    T[3] = vandq_u64(T[3], mask);

    H[0] = vmovn_u64(T[0]);
    H[1] = vmovn_u64(T[1]);
    H[2] = vmovn_u64(T[2]);
    H[3] = vmovn_u64(T[3]);
    H[4] = vmovn_u64(T[4]);
}

/* r^5..r^8 are computed at once, as [r^4, r^3, r^2, r] * r^4 */
static POLY1305_NOINLINE void
poly1305_compute_powers(poly1305_state_internal_t *st)
{
    uint64_t   r2[3], r3[3], r4[3];
    uint32x4_t X[5], S[5];
    uint64x2_t TL[5], TH[5];
    uint32x2_t lo[5], hi[5];
    int        i;

    poly1305_sqr(r2, st->r);
    poly1305_mul(r3, r2, st->r);
    poly1305_sqr(r4, r2);
    poly1305_store_power(st, st->r, 1);
    poly1305_store_power(st, r2, 2);
    poly1305_store_power(st, r3, 3);
    poly1305_store_power(st, r4, 4);

    for (i = 0; i < 5; i++) {
        X[i]  = vld1q_u32(&st->rpow[i][4]);
        S[i]  = vmulq_n_u32(X[i], 5);
        lo[i] = vget_low_u32(X[i]);
        hi[i] = vget_high_u32(X[i]);
        TL[i] = vdupq_n_u64(0);
        TH[i] = vdupq_n_u64(0);
    }
    POLY1305_MLAL_LANE(TL, lo, X, S, 0);
    POLY1305_MLAL_LANE(TH, hi, X, S, 0);
    poly1305_reduce(lo, TL);
    poly1305_reduce(hi, TH);
    for (i = 0; i < 5; i++) {
        vst1q_u32(&st->rpow[i][0], vcombine_u32(lo[i], hi[i]));
    }

    st->rpow_ready = 1;
}

/*
 * Even blocks go to lane 0 and odd blocks to lane 1.
 * The last pairs use different powers of r in each lane, so that adding both
 * lanes gives the result.
 */
static POLY1305_NOINLINE void
poly1305_blocks_neon(poly1305_state_internal_t *st, const unsigned char *m,
                     unsigned long long bytes)
{
    size_t     npairs = (size_t) (bytes / 32);
    uint64x2_t T[5];
    uint32x2_t H[5];
    uint32x2_t a[5], r[5], s[5];
    uint32_t   h26[5];
    uint64_t   h[3];
    uint64_t   t0, t1, t2, t3, t4, c;
    size_t     idx, p;
    int        i;

    if (!st->rpow_ready) {
        poly1305_compute_powers(st);
    }
    h[0] = st->h[0];
    h[1] = st->h[1];
    h[2] = st->h[2];
    /* with an odd number of blocks, the first one is done here */
    if (bytes & 16) {
        t0 = LOAD64_LE(&m[0]);
        t1 = LOAD64_LE(&m[8]);
        h[0] += t0 & 0xfffffffffff;
        h[1] += ((t0 >> 44) | (t1 << 20)) & 0xfffffffffff;
        h[2] += ((t1 >> 24) & 0x3ffffffffff) | (1ULL << 40);
        poly1305_mul(h, h, st->r);
        m += poly1305_block_size;
    }
    poly1305_radix26_from_44(h26, h);
    for (i = 0; i < 5; i++) {
        H[i] = vset_lane_u32(h26[i], vdup_n_u32(0), 0);
    }

    if (npairs > 4) {
        uint32x4_t R[5], S[5];
        uint32x4_t A[5], B[5];

        for (i = 0; i < 5; i++) {
            R[i] = vuzp1q_u32(vld1q_u32(&st->rpow[i][0]),
                              vld1q_u32(&st->rpow[i][4]));
            S[i] = vmulq_n_u32(R[i], 5);
        }
        do {
            poly1305_load_4blocks(A, m);
            poly1305_load_4blocks(B, m + 64);
            for (i = 0; i < 5; i++) {
                T[i] = vdupq_n_u64(0);
                a[i] = vget_high_u32(A[i]);
            }
            POLY1305_MLAL_LANE(T, a, R, S, 1);
            for (i = 0; i < 5; i++) {
                a[i] = vget_low_u32(B[i]);
            }
            POLY1305_MLAL_LANE(T, a, R, S, 2);
            for (i = 0; i < 5; i++) {
                a[i] = vget_high_u32(B[i]);
            }
            POLY1305_MLAL_LANE(T, a, R, S, 3);
            for (i = 0; i < 5; i++) {
                a[i] = vadd_u32(H[i], vget_low_u32(A[i]));
            }
            POLY1305_MLAL_LANE(T, a, R, S, 0);
            poly1305_reduce(H, T);
            m += 128;
            npairs -= 4;
        } while (npairs > 4);
    }

    /* the pair that needs H goes last, so that the others don't wait for it */
    for (i = 0; i < 5; i++) {
        T[i] = vdupq_n_u64(0);
    }
    idx = 8 - 2 * npairs;
    for (p = 1; p < npairs; p++) {
        poly1305_load_2blocks(a, m + 32 * p);
        poly1305_load_rpow(r, s, st, idx + 2 * p);
        poly1305_mlal(T, a, r, s);
    }
    poly1305_load_2blocks(a, m);
    for (i = 0; i < 5; i++) {
        a[i] = vadd_u32(a[i], H[i]);
    }
    poly1305_load_rpow(r, s, st, idx);
    poly1305_mlal(T, a, r, s);

    t0 = vaddvq_u64(T[0]);
    t1 = vaddvq_u64(T[1]);
    t2 = vaddvq_u64(T[2]);
    t3 = vaddvq_u64(T[3]);
    t4 = vaddvq_u64(T[4]);

    c = t0 >> 26;
    t0 &= 0x3ffffff;
    t1 += c;
    c = t1 >> 26;
    t1 &= 0x3ffffff;
    t2 += c;
    c = t2 >> 26;
    t2 &= 0x3ffffff;
    t3 += c;
    c = t3 >> 26;
    t3 &= 0x3ffffff;
    t4 += c;
    c = t4 >> 26;
    t4 &= 0x3ffffff;
    t0 += c * 5;

    t0 += t1 << 26;
    c = t0 >> 44;
    st->h[0] = t0 & 0xfffffffffff;
    t2 = (t2 << 8) + ((t3 & 0x3ff) << 34) + c;
    c  = t2 >> 44;
    st->h[1] = t2 & 0xfffffffffff;
    st->h[2] = (t3 >> 10) + (t4 << 16) + c;
}

static void
poly1305_blocks(poly1305_state_internal_t *st, const unsigned char *m,
                unsigned long long bytes)
{
    const uint64_t hibit = (st->final) ? 0ULL : (1ULL << 40);
    uint64_t       r0, r1, r2;
    uint64_t       s1, s2;
    uint64_t       h0, h1, h2;
    uint64_t       c;
    uint128_t      d0, d1, d2;

    COMPILER_ASSERT(MIN_VECTOR_BYTES >= 32 && MIN_VECTOR_BYTES_WARM >= 32);
    if (bytes >= (st->rpow_ready ? MIN_VECTOR_BYTES_WARM : MIN_VECTOR_BYTES)) {
        poly1305_blocks_neon(st, m, bytes);
        return;
    }

    r0 = st->r[0];
    r1 = st->r[1];
    r2 = st->r[2];

    h0 = st->h[0];
    h1 = st->h[1];
    h2 = st->h[2];

    s1 = r1 * (5 << 2);
    s2 = r2 * (5 << 2);

    while (bytes >= poly1305_block_size) {
        uint64_t t0, t1;

        t0 = LOAD64_LE(&m[0]);
        t1 = LOAD64_LE(&m[8]);

        h0 += t0 & 0xfffffffffff;
        h1 += ((t0 >> 44) | (t1 << 20)) & 0xfffffffffff;
        h2 += ((t1 >> 24) & 0x3ffffffffff) | hibit;

        d0 = (uint128_t) h0 * r0 + (uint128_t) h1 * s2 + (uint128_t) h2 * s1;
        d1 = (uint128_t) h0 * r1 + (uint128_t) h1 * r0 + (uint128_t) h2 * s2;
        d2 = (uint128_t) h0 * r2 + (uint128_t) h1 * r1 + (uint128_t) h2 * r0;

        c  = (uint64_t) (d0 >> 44);
        h0 = (uint64_t) d0 & 0xfffffffffff;
        d1 += c;
        c  = (uint64_t) (d1 >> 44);
        h1 = (uint64_t) d1 & 0xfffffffffff;
        d2 += c;
        c  = (uint64_t) (d2 >> 42);
        h2 = (uint64_t) d2 & 0x3ffffffffff;
        h0 += c * 5;
        c  = h0 >> 44;
        h0 &= 0xfffffffffff;
        h1 += c;

        m += poly1305_block_size;
        bytes -= poly1305_block_size;
    }

    st->h[0] = h0;
    st->h[1] = h1;
    st->h[2] = h2;
}

static void
poly1305_update(poly1305_state_internal_t *st, const unsigned char *m,
                unsigned long long bytes)
{
    unsigned long long i;

    if (st->leftover) {
        unsigned long long want = (poly1305_block_size - st->leftover);

        if (want > bytes) {
            want = bytes;
        }
        for (i = 0; i < want; i++) {
            st->buffer[st->leftover + i] = m[i];
        }
        bytes -= want;
        m += want;
        st->leftover += want;
        if (st->leftover < poly1305_block_size) {
            return;
        }
        poly1305_blocks(st, st->buffer, poly1305_block_size);
        st->leftover = 0;
    }

    if (bytes >= poly1305_block_size) {
        unsigned long long want = (bytes & ~(poly1305_block_size - 1));

        poly1305_blocks(st, m, want);
        m += want;
        bytes -= want;
    }

    if (bytes) {
        for (i = 0; i < bytes; i++) {
            st->buffer[st->leftover + i] = m[i];
        }
        st->leftover += bytes;
    }
}

static POLY1305_NOINLINE void
poly1305_finish(poly1305_state_internal_t *st, unsigned char mac[16])
{
    uint64_t h0, h1, h2, c;
    uint64_t g0, g1, g2;
    uint64_t t0, t1;
    uint64_t mask;

    if (st->leftover) {
        unsigned long long i = st->leftover;

        st->buffer[i] = 1;
        for (i = i + 1; i < poly1305_block_size; i++) {
            st->buffer[i] = 0;
        }
        st->final = 1;
        poly1305_blocks(st, st->buffer, poly1305_block_size);
    }

    h0 = st->h[0];
    h1 = st->h[1];
    h2 = st->h[2];

    c = h1 >> 44;
    h1 &= 0xfffffffffff;
    h2 += c;
    c = h2 >> 42;
    h2 &= 0x3ffffffffff;
    h0 += c * 5;
    c = h0 >> 44;
    h0 &= 0xfffffffffff;
    h1 += c;
    c = h1 >> 44;
    h1 &= 0xfffffffffff;
    h2 += c;
    c = h2 >> 42;
    h2 &= 0x3ffffffffff;
    h0 += c * 5;
    c = h0 >> 44;
    h0 &= 0xfffffffffff;
    h1 += c;

    g0 = h0 + 5;
    c  = g0 >> 44;
    g0 &= 0xfffffffffff;
    g1 = h1 + c;
    c  = g1 >> 44;
    g1 &= 0xfffffffffff;
    g2 = h2 + c - (1ULL << 42);

    mask = (g2 >> ((sizeof(uint64_t) * 8) - 1)) - 1;
    g0 &= mask;
    g1 &= mask;
    g2 &= mask;
    mask = ~mask;
    h0   = (h0 & mask) | g0;
    h1   = (h1 & mask) | g1;
    h2   = (h2 & mask) | g2;

    t0 = st->pad[0];
    t1 = st->pad[1];

    h0 += t0 & 0xfffffffffff;
    c = h0 >> 44;
    h0 &= 0xfffffffffff;
    h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff) + c;
    c = h1 >> 44;
    h1 &= 0xfffffffffff;
    h2 += ((t1 >> 24) & 0x3ffffffffff) + c;
    h2 &= 0x3ffffffffff;

    h0 = h0 | (h1 << 44);
    h1 = (h1 >> 20) | (h2 << 24);

    STORE64_LE(&mac[0], h0);
    STORE64_LE(&mac[8], h1);

    sodium_memzero((void *) st, sizeof *st);
}

static int
crypto_onetimeauth_poly1305_neon(unsigned char *out, const unsigned char *m,
                                 unsigned long long   inlen,
                                 const unsigned char *key)
{
    CRYPTO_ALIGN(64) poly1305_state_internal_t state;

    poly1305_init(&state, key);
    poly1305_update(&state, m, inlen);
    poly1305_finish(&state, out);

    return 0;
}

static int
crypto_onetimeauth_poly1305_neon_init(crypto_onetimeauth_poly1305_state *state,
                                      const unsigned char *key)
{
    COMPILER_ASSERT(sizeof(crypto_onetimeauth_poly1305_state) >=
                    sizeof(poly1305_state_internal_t));
    poly1305_init((poly1305_state_internal_t *) (void *) state, key);

    return 0;
}

static int
crypto_onetimeauth_poly1305_neon_update(
    crypto_onetimeauth_poly1305_state *state, const unsigned char *in,
    unsigned long long inlen)
{
    poly1305_update((poly1305_state_internal_t *) (void *) state, in, inlen);

    return 0;
}

static int
crypto_onetimeauth_poly1305_neon_final(crypto_onetimeauth_poly1305_state *state,
                                       unsigned char *out)
{
    poly1305_finish((poly1305_state_internal_t *) (void *) state, out);

    return 0;
}

static int
crypto_onetimeauth_poly1305_neon_verify(const unsigned char *h,
                                        const unsigned char *in,
                                        unsigned long long   inlen,
                                        const unsigned char *k)
{
    unsigned char correct[16];

    crypto_onetimeauth_poly1305_neon(correct, in, inlen, k);

    return crypto_verify_16(h, correct);
}

struct crypto_onetimeauth_poly1305_implementation
    crypto_onetimeauth_poly1305_neon_implementation = {
        SODIUM_C99(.onetimeauth =) crypto_onetimeauth_poly1305_neon,
        SODIUM_C99(.onetimeauth_verify =)
            crypto_onetimeauth_poly1305_neon_verify,
        SODIUM_C99(.onetimeauth_init =) crypto_onetimeauth_poly1305_neon_init,
        SODIUM_C99(.onetimeauth_update =)
            crypto_onetimeauth_poly1305_neon_update,
        SODIUM_C99(.onetimeauth_final =) crypto_onetimeauth_poly1305_neon_final
    };

#endif
