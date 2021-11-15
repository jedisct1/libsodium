#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core_h2c.h"
#include "crypto_core_ed25519.h"
#include "crypto_hash_sha512.h"
#include "private/common.h"
#include "private/ed25519_ref10.h"
#include "randombytes.h"
#include "utils.h"

int
crypto_core_ed25519_is_valid_point(const unsigned char *p)
{
    ge25519_p3 p_p3;

    if (ge25519_is_canonical(p) == 0 ||
        ge25519_has_small_order(p) != 0 ||
        ge25519_frombytes(&p_p3, p) != 0 ||
        ge25519_is_on_curve(&p_p3) == 0 ||
        ge25519_is_on_main_subgroup(&p_p3) == 0) {
        return 0;
    }
    return 1;
}

int
crypto_core_ed25519_add(unsigned char *r,
                        const unsigned char *p, const unsigned char *q)
{
    ge25519_p3     p_p3, q_p3, r_p3;
    ge25519_p1p1   r_p1p1;
    ge25519_cached q_cached;

    if (ge25519_frombytes(&p_p3, p) != 0 || ge25519_is_on_curve(&p_p3) == 0 ||
        ge25519_frombytes(&q_p3, q) != 0 || ge25519_is_on_curve(&q_p3) == 0) {
        return -1;
    }
    ge25519_p3_to_cached(&q_cached, &q_p3);
    ge25519_add_cached(&r_p1p1, &p_p3, &q_cached);
    ge25519_p1p1_to_p3(&r_p3, &r_p1p1);
    ge25519_p3_tobytes(r, &r_p3);

    return 0;
}

int
crypto_core_ed25519_sub(unsigned char *r,
                        const unsigned char *p, const unsigned char *q)
{
    ge25519_p3     p_p3, q_p3, r_p3;
    ge25519_p1p1   r_p1p1;
    ge25519_cached q_cached;

    if (ge25519_frombytes(&p_p3, p) != 0 || ge25519_is_on_curve(&p_p3) == 0 ||
        ge25519_frombytes(&q_p3, q) != 0 || ge25519_is_on_curve(&q_p3) == 0) {
        return -1;
    }
    ge25519_p3_to_cached(&q_cached, &q_p3);
    ge25519_sub_cached(&r_p1p1, &p_p3, &q_cached);
    ge25519_p1p1_to_p3(&r_p3, &r_p1p1);
    ge25519_p3_tobytes(r, &r_p3);

    return 0;
}

int
crypto_core_ed25519_from_uniform(unsigned char *p, const unsigned char *r)
{
    ge25519_from_uniform(p, r);

    return 0;
}

#define HASH_GE_L 48U

static int
_string_to_points(unsigned char * const px, const size_t n,
                  const char *ctx, const unsigned char *msg, size_t msg_len,
                  int hash_alg)
{
    unsigned char h[crypto_core_ed25519_HASHBYTES];
    unsigned char h_be[2U * HASH_GE_L];
    size_t        i, j;

    if (n > 2U) {
        abort(); /* LCOV_EXCL_LINE */
    }
    if (core_h2c_string_to_hash(h_be, n * HASH_GE_L, ctx, msg, msg_len,
                                hash_alg) != 0) {
        return -1;
    }
    COMPILER_ASSERT(sizeof h >= HASH_GE_L);
    for (i = 0U; i < n; i++) {
        for (j = 0U; j < HASH_GE_L; j++) {
            h[j] = h_be[i * HASH_GE_L + HASH_GE_L - 1U - j];
        }
        memset(&h[j], 0, (sizeof h) - j);
        ge25519_from_hash(&px[i * crypto_core_ed25519_BYTES], h);
    }
    return 0;
}

int
crypto_core_ed25519_from_string(unsigned char p[crypto_core_ed25519_BYTES],
                                const char *ctx, const unsigned char *msg,
                                size_t msg_len, int hash_alg)
{
    return _string_to_points(p, 1, ctx, msg, msg_len, hash_alg);
}

int
crypto_core_ed25519_from_string_ro(unsigned char p[crypto_core_ed25519_BYTES],
                                   const char *ctx, const unsigned char *msg,
                                   size_t msg_len, int hash_alg)
{
    unsigned char px[2 * crypto_core_ed25519_BYTES];

    if (_string_to_points(px, 2, ctx, msg, msg_len, hash_alg) != 0) {
        return -1;
    }
    return crypto_core_ed25519_add(p, &px[0], &px[crypto_core_ed25519_BYTES]);
}

void
crypto_core_ed25519_random(unsigned char *p)
{
    unsigned char h[crypto_core_ed25519_UNIFORMBYTES];

    randombytes_buf(h, sizeof h);
    (void) crypto_core_ed25519_from_uniform(p, h);
}

