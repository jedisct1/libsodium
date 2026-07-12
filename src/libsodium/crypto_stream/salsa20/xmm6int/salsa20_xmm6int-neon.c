
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "crypto_stream_salsa20.h"
#include "private/common.h"
#include "utils.h"

#if (defined(__aarch64__) || defined(_M_ARM64)) && \
    (defined(__ARM_NEON) || defined(__ARM_NEON__))

# include <arm_neon.h>

# include "../ref/salsa20_ref.h"
# include "../stream_salsa20.h"
# include "salsa20_xmm6int-neon.h"

# define ROUNDS 20

# define MIN_VECTOR_BYTES 256

typedef struct salsa_ctx {
    uint32_t input[16];
} salsa_ctx;

static const int TR[16] = {
    0, 5, 10, 15, 12, 1, 6, 11, 8, 13, 2, 7, 4, 9, 14, 3
};

static inline uint32x4_t
rotl32_7_neon(uint32x4_t x)
{
    return vsriq_n_u32(vshlq_n_u32(x, 7), x, 25);
}

static inline uint32x4_t
rotl32_9_neon(uint32x4_t x)
{
    return vsriq_n_u32(vshlq_n_u32(x, 9), x, 23);
}

static inline uint32x4_t
rotl32_13_neon(uint32x4_t x)
{
    return vsriq_n_u32(vshlq_n_u32(x, 13), x, 19);
}

static inline uint32x4_t
rotl32_18_neon(uint32x4_t x)
{
    return vsriq_n_u32(vshlq_n_u32(x, 18), x, 14);
}

static inline uint32x4x4_t
undiagonalize_neon(uint32x4_t diag0, uint32x4_t diag1, uint32x4_t diag2,
                   uint32x4_t diag3)
{
    uint32x4_t   t0, t1, t2, t3;
    uint32x4x4_t rows;

    diag1 = vextq_u32(diag1, diag1, 1);
    diag2 = vextq_u32(diag2, diag2, 2);
    diag3 = vextq_u32(diag3, diag3, 3);

    t0 = vtrn1q_u32(diag0, diag1);
    t1 = vtrn2q_u32(diag0, diag1);
    t2 = vtrn1q_u32(diag2, diag3);
    t3 = vtrn2q_u32(diag2, diag3);

    diag0 = vreinterpretq_u32_u64(
        vtrn1q_u64(vreinterpretq_u64_u32(t0), vreinterpretq_u64_u32(t2)));
    diag1 = vreinterpretq_u32_u64(
        vtrn1q_u64(vreinterpretq_u64_u32(t1), vreinterpretq_u64_u32(t3)));
    diag2 = vreinterpretq_u32_u64(
        vtrn2q_u64(vreinterpretq_u64_u32(t0), vreinterpretq_u64_u32(t2)));
    diag3 = vreinterpretq_u32_u64(
        vtrn2q_u64(vreinterpretq_u64_u32(t1), vreinterpretq_u64_u32(t3)));

    rows.val[0] = diag0;
    rows.val[1] = vextq_u32(diag1, diag1, 3);
    rows.val[2] = vextq_u32(diag2, diag2, 2);
    rows.val[3] = vextq_u32(diag3, diag3, 1);

    return rows;
}

static void
salsa_keysetup(salsa_ctx *ctx, const uint8_t *k)
{
    ctx->input[TR[1]]  = LOAD32_LE(k + 0);
    ctx->input[TR[2]]  = LOAD32_LE(k + 4);
    ctx->input[TR[3]]  = LOAD32_LE(k + 8);
    ctx->input[TR[4]]  = LOAD32_LE(k + 12);
    ctx->input[TR[11]] = LOAD32_LE(k + 16);
    ctx->input[TR[12]] = LOAD32_LE(k + 20);
    ctx->input[TR[13]] = LOAD32_LE(k + 24);
    ctx->input[TR[14]] = LOAD32_LE(k + 28);
    ctx->input[TR[0]]  = 0x61707865;
    ctx->input[TR[5]]  = 0x3320646e;
    ctx->input[TR[10]] = 0x79622d32;
    ctx->input[TR[15]] = 0x6b206574;
}

static void
salsa_ivsetup(salsa_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[TR[6]] = LOAD32_LE(iv + 0);
    ctx->input[TR[7]] = LOAD32_LE(iv + 4);
    ctx->input[TR[8]] = counter == NULL ? 0 : LOAD32_LE(counter + 0);
    ctx->input[TR[9]] = counter == NULL ? 0 : LOAD32_LE(counter + 4);
}

static void
salsa20_encrypt_bytes(salsa_ctx *ctx, const uint8_t *m, uint8_t *c,
                      unsigned long long bytes)
{
    uint32_t * const x = &ctx->input[0];

    if (!bytes) {
        return; /* LCOV_EXCL_LINE */
    }

#include "u4-neon.h"
#include "u2-neon.h"
#include "u1-neon.h"
#include "u0-neon.h"
}

static int
stream_neon(unsigned char *c, unsigned long long clen, const unsigned char *n,
            const unsigned char *k)
{
    struct salsa_ctx ctx;

    if (clen < MIN_VECTOR_BYTES) {
        return crypto_stream_salsa20_ref_implementation.stream(c, clen, n, k);
    }
    COMPILER_ASSERT(crypto_stream_salsa20_KEYBYTES == 256 / 8);
    salsa_keysetup(&ctx, k);
    salsa_ivsetup(&ctx, n, NULL);
    memset(c, 0, clen);
    salsa20_encrypt_bytes(&ctx, c, c, clen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_neon_xor_ic(unsigned char *c, const unsigned char *m,
                   unsigned long long mlen, const unsigned char *n, uint64_t ic,
                   const unsigned char *k)
{
    struct salsa_ctx ctx;
    uint8_t          ic_bytes[8];
    uint32_t         ic_high;
    uint32_t         ic_low;

    if (mlen < MIN_VECTOR_BYTES) {
        return crypto_stream_salsa20_ref_implementation.stream_xor_ic(
            c, m, mlen, n, ic, k);
    }
    ic_high = (uint32_t) (ic >> 32);
    ic_low  = (uint32_t) (ic);
    STORE32_LE(&ic_bytes[0], ic_low);
    STORE32_LE(&ic_bytes[4], ic_high);
    salsa_keysetup(&ctx, k);
    salsa_ivsetup(&ctx, n, ic_bytes);
    salsa20_encrypt_bytes(&ctx, m, c, mlen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

struct crypto_stream_salsa20_implementation
    crypto_stream_salsa20_xmm6int_neon_implementation = {
        SODIUM_C99(.stream =) stream_neon,
        SODIUM_C99(.stream_xor_ic =) stream_neon_xor_ic
    };

#endif
