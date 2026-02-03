#ifndef blamka_round_wasm32_H
#define blamka_round_wasm32_H

#include <wasm_simd128.h>
#include "private/common.h"

static inline v128_t
fBlaMka_wasm(v128_t x, v128_t y)
{
    const v128_t z = wasm_u64x2_extmul_low_u32x4(x, y);
    return wasm_i64x2_add(wasm_i64x2_add(x, y), wasm_i64x2_add(z, z));
}

static inline v128_t
rotr64_32_wasm(v128_t x)
{
    return wasm_i32x4_shuffle(x, x, 1, 0, 3, 2);
}

static inline v128_t
rotr64_24_wasm(v128_t x)
{
    return wasm_v128_or(wasm_u64x2_shr(x, 24), wasm_i64x2_shl(x, 40));
}

static inline v128_t
rotr64_16_wasm(v128_t x)
{
    return wasm_v128_or(wasm_u64x2_shr(x, 16), wasm_i64x2_shl(x, 48));
}

static inline v128_t
rotr64_63_wasm(v128_t x)
{
    return wasm_v128_or(wasm_u64x2_shr(x, 63), wasm_i64x2_shl(x, 1));
}

#define G1_WASM(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                        \
        A0 = fBlaMka_wasm(A0, B0);              \
        A1 = fBlaMka_wasm(A1, B1);              \
                                                \
        D0 = wasm_v128_xor(D0, A0);             \
        D1 = wasm_v128_xor(D1, A1);             \
                                                \
        D0 = rotr64_32_wasm(D0);                \
        D1 = rotr64_32_wasm(D1);                \
                                                \
        C0 = fBlaMka_wasm(C0, D0);              \
        C1 = fBlaMka_wasm(C1, D1);              \
                                                \
        B0 = wasm_v128_xor(B0, C0);             \
        B1 = wasm_v128_xor(B1, C1);             \
                                                \
        B0 = rotr64_24_wasm(B0);                \
        B1 = rotr64_24_wasm(B1);                \
    } while ((void) 0, 0)

#define G2_WASM(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                        \
        A0 = fBlaMka_wasm(A0, B0);              \
        A1 = fBlaMka_wasm(A1, B1);              \
                                                \
        D0 = wasm_v128_xor(D0, A0);             \
        D1 = wasm_v128_xor(D1, A1);             \
                                                \
        D0 = rotr64_16_wasm(D0);                \
        D1 = rotr64_16_wasm(D1);                \
                                                \
        C0 = fBlaMka_wasm(C0, D0);              \
        C1 = fBlaMka_wasm(C1, D1);              \
                                                \
        B0 = wasm_v128_xor(B0, C0);             \
        B1 = wasm_v128_xor(B1, C1);             \
                                                \
        B0 = rotr64_63_wasm(B0);                \
        B1 = rotr64_63_wasm(B1);                \
    } while ((void) 0, 0)

static inline v128_t
wasm_alignr_8(v128_t a, v128_t b)
{
    return wasm_i8x16_shuffle(b, a, 8, 9, 10, 11, 12, 13, 14, 15,
                              16, 17, 18, 19, 20, 21, 22, 23);
}

#define DIAGONALIZE_WASM(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                                 \
        v128_t t0 = wasm_alignr_8(B1, B0);               \
        v128_t t1 = wasm_alignr_8(B0, B1);               \
        B0        = t0;                                  \
        B1        = t1;                                  \
                                                         \
        t0 = C0;                                         \
        C0 = C1;                                         \
        C1 = t0;                                         \
                                                         \
        t0 = wasm_alignr_8(D1, D0);                      \
        t1 = wasm_alignr_8(D0, D1);                      \
        D0 = t1;                                         \
        D1 = t0;                                         \
    } while ((void) 0, 0)

#define UNDIAGONALIZE_WASM(A0, B0, C0, D0, A1, B1, C1, D1) \
    do {                                                   \
        v128_t t0 = wasm_alignr_8(B0, B1);                 \
        v128_t t1 = wasm_alignr_8(B1, B0);                 \
        B0        = t0;                                    \
        B1        = t1;                                    \
                                                           \
        t0 = C0;                                           \
        C0 = C1;                                           \
        C1 = t0;                                           \
                                                           \
        t0 = wasm_alignr_8(D0, D1);                        \
        t1 = wasm_alignr_8(D1, D0);                        \
        D0 = t1;                                           \
        D1 = t0;                                           \
    } while ((void) 0, 0)

#define BLAKE2_ROUND_WASM(A0, A1, B0, B1, C0, C1, D0, D1)   \
    do {                                                    \
        G1_WASM(A0, B0, C0, D0, A1, B1, C1, D1);            \
        G2_WASM(A0, B0, C0, D0, A1, B1, C1, D1);            \
                                                            \
        DIAGONALIZE_WASM(A0, B0, C0, D0, A1, B1, C1, D1);   \
                                                            \
        G1_WASM(A0, B0, C0, D0, A1, B1, C1, D1);            \
        G2_WASM(A0, B0, C0, D0, A1, B1, C1, D1);            \
                                                            \
        UNDIAGONALIZE_WASM(A0, B0, C0, D0, A1, B1, C1, D1); \
    } while ((void) 0, 0)

#endif
