
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

#define U32TO8_LITTLE(p, v) (((uint32_t *)(p))[0] = v)
#define U32C(v) (v##U)
#define U32V(v) ((uint32_t)(v)&U32C(0xFFFFFFFF))

#define ROUNDS 14

#define ROTATE(v, c) (ROTL32(v, c))
#define XOR(v, w) ((v) ^ (w))
#define PLUS(v, w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v), 1))

#define QUARTERROUND(a, b, c, d, e) \
    d = PLUS(d, e);                 \
    c = XOR(c, d);                  \
    b = ROTATE(PLUS(b, c), 10);     \
    a = PLUS(a, b);                 \
    e = XOR(e, a);                  \
    d = ROTATE(PLUS(d, e), 27);     \
    c = PLUS(c, d);                 \
    b = XOR(b, c);                  \
    a = ROTATE(PLUS(a, b), 8);

typedef struct forro_ctx
{
    uint32_t input[16];
} forro_ctx;

static void
forro_keysetup(forro_ctx *ctx, const uint8_t *k)
{
    ctx->input[0] = LOAD32_LE(k + 0);
    ctx->input[1] = LOAD32_LE(k + 4);
    ctx->input[2] = LOAD32_LE(k + 8);
    ctx->input[3] = LOAD32_LE(k + 12);
    ctx->input[6] = 0x746C6F76;
    ctx->input[7] = 0x61616461;
    ctx->input[8] = LOAD32_LE(k + 16);
    ctx->input[9] = LOAD32_LE(k + 20);
    ctx->input[10] = LOAD32_LE(k + 24);
    ctx->input[11] = LOAD32_LE(k + 28);
    ctx->input[14] = 0x72626173;
    ctx->input[15] = 0x61636E61;
}

static void
forro_ivsetup(forro_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[4] = counter == NULL ? 0 : LOAD32_LE(counter + 0);
    ctx->input[5] = counter == NULL ? 0 : LOAD32_LE(counter + 4);
    ctx->input[12] = LOAD32_LE(iv + 0);
    ctx->input[13] = LOAD32_LE(iv + 4);
}

static void
forro_ietf_ivsetup(forro_ctx *ctx, const uint8_t *iv, const uint8_t *counter)
{
    ctx->input[4] = counter == NULL ? 0 : LOAD32_LE(counter);
    ctx->input[5] = LOAD32_LE(iv + 0);
    ctx->input[12] = LOAD32_LE(iv + 4);
    ctx->input[13] = LOAD32_LE(iv + 8);
}

static void
forro(uint8_t output[64], const uint32_t input[16])
{
    uint32_t x[16];
    int i;

    for (i = 0; i < 16; ++i)
        x[i] = input[i];
    for (i = ROUNDS; i > 0; i -= 2)
    {
        QUARTERROUND(x[0], x[4], x[8], x[12], x[3])
        QUARTERROUND(x[1], x[5], x[9], x[13], x[0])
        QUARTERROUND(x[2], x[6], x[10], x[14], x[1])
        QUARTERROUND(x[3], x[7], x[11], x[15], x[2])
        QUARTERROUND(x[0], x[5], x[10], x[15], x[3])
        QUARTERROUND(x[1], x[6], x[11], x[12], x[0])
        QUARTERROUND(x[2], x[7], x[8], x[13], x[1])
        QUARTERROUND(x[3], x[4], x[9], x[14], x[2])
    }
    for (i = 0; i < 16; ++i)
        x[i] = PLUS(x[i], input[i]);
    for (i = 0; i < 16; ++i)
        U32TO8_LITTLE(output + 4 * i, x[i]);
}

static void
forro14_encrypt_bytes(forro_ctx *ctx, const uint8_t *m, uint8_t *c,
                      unsigned long long bytes)
{
    uint8_t output[64];
    int i;
    uint32_t *x = (uint32_t *)&ctx->input;
    uint8_t *out = c;

#include "u4.h"

    if (!bytes)
        return;
    for (;;)
    {
        forro(output, x);
        x[4] = PLUSONE(x[4]);
        if (!x[4])
        {
            x[5] = PLUSONE(x[5]);
            /* stopping at 2^70 bytes per nonce is user's responsibility */
        }
        if (bytes <= 64)
        {
            for (i = 0; i < bytes; ++i)
                out[i] = m[i] ^ output[i];
            return;
        }
        for (i = 0; i < 64; ++i)
            out[i] = m[i] ^ output[i];
        bytes -= 64;
        out += 64;
        m += 64;
    }
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
