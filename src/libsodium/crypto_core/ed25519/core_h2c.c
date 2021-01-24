#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "core_h2c.h"
#include "crypto_hash_sha256.h"
#include "crypto_hash_sha512.h"
#include "private/common.h"

#define HASH_BYTES      crypto_hash_sha256_BYTES
#define HASH_BLOCKBYTES 64U

static int
core_h2c_string_to_hash_sha256(unsigned char *h, const size_t h_len, const char *ctx,
                               const unsigned char *msg, size_t msg_len)
{
    crypto_hash_sha256_state st;
    const unsigned char      empty_block[HASH_BLOCKBYTES] = { 0 };
    unsigned char            u0[HASH_BYTES];
    unsigned char            ux[HASH_BYTES] = { 0 };
    unsigned char            t[3] = { 0U, (unsigned char) h_len, 0U};
    unsigned char            ctx_len_u8;
    size_t                   ctx_len = ctx != NULL ? strlen(ctx) : 0U;
    size_t                   i, j;

    assert(h_len <= 0xff);
    if (ctx_len > (size_t) 0xff) {
        crypto_hash_sha256_init(&st);
        crypto_hash_sha256_update(&st,
                                  (const unsigned char *) "H2C-OVERSIZE-DST-",
                                  sizeof "H2C-OVERSIZE-DST-" - 1U);
        crypto_hash_sha256_update(&st, (const unsigned char *) ctx, ctx_len);
        crypto_hash_sha256_final(&st, u0);
        ctx = (const char *) u0;
        ctx_len = HASH_BYTES;
        COMPILER_ASSERT(HASH_BYTES <= (size_t) 0xff);
    }
    ctx_len_u8 = (unsigned char) ctx_len;
    crypto_hash_sha256_init(&st);
    crypto_hash_sha256_update(&st, empty_block, sizeof empty_block);
    crypto_hash_sha256_update(&st, msg, msg_len);
    crypto_hash_sha256_update(&st, t, 3U);
    crypto_hash_sha256_update(&st, (const unsigned char *) ctx, ctx_len);
    crypto_hash_sha256_update(&st, &ctx_len_u8, 1U);
    crypto_hash_sha256_final(&st, u0);

    for (i = 0U; i < h_len; i += HASH_BYTES) {
        for (j = 0U; j < HASH_BYTES; j++) {
            ux[j] ^= u0[j];
        }
        t[2]++;
        crypto_hash_sha256_init(&st);
        crypto_hash_sha256_update(&st, ux, HASH_BYTES);
        crypto_hash_sha256_update(&st, &t[2], 1U);
        crypto_hash_sha256_update(&st, (const unsigned char *) ctx, ctx_len);
        crypto_hash_sha256_update(&st, &ctx_len_u8, 1U);
        crypto_hash_sha256_final(&st, ux);
        memcpy(&h[i], ux, h_len - i >= (sizeof ux) ? (sizeof ux) : h_len - i);
    }
    return 0;
}

#undef  HASH_BYTES
#undef  HASH_BLOCKBYTES

#define HASH_BYTES      crypto_hash_sha512_BYTES
#define HASH_BLOCKBYTES 128U

static int
core_h2c_string_to_hash_sha512(unsigned char *h, const size_t h_len, const char *ctx,
                               const unsigned char *msg, size_t msg_len)
{
    crypto_hash_sha512_state st;
    const unsigned char      empty_block[HASH_BLOCKBYTES] = { 0 };
    unsigned char            u0[HASH_BYTES];
    unsigned char            ux[HASH_BYTES] = { 0 };
    unsigned char            t[3] = { 0U, (unsigned char) h_len, 0U};
    unsigned char            ctx_len_u8;
    size_t                   ctx_len = ctx != NULL ? strlen(ctx) : 0U;
    size_t                   i, j;

    assert(h_len <= 0xff);
    if (ctx_len > (size_t) 0xff) {
        crypto_hash_sha512_init(&st);
        crypto_hash_sha512_update(&st,
                                  (const unsigned char *) "H2C-OVERSIZE-DST-",
                                  sizeof "H2C-OVERSIZE-DST-" - 1U);
        crypto_hash_sha512_update(&st, (const unsigned char *) ctx, ctx_len);
        crypto_hash_sha512_final(&st, u0);
        ctx = (const char *) u0;
        ctx_len = HASH_BYTES;
        COMPILER_ASSERT(HASH_BYTES <= (size_t) 0xff);
    }
    ctx_len_u8 = (unsigned char) ctx_len;
    crypto_hash_sha512_init(&st);
    crypto_hash_sha512_update(&st, empty_block, sizeof empty_block);
    crypto_hash_sha512_update(&st, msg, msg_len);
    crypto_hash_sha512_update(&st, t, 3U);
    crypto_hash_sha512_update(&st, (const unsigned char *) ctx, ctx_len);
    crypto_hash_sha512_update(&st, &ctx_len_u8, 1U);
    crypto_hash_sha512_final(&st, u0);

    for (i = 0U; i < h_len; i += HASH_BYTES) {
        for (j = 0U; j < HASH_BYTES; j++) {
            ux[j] ^= u0[j];
        }
        t[2]++;
        crypto_hash_sha512_init(&st);
        crypto_hash_sha512_update(&st, ux, HASH_BYTES);
        crypto_hash_sha512_update(&st, &t[2], 1U);
        crypto_hash_sha512_update(&st, (const unsigned char *) ctx, ctx_len);
        crypto_hash_sha512_update(&st, &ctx_len_u8, 1U);
        crypto_hash_sha512_final(&st, ux);
        memcpy(&h[i], ux, h_len - i >= (sizeof ux) ? (sizeof ux) : h_len - i);
    }
    return 0;
}

int
core_h2c_string_to_hash(unsigned char *h, const size_t h_len, const char *ctx,
                        const unsigned char *msg, size_t msg_len, int hash_alg)
{
    switch (hash_alg) {
    case CORE_H2C_SHA256:
        return core_h2c_string_to_hash_sha256(h, h_len, ctx, msg, msg_len);
    case CORE_H2C_SHA512:
        return core_h2c_string_to_hash_sha512(h, h_len, ctx, msg, msg_len);
    default:
        errno = EINVAL;
        return -1;
    }
}
