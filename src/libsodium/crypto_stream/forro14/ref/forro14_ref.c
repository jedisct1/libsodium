
/*
 forro-merged.c version 20080118
 D. J. Bernstein
 Public domain.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_stream_forro14.h"
#include "private/common.h"
#include "utils.h"

#include "../stream_forro14.h"
#include "forro14_ref.h"

struct forro_ctx
{
    uint32_t input[16];
};

typedef struct forro_ctx forro_ctx;

#define U32C(v) (v##U)

#define U32V(v) ((uint32_t)(v)&U32C(0xFFFFFFFF))

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

static void
forro_keysetup(forro_ctx *ctx, const uint8_t *k)
{
    ctx->input[0] = LOAD32_LE(k + 0);
    ctx->input[1] = LOAD32_LE(k + 4);
    ctx->input[2] = LOAD32_LE(k + 8);
    ctx->input[3] = LOAD32_LE(k + 12);
    ctx->input[6] = U32C(0x746C6F76);
    ctx->input[7] = U32C(0x61616461);
    ctx->input[8] = LOAD32_LE(k + 16);
    ctx->input[9] = LOAD32_LE(k + 20);
    ctx->input[10] = LOAD32_LE(k + 24);
    ctx->input[11] = LOAD32_LE(k + 28);
    ctx->input[14] = U32C(0x72626173);
    ctx->input[15] = U32C(0x61636E61);
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
forro14_encrypt_bytes(forro_ctx *ctx, const uint8_t *m, uint8_t *c,
                      unsigned long long bytes)
{
    uint32_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14,
        x15;
    uint32_t j0, j1, j2, j3, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14,
        j15;
    uint8_t *ctarget = NULL;
    uint8_t tmp[64];
    unsigned int i;

    if (!bytes)
    {
        return; /* LCOV_EXCL_LINE */
    }
    j0 = ctx->input[0];
    j1 = ctx->input[1];
    j2 = ctx->input[2];
    j3 = ctx->input[3];
    j4 = ctx->input[4];
    j5 = ctx->input[5];
    j6 = ctx->input[6];
    j7 = ctx->input[7];
    j8 = ctx->input[8];
    j9 = ctx->input[9];
    j10 = ctx->input[10];
    j11 = ctx->input[11];
    j12 = ctx->input[12];
    j13 = ctx->input[13];
    j14 = ctx->input[14];
    j15 = ctx->input[15];

    for (;;)
    {
        if (bytes < 64)
        {
            memset(tmp, 0, 64);
            for (i = 0; i < bytes; ++i)
            {
                tmp[i] = m[i];
            }
            m = tmp;
            ctarget = c;
            c = tmp;
        }
        x0 = j0;
        x1 = j1;
        x2 = j2;
        x3 = j3;
        x4 = j4;
        x5 = j5;
        x6 = j6;
        x7 = j7;
        x8 = j8;
        x9 = j9;
        x10 = j10;
        x11 = j11;
        x12 = j12;
        x13 = j13;
        x14 = j14;
        x15 = j15;
        for (i = 14; i > 0; i -= 2)
        {
            QUARTERROUND(x0, x4, x8, x12, x3)
            QUARTERROUND(x1, x5, x9, x13, x0)
            QUARTERROUND(x2, x6, x10, x14, x1)
            QUARTERROUND(x3, x7, x11, x15, x2)
            QUARTERROUND(x0, x5, x10, x15, x3)
            QUARTERROUND(x1, x6, x11, x12, x0)
            QUARTERROUND(x2, x7, x8, x13, x1)
            QUARTERROUND(x3, x4, x9, x14, x2)
        }
        x0 = PLUS(x0, j0);
        x1 = PLUS(x1, j1);
        x2 = PLUS(x2, j2);
        x3 = PLUS(x3, j3);
        x4 = PLUS(x4, j4);
        x5 = PLUS(x5, j5);
        x6 = PLUS(x6, j6);
        x7 = PLUS(x7, j7);
        x8 = PLUS(x8, j8);
        x9 = PLUS(x9, j9);
        x10 = PLUS(x10, j10);
        x11 = PLUS(x11, j11);
        x12 = PLUS(x12, j12);
        x13 = PLUS(x13, j13);
        x14 = PLUS(x14, j14);
        x15 = PLUS(x15, j15);

        x0 = XOR(x0, LOAD32_LE(m + 0));
        x1 = XOR(x1, LOAD32_LE(m + 4));
        x2 = XOR(x2, LOAD32_LE(m + 8));
        x3 = XOR(x3, LOAD32_LE(m + 12));
        x4 = XOR(x4, LOAD32_LE(m + 16));
        x5 = XOR(x5, LOAD32_LE(m + 20));
        x6 = XOR(x6, LOAD32_LE(m + 24));
        x7 = XOR(x7, LOAD32_LE(m + 28));
        x8 = XOR(x8, LOAD32_LE(m + 32));
        x9 = XOR(x9, LOAD32_LE(m + 36));
        x10 = XOR(x10, LOAD32_LE(m + 40));
        x11 = XOR(x11, LOAD32_LE(m + 44));
        x12 = XOR(x12, LOAD32_LE(m + 48));
        x13 = XOR(x13, LOAD32_LE(m + 52));
        x14 = XOR(x14, LOAD32_LE(m + 56));
        x15 = XOR(x15, LOAD32_LE(m + 60));

        j4 = PLUSONE(j4);
        /* LCOV_EXCL_START */
        if (!j4)
        {
            j5 = PLUSONE(j5);
        }
        /* LCOV_EXCL_STOP */

        STORE32_LE(c + 0, x0);
        STORE32_LE(c + 4, x1);
        STORE32_LE(c + 8, x2);
        STORE32_LE(c + 12, x3);
        STORE32_LE(c + 16, x4);
        STORE32_LE(c + 20, x5);
        STORE32_LE(c + 24, x6);
        STORE32_LE(c + 28, x7);
        STORE32_LE(c + 32, x8);
        STORE32_LE(c + 36, x9);
        STORE32_LE(c + 40, x10);
        STORE32_LE(c + 44, x11);
        STORE32_LE(c + 48, x12);
        STORE32_LE(c + 52, x13);
        STORE32_LE(c + 56, x14);
        STORE32_LE(c + 60, x15);

        if (bytes <= 64)
        {
            if (bytes < 64)
            {
                for (i = 0; i < (unsigned int)bytes; ++i)
                {
                    ctarget[i] = c[i]; /* ctarget cannot be NULL */
                }
            }
            ctx->input[4] = j4;
            ctx->input[5] = j5;

            return;
        }
        bytes -= 64;
        c += 64;
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
    ic_high = U32V(ic >> 32);
    ic_low = U32V(ic);
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
    crypto_stream_forro14_ref_implementation = {
        SODIUM_C99(.stream =) stream_ref,
        SODIUM_C99(.stream_ietf_ext =) stream_ietf_ext_ref,
        SODIUM_C99(.stream_xor_ic =) stream_ref_xor_ic,
        SODIUM_C99(.stream_ietf_ext_xor_ic =) stream_ietf_ext_ref_xor_ic};