void
crypto_core_ed25519_scalar_random(unsigned char *r)
{
    do {
        randombytes_buf(r, crypto_core_ed25519_SCALARBYTES);
        r[crypto_core_ed25519_SCALARBYTES - 1] &= 0x1f;
    } while (sc25519_is_canonical(r) == 0 ||
             sodium_is_zero(r, crypto_core_ed25519_SCALARBYTES));
}

int
crypto_core_ed25519_scalar_invert(unsigned char *recip, const unsigned char *s)
{
    sc25519_invert(recip, s);

    return - sodium_is_zero(s, crypto_core_ed25519_SCALARBYTES);
}

/* 2^252+27742317777372353535851937790883648493 */
static const unsigned char L[] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
    0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

void
crypto_core_ed25519_scalar_negate(unsigned char *neg, const unsigned char *s)
{
    unsigned char t_[crypto_core_ed25519_NONREDUCEDSCALARBYTES];
    unsigned char s_[crypto_core_ed25519_NONREDUCEDSCALARBYTES];

    COMPILER_ASSERT(crypto_core_ed25519_NONREDUCEDSCALARBYTES >=
                    2 * crypto_core_ed25519_SCALARBYTES);
    memset(t_, 0, sizeof t_);
    memset(s_, 0, sizeof s_);
    memcpy(t_ + crypto_core_ed25519_SCALARBYTES, L,
           crypto_core_ed25519_SCALARBYTES);
    memcpy(s_, s, crypto_core_ed25519_SCALARBYTES);
    sodium_sub(t_, s_, sizeof t_);
    sc25519_reduce(t_);
    memcpy(neg, t_, crypto_core_ed25519_SCALARBYTES);
}

void
crypto_core_ed25519_scalar_complement(unsigned char *comp,
                                      const unsigned char *s)
{
    unsigned char t_[crypto_core_ed25519_NONREDUCEDSCALARBYTES];
    unsigned char s_[crypto_core_ed25519_NONREDUCEDSCALARBYTES];

    COMPILER_ASSERT(crypto_core_ed25519_NONREDUCEDSCALARBYTES >=
                    2 * crypto_core_ed25519_SCALARBYTES);
    memset(t_, 0, sizeof t_);
    memset(s_, 0, sizeof s_);
    t_[0]++;
    memcpy(t_ + crypto_core_ed25519_SCALARBYTES, L,
           crypto_core_ed25519_SCALARBYTES);
    memcpy(s_, s, crypto_core_ed25519_SCALARBYTES);
    sodium_sub(t_, s_, sizeof t_);
    sc25519_reduce(t_);
    memcpy(comp, t_, crypto_core_ed25519_SCALARBYTES);
}

void
crypto_core_ed25519_scalar_add(unsigned char *z, const unsigned char *x,
                               const unsigned char *y)
{
    unsigned char x_[crypto_core_ed25519_NONREDUCEDSCALARBYTES];
    unsigned char y_[crypto_core_ed25519_NONREDUCEDSCALARBYTES];

    memset(x_, 0, sizeof x_);
    memset(y_, 0, sizeof y_);
    memcpy(x_, x, crypto_core_ed25519_SCALARBYTES);
    memcpy(y_, y, crypto_core_ed25519_SCALARBYTES);
    sodium_add(x_, y_, crypto_core_ed25519_NONREDUCEDSCALARBYTES);
    crypto_core_ed25519_scalar_reduce(z, x_);
}

void
crypto_core_ed25519_scalar_sub(unsigned char *z, const unsigned char *x,
                               const unsigned char *y)
{
    unsigned char yn[crypto_core_ed25519_SCALARBYTES];

    crypto_core_ed25519_scalar_negate(yn, y);
    crypto_core_ed25519_scalar_add(z, x, yn);
}

void
crypto_core_ed25519_scalar_mul(unsigned char *z, const unsigned char *x,
                               const unsigned char *y)
{
    sc25519_mul(z, x, y);
}

void
crypto_core_ed25519_scalar_reduce(unsigned char *r,
                                  const unsigned char *s)
{
    unsigned char t[crypto_core_ed25519_NONREDUCEDSCALARBYTES];

    memcpy(t, s, sizeof t);
    sc25519_reduce(t);
    memcpy(r, t, crypto_core_ed25519_SCALARBYTES);
    sodium_memzero(t, sizeof t);
}

int
crypto_core_ed25519_scalar_is_canonical(const unsigned char *s)
{
    return sc25519_is_canonical(s);
}

size_t
crypto_core_ed25519_bytes(void)
{
    return crypto_core_ed25519_BYTES;
}

size_t
crypto_core_ed25519_nonreducedscalarbytes(void)
{
    return crypto_core_ed25519_NONREDUCEDSCALARBYTES;
}

size_t
crypto_core_ed25519_uniformbytes(void)
{
    return crypto_core_ed25519_UNIFORMBYTES;
}

size_t
crypto_core_ed25519_hashbytes(void)
{
    return crypto_core_ed25519_HASHBYTES;
}

size_t
crypto_core_ed25519_scalarbytes(void)
{
    return crypto_core_ed25519_SCALARBYTES;
}
