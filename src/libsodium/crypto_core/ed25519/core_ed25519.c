
#include "crypto_core_ed25519.h"
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
    ge25519_add(&r_p1p1, &p_p3, &q_cached);
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
    ge25519_sub(&r_p1p1, &p_p3, &q_cached);
    ge25519_p1p1_to_p3(&r_p3, &r_p1p1);
    ge25519_p3_tobytes(r, &r_p3);

    return 0;
}

int
crypto_core_ed25519_from_uniform(unsigned char *p, const unsigned char *r)
{
    ge25519_from_uniform(p, r);

    return - ge25519_has_small_order(p);
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

void
crypto_core_ed25519_scalar_reduce(unsigned char *r,
                                  const unsigned char s[crypto_core_ed25519_NONREDUCEDSCALARBYTES])
{
    unsigned char t[crypto_core_ed25519_NONREDUCEDSCALARBYTES];

    memcpy(t, s, sizeof t);
    sc25519_reduce(t);
    memcpy(r, t, crypto_core_ed25519_SCALARBYTES);
    sodium_memzero(t, sizeof t);
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
crypto_core_ed25519_scalarbytes(void)
{
    return crypto_core_ed25519_SCALARBYTES;
}
