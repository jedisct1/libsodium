
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_stream_forro14.h"
#include "private/common.h"
#include "utils.h"

#if defined(HAVE_EMMINTRIN_H) && defined(HAVE_TMMINTRIN_H)

#ifdef __GNUC__
#pragma GCC target("sse2")
#pragma GCC target("ssse3")
#endif

#include <emmintrin.h>
#include <tmmintrin.h>
#include "private/sse2_64_32.h"

#include "../stream_forro14.h"
#include "forro14_dolbeau-ssse3.h"

#define ROUNDS 20

typedef struct forro_ctx
{
    uint32_t input[16];
} forro_ctx;

static void
forro_keysetup(forro_ctx *ctx, const uint8_t *k)
{
    ctx->input[0] = 0x61707865;
    ctx->input[1] = 0x3320646e;
    ctx->input[2] = 0x79622d32;
    ctx->input[3] = 0x6b206574;
    ctx->input[4] = LOAD32_LE(k + 0);
    ctx->input[5] = LOAD32_LE(k + 4);
    ctx->input[6] = LOAD32_LE(k + 8);
    ctx->input[7] = LOAD32_LE(k + 12);
    ctx->input[8] = LOAD32_LE(k + 16);
    ctx->input[9] = LOAD32_LE(k + 20);
    ctx->input[10] = LOAD32_LE(k + 24);
    ctx->input[11] = LOAD32_LE(k + 28);
}

static void
forro_ivsetup(forro_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[12] = counter == NULL ? 0 : LOAD32_LE(counter + 0);
    ctx->input[13] = counter == NULL ? 0 : LOAD32_LE(counter + 4);
    ctx->input[14] = LOAD32_LE(iv + 0);
    ctx->input[15] = LOAD32_LE(iv + 4);
}

static void
forro_ietf_ivsetup(forro_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[12] = counter == NULL ? 0 : LOAD32_LE(counter);
    ctx->input[13] = LOAD32_LE(iv + 0);
    ctx->input[14] = LOAD32_LE(iv + 4);
    ctx->input[15] = LOAD32_LE(iv + 8);
}

static void
forro14_encrypt_bytes(forro_ctx *ctx, const uint8_t *m, uint8_t *c,
                      unsigned long long bytes)
{
    uint32_t *const x = &ctx->input[0];

    if (!bytes)
    {
        return; /* LCOV_EXCL_LINE */
    }
#include "u4.h"
#include "u1.h"
#include "u0.h"
}

static int
stream_ref(unsigned char *c, unsigned long long clen, const unsigned char *n,
           const unsigned char *k)
{
    struct forro_ctx ctx;

    if (!clen)
    {
        return 0;
    }
    COMPILER_ASSERT(crypto_stream_forro14_KEYBYTES == 256 / 8);
    forro_keysetup(&ctx, k);
    forro_ivsetup(&ctx, n, NULL);
    memset(c, 0, clen);
    forro14_encrypt_bytes(&ctx, c, c, clen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_ietf_ext_ref(unsigned char *c, unsigned long long clen,
                    const unsigned char *n, const unsigned char *k)
{
    struct forro_ctx ctx;

    if (!clen)
    {
        return 0;
    }
    COMPILER_ASSERT(crypto_stream_forro14_KEYBYTES == 256 / 8);
    forro_keysetup(&ctx, k);
    forro_ietf_ivsetup(&ctx, n, NULL);
    memset(c, 0, clen);
    forro14_encrypt_bytes(&ctx, c, c, clen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_ref_xor_ic(unsigned char *c, const unsigned char *m,
                  unsigned long long mlen, const unsigned char *n, uint64_t ic,
                  const unsigned char *k)
{
    struct forro_ctx ctx;
    uint8_t ic_bytes[8];
    uint32_t ic_high;
    uint32_t ic_low;

    if (!mlen)
    {
        return 0;
    }
    ic_high = (uint32_t)(ic >> 32);
    ic_low = (uint32_t)ic;
    STORE32_LE(&ic_bytes[0], ic_low);
    STORE32_LE(&ic_bytes[4], ic_high);
    forro_keysetup(&ctx, k);
    forro_ivsetup(&ctx, n, ic_bytes);
    forro14_encrypt_bytes(&ctx, m, c, mlen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

static int
stream_ietf_ext_ref_xor_ic(unsigned char *c, const unsigned char *m,
                           unsigned long long mlen, const unsigned char *n,
                           uint32_t ic, const unsigned char *k)
{
    struct forro_ctx ctx;
    uint8_t ic_bytes[4];

    if (!mlen)
    {
        return 0;
    }
    STORE32_LE(ic_bytes, ic);
    forro_keysetup(&ctx, k);
    forro_ietf_ivsetup(&ctx, n, ic_bytes);
    forro14_encrypt_bytes(&ctx, m, c, mlen);
    sodium_memzero(&ctx, sizeof ctx);

    return 0;
}

struct crypto_stream_forro14_implementation
    crypto_stream_forro14_dolbeau_ssse3_implementation = {
        SODIUM_C99(.stream =) stream_ref,
        SODIUM_C99(.stream_ietf_ext =) stream_ietf_ext_ref,
        SODIUM_C99(.stream_xor_ic =) stream_ref_xor_ic,
        SODIUM_C99(.stream_ietf_ext_xor_ic =) stream_ietf_ext_ref_xor_ic};

#endif
