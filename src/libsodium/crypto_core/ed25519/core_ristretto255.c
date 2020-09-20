
#include <stdint.h>

#include "crypto_core_ed25519.h"
#include "crypto_core_ristretto255.h"
#include "crypto_hash_sha512.h"
#include "private/common.h"
#include "private/ed25519_ref10.h"
#include "randombytes.h"
#include "utils.h"

int
crypto_core_ristretto255_is_valid_point(const unsigned char *p)
{
    ge25519_p3 p_p3;

    if (ristretto255_frombytes(&p_p3, p) != 0) {
        return 0;
    }
    return 1;
}

int
crypto_core_ristretto255_add(unsigned char *r,
                             const unsigned char *p, const unsigned char *q)
{
    ge25519_p3     p_p3, q_p3, r_p3;
    ge25519_p1p1   r_p1p1;
    ge25519_cached q_cached;

    if (ristretto255_frombytes(&p_p3, p) != 0 ||
        ristretto255_frombytes(&q_p3, q) != 0) {
        return -1;
    }
    ge25519_p3_to_cached(&q_cached, &q_p3);
    ge25519_add_cached(&r_p1p1, &p_p3, &q_cached);
    ge25519_p1p1_to_p3(&r_p3, &r_p1p1);
    ristretto255_p3_tobytes(r, &r_p3);

    return 0;
}

int
crypto_core_ristretto255_sub(unsigned char *r,
                             const unsigned char *p, const unsigned char *q)
{
    ge25519_p3     p_p3, q_p3, r_p3;
    ge25519_p1p1   r_p1p1;
    ge25519_cached q_cached;

    if (ristretto255_frombytes(&p_p3, p) != 0 ||
        ristretto255_frombytes(&q_p3, q) != 0) {
        return -1;
    }
    ge25519_p3_to_cached(&q_cached, &q_p3);
    ge25519_sub_cached(&r_p1p1, &p_p3, &q_cached);
    ge25519_p1p1_to_p3(&r_p3, &r_p1p1);
    ristretto255_p3_tobytes(r, &r_p3);

    return 0;
}

int
crypto_core_ristretto255_from_hash(unsigned char *p, const unsigned char *r)
{
    ristretto255_from_hash(p, r);

    return 0;
}

#define HASH_BYTES      crypto_hash_sha512_BYTES
#define HASH_BLOCKBYTES 128U
#define HASH_L          crypto_core_ristretto255_HASHBYTES

static int
_string_to_element(unsigned char *p,
                   const char *ctx, const unsigned char *msg, size_t msg_len)
{
    crypto_hash_sha512_state st;
    const unsigned char      empty_block[128] = { 0 };
    unsigned char            u0[HASH_BYTES];
    unsigned char            t[3] = { 0U, HASH_L, 0U};
    unsigned char            ctx_len_u8;
    size_t                   ctx_len = ctx != NULL ? strlen(ctx) : 0U;

    COMPILER_ASSERT(HASH_L <= 0xff);
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

    t[2]++;
    crypto_hash_sha512_init(&st);
    crypto_hash_sha512_update(&st, u0, HASH_BYTES);
    crypto_hash_sha512_update(&st, &t[2], 1U);
    crypto_hash_sha512_update(&st, (const unsigned char *) ctx, ctx_len);
    crypto_hash_sha512_update(&st, &ctx_len_u8, 1U);
    crypto_hash_sha512_final(&st, u0);

    COMPILER_ASSERT(crypto_core_ristretto255_HASHBYTES == HASH_L);
    ristretto255_from_hash(p, u0);

    return 0;
}

int
crypto_core_risretto255_from_string(unsigned char p[crypto_core_ristretto255_BYTES],
                                    const char *ctx, const unsigned char *msg,
                                    size_t msg_len)
{
    return _string_to_element(p, ctx, msg, msg_len);
}

int
crypto_core_ristretto255_from_string_ro(unsigned char p[crypto_core_ristretto255_BYTES],
                                        const char *ctx, const unsigned char *msg,
                                        size_t msg_len)
{
    return crypto_core_risretto255_from_string(p, ctx, msg, msg_len);
}

void
crypto_core_ristretto255_random(unsigned char *p)
{
    unsigned char h[crypto_core_ristretto255_HASHBYTES];

    randombytes_buf(h, sizeof h);
    (void) crypto_core_ristretto255_from_hash(p, h);
}

void
crypto_core_ristretto255_scalar_random(unsigned char *r)
{
    crypto_core_ed25519_scalar_random(r);
}

int
crypto_core_ristretto255_scalar_invert(unsigned char *recip,
                                       const unsigned char *s)
{
    return crypto_core_ed25519_scalar_invert(recip, s);
}

void
crypto_core_ristretto255_scalar_negate(unsigned char *neg,
                                       const unsigned char *s)
{
    crypto_core_ed25519_scalar_negate(neg, s);
}

void
crypto_core_ristretto255_scalar_complement(unsigned char *comp,
                                           const unsigned char *s)
{
    crypto_core_ed25519_scalar_complement(comp, s);
}

void
crypto_core_ristretto255_scalar_add(unsigned char *z, const unsigned char *x,
                                    const unsigned char *y)
{
    crypto_core_ed25519_scalar_add(z, x, y);
}

void
crypto_core_ristretto255_scalar_sub(unsigned char *z, const unsigned char *x,
                                    const unsigned char *y)
{
    crypto_core_ed25519_scalar_sub(z, x, y);
}

void
crypto_core_ristretto255_scalar_mul(unsigned char *z, const unsigned char *x,
                                    const unsigned char *y)
{
    sc25519_mul(z, x, y);
}

void
crypto_core_ristretto255_scalar_reduce(unsigned char *r,
                                       const unsigned char *s)
{
    crypto_core_ed25519_scalar_reduce(r, s);
}

int
crypto_core_ristretto255_scalar_is_canonical(const unsigned char *s)
{
    return sc25519_is_canonical(s);
}

size_t
crypto_core_ristretto255_bytes(void)
{
    return crypto_core_ristretto255_BYTES;
}

size_t
crypto_core_ristretto255_nonreducedscalarbytes(void)
{
    return crypto_core_ristretto255_NONREDUCEDSCALARBYTES;
}

size_t
crypto_core_ristretto255_hashbytes(void)
{
    return crypto_core_ristretto255_HASHBYTES;
}

size_t
crypto_core_ristretto255_scalarbytes(void)
{
    return crypto_core_ristretto255_SCALARBYTES;
}
