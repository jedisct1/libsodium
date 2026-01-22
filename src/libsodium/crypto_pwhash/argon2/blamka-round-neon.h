#ifndef blamka_round_neon_H
#define blamka_round_neon_H

#include <arm_neon.h>

#include "private/common.h"

static inline uint64x2_t
fBlaMka_neon(uint64x2_t x, uint64x2_t y)
{
    const uint32x2_t x_lo = vmovn_u64(x);
    const uint32x2_t y_lo = vmovn_u64(y);
    const uint64x2_t z    = vmull_u32(x_lo, y_lo);

    return vaddq_u64(vaddq_u64(x, y), vaddq_u64(z, z));
}

static inline uint64x2_t
rotr64_32_neon(uint64x2_t x)
{
    return vreinterpretq_u64_u32(vrev64q_u32(vreinterpretq_u32_u64(x)));
}

static inline uint64x2_t
rotr64_24_neon(uint64x2_t x)
{
    static const uint8_t rot24_tbl[16] = {
        3, 4, 5, 6, 7, 0, 1, 2,
        11, 12, 13, 14, 15, 8, 9, 10
    };
    const uint8x16_t tbl = vld1q_u8(rot24_tbl);
    return vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(x), tbl));
}

static inline uint64x2_t
rotr64_16_neon(uint64x2_t x)
{
    static const uint8_t rot16_tbl[16] = {
        2, 3, 4, 5, 6, 7, 0, 1,
        10, 11, 12, 13, 14, 15, 8, 9
    };
    const uint8x16_t tbl = vld1q_u8(rot16_tbl);
    return vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(x), tbl));
}

static inline uint64x2_t
rotr64_63_neon(uint64x2_t x)
{
    return veorq_u64(vshrq_n_u64(x, 63), vaddq_u64(x, x));
}

#define G1_NEON(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                        \
        A0 = fBlaMka_neon(A0, B0);              \
        A1 = fBlaMka_neon(A1, B1);              \
                                                \
        D0 = veorq_u64(D0, A0);                 \
        D1 = veorq_u64(D1, A1);                 \
                                                \
        D0 = rotr64_32_neon(D0);                \
        D1 = rotr64_32_neon(D1);                \
                                                \
        C0 = fBlaMka_neon(C0, D0);              \
        C1 = fBlaMka_neon(C1, D1);              \
                                                \
        B0 = veorq_u64(B0, C0);                 \
        B1 = veorq_u64(B1, C1);                 \
                                                \
        B0 = rotr64_24_neon(B0);                \
        B1 = rotr64_24_neon(B1);                \
    } while ((void) 0, 0)

#define G2_NEON(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                        \
        A0 = fBlaMka_neon(A0, B0);              \
        A1 = fBlaMka_neon(A1, B1);              \
                                                \
        D0 = veorq_u64(D0, A0);                 \
        D1 = veorq_u64(D1, A1);                 \
                                                \
        D0 = rotr64_16_neon(D0);                \
        D1 = rotr64_16_neon(D1);                \
                                                \
        C0 = fBlaMka_neon(C0, D0);              \
        C1 = fBlaMka_neon(C1, D1);              \
                                                \
        B0 = veorq_u64(B0, C0);                 \
        B1 = veorq_u64(B1, C1);                 \
                                                \
        B0 = rotr64_63_neon(B0);                \
        B1 = rotr64_63_neon(B1);                \
    } while ((void) 0, 0)

#define DIAGONALIZE_NEON(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                                 \
        uint64x2_t t0, t1;                               \
                                                         \
        t0 = vreinterpretq_u64_u8(                       \
            vextq_u8(vreinterpretq_u8_u64(B0),           \
                     vreinterpretq_u8_u64(B1), 8));      \
        t1 = vreinterpretq_u64_u8(                       \
            vextq_u8(vreinterpretq_u8_u64(B1),           \
                     vreinterpretq_u8_u64(B0), 8));      \
        B0 = t0;                                         \
        B1 = t1;                                         \
                                                         \
        t0 = C0;                                         \
        C0 = C1;                                         \
        C1 = t0;                                         \
                                                         \
        t0 = vreinterpretq_u64_u8(                       \
            vextq_u8(vreinterpretq_u8_u64(D0),           \
                     vreinterpretq_u8_u64(D1), 8));      \
        t1 = vreinterpretq_u64_u8(                       \
            vextq_u8(vreinterpretq_u8_u64(D1),           \
                     vreinterpretq_u8_u64(D0), 8));      \
        D0 = t1;                                         \
        D1 = t0;                                         \
    } while ((void) 0, 0)

#define UNDIAGONALIZE_NEON(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                                   \
        uint64x2_t t0, t1;                                 \
                                                           \
        t0 = vreinterpretq_u64_u8(                         \
            vextq_u8(vreinterpretq_u8_u64(B1),             \
                     vreinterpretq_u8_u64(B0), 8));        \
        t1 = vreinterpretq_u64_u8(                         \
            vextq_u8(vreinterpretq_u8_u64(B0),             \
                     vreinterpretq_u8_u64(B1), 8));        \
        B0 = t0;                                           \
        B1 = t1;                                           \
                                                           \
        t0 = C0;                                           \
        C0 = C1;                                           \
        C1 = t0;                                           \
                                                           \
        t0 = vreinterpretq_u64_u8(                         \
            vextq_u8(vreinterpretq_u8_u64(D1),             \
                     vreinterpretq_u8_u64(D0), 8));        \
        t1 = vreinterpretq_u64_u8(                         \
            vextq_u8(vreinterpretq_u8_u64(D0),             \
                     vreinterpretq_u8_u64(D1), 8));        \
        D0 = t1;                                           \
        D1 = t0;                                           \
    } while ((void) 0, 0)

#define BLAKE2_ROUND_NEON(A0, A1, B0, B1, C0, C1, D0, D1)   \
    do {                                                    \
        G1_NEON(A0, B0, C0, D0, A1, B1, C1, D1);            \
        G2_NEON(A0, B0, C0, D0, A1, B1, C1, D1);            \
                                                            \
        DIAGONALIZE_NEON(A0, B0, C0, D0, A1, B1, C1, D1);   \
                                                            \
        G1_NEON(A0, B0, C0, D0, A1, B1, C1, D1);            \
        G2_NEON(A0, B0, C0, D0, A1, B1, C1, D1);            \
                                                            \
        UNDIAGONALIZE_NEON(A0, B0, C0, D0, A1, B1, C1, D1); \
    } while ((void) 0, 0)

#endif
