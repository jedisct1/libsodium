
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_stream_chacha20.h"
#include "private/common.h"
#include "utils.h"

#if (defined(__aarch64__) || defined(_M_ARM64)) && \
    (defined(__ARM_NEON) || defined(__ARM_NEON__))

# include <arm_neon.h>

# include "../ref/chacha20_ref.h"
# include "../stream_chacha20.h"
# include "chacha20_dolbeau-neon.h"

# define ROUNDS 20

# define MIN_VECTOR_BYTES 128

typedef struct chacha_ctx {
    uint32_t input[16];
} chacha_ctx;

static inline uint32x4_t
rotl32_16_neon(uint32x4_t x)
{
    return vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(x)));
}

static inline uint32x4_t
rotl32_12_neon(uint32x4_t x)
{
    return vsriq_n_u32(vshlq_n_u32(x, 12), x, 20);
}

static inline uint32x4_t
rotl32_8_neon(uint32x4_t x)
{
    static const uint8_t rot8_tbl[16] = {
        3, 0, 1, 2, 7, 4, 5, 6,
        11, 8, 9, 10, 15, 12, 13, 14
    };
    const uint8x16_t tbl = vld1q_u8(rot8_tbl);

    return vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(x), tbl));
}

static inline uint32x4_t
rotl32_7_neon(uint32x4_t x)
{
    return vsriq_n_u32(vshlq_n_u32(x, 7), x, 25);
}

static void
chacha_keysetup(chacha_ctx *ctx, const uint8_t *k)
{
    ctx->input[0]  = 0x61707865;
    ctx->input[1]  = 0x3320646e;
    ctx->input[2]  = 0x79622d32;
    ctx->input[3]  = 0x6b206574;
    ctx->input[4]  = LOAD32_LE(k + 0);
    ctx->input[5]  = LOAD32_LE(k + 4);
    ctx->input[6]  = LOAD32_LE(k + 8);
    ctx->input[7]  = LOAD32_LE(k + 12);
    ctx->input[8]  = LOAD32_LE(k + 16);
    ctx->input[9]  = LOAD32_LE(k + 20);
    ctx->input[10] = LOAD32_LE(k + 24);
    ctx->input[11] = LOAD32_LE(k + 28);
}

static void
chacha_ivsetup(chacha_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[12] = counter == NULL ? 0 : LOAD32_LE(counter + 0);
    ctx->input[13] = counter == NULL ? 0 : LOAD32_LE(counter + 4);
    ctx->input[14] = LOAD32_LE(iv + 0);
    ctx->input[15] = LOAD32_LE(iv + 4);
}

static void
chacha_ietf_ivsetup(chacha_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[12] = counter == NULL ? 0 : LOAD32_LE(counter);
    ctx->input[13] = LOAD32_LE(iv + 0);
    ctx->input[14] = LOAD32_LE(iv + 4);
    ctx->input[15] = LOAD32_LE(iv + 8);
}

static void
chacha20_encrypt_bytes(chacha_ctx *ctx, const uint8_t *m, uint8_t *c,
                       unsigned long long bytes)
{
    uint32_t * const x = &ctx->input[0];

    if (!bytes) {
        return; /* LCOV_EXCL_LINE */
    }
# include "u4-neon.h"
# include "u2-neon.h"
# include "u1-neon.h"
# include "u0-neon.h"
}

static int
stream_neon(unsigned char *c, unsigned long long clen, const unsigned char *n,
            const unsigned char *k)
{
    struct chacha_ctx ctx;

    if (clen < MIN_VECTOR_BYTES) {
        return crypto_stream_chacha20_ref_implementation.stream(c, clen, n, k);
    }
    COMPILER_ASSERT(crypto_stream_chacha20_KEYBYTES == 256 / 8);
    chacha_keysetup(&ctx, k);
    chacha_ivsetup(&ctx, n, NULL);
    memset(c, 0, clen);
    chacha20_encrypt_bytes(&ctx, c, c, clen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_ietf_ext_neon(unsigned char *c, unsigned long long clen,
                     const unsigned char *n, const unsigned char *k)
{
    struct chacha_ctx ctx;

    if (clen < MIN_VECTOR_BYTES) {
        return crypto_stream_chacha20_ref_implementation.stream_ietf_ext(
            c, clen, n, k);
    }
    COMPILER_ASSERT(crypto_stream_chacha20_KEYBYTES == 256 / 8);
    chacha_keysetup(&ctx, k);
    chacha_ietf_ivsetup(&ctx, n, NULL);
    memset(c, 0, clen);
    chacha20_encrypt_bytes(&ctx, c, c, clen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_neon_xor_ic(unsigned char *c, const unsigned char *m,
                   unsigned long long mlen, const unsigned char *n,
                   uint64_t ic, const unsigned char *k)
{
    struct chacha_ctx ctx;
    uint8_t           ic_bytes[8];
    uint32_t          ic_high;
    uint32_t          ic_low;

    if (mlen < MIN_VECTOR_BYTES) {
        return crypto_stream_chacha20_ref_implementation.stream_xor_ic(
            c, m, mlen, n, ic, k);
    }
    ic_high = (uint32_t) (ic >> 32);
    ic_low  = (uint32_t) ic;
    STORE32_LE(&ic_bytes[0], ic_low);
    STORE32_LE(&ic_bytes[4], ic_high);
    chacha_keysetup(&ctx, k);
    chacha_ivsetup(&ctx, n, ic_bytes);
    chacha20_encrypt_bytes(&ctx, m, c, mlen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_ietf_ext_neon_xor_ic(unsigned char *c, const unsigned char *m,
                            unsigned long long mlen, const unsigned char *n,
                            uint32_t ic, const unsigned char *k)
{
    struct chacha_ctx ctx;
    uint8_t           ic_bytes[4];

    if (mlen < MIN_VECTOR_BYTES) {
        return crypto_stream_chacha20_ref_implementation.stream_ietf_ext_xor_ic(
            c, m, mlen, n, ic, k);
    }
    STORE32_LE(ic_bytes, ic);
    chacha_keysetup(&ctx, k);
    chacha_ietf_ivsetup(&ctx, n, ic_bytes);
    chacha20_encrypt_bytes(&ctx, m, c, mlen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

struct crypto_stream_chacha20_implementation
    crypto_stream_chacha20_dolbeau_neon_implementation = {
        SODIUM_C99(.stream =) stream_neon,
        SODIUM_C99(.stream_ietf_ext =) stream_ietf_ext_neon,
        SODIUM_C99(.stream_xor_ic =) stream_neon_xor_ic,
        SODIUM_C99(.stream_ietf_ext_xor_ic =) stream_ietf_ext_neon_xor_ic
    };

#endif
