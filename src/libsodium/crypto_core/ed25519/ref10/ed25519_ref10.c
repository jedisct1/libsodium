#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "crypto_verify_32.h"
#include "private/common.h"
#include "private/ed25519_ref10.h"
#include "utils.h"

static inline uint64_t
load_3(const unsigned char *in)
{
    uint64_t result;

    result = (uint64_t) in[0];
    result |= ((uint64_t) in[1]) << 8;
    result |= ((uint64_t) in[2]) << 16;

    return result;
}

static inline uint64_t
load_4(const unsigned char *in)
{
    uint64_t result;

    result = (uint64_t) in[0];
    result |= ((uint64_t) in[1]) << 8;
    result |= ((uint64_t) in[2]) << 16;
    result |= ((uint64_t) in[3]) << 24;

    return result;
}

/*
 * Field arithmetic:
 * Use 5*51 bit limbs on 64-bit systems with support for 128 bit arithmetic,
 * and 10*25.5 bit limbs elsewhere.
 *
 * Functions used elsewhere that are candidates for inlining are defined
 * via "private/curve25519_ref10.h".
 */

#ifdef HAVE_TI_MODE
# include "fe_51/constants.h"
# include "fe_51/fe.h"
#else
# include "fe_25_5/constants.h"
# include "fe_25_5/fe.h"
#endif

static inline void
fe25519_sqmul(fe25519 s, const int n, const fe25519 a)
{
    int i;

    for (i = 0; i < n; i++) {
        fe25519_sq(s, s);
    }
    fe25519_mul(s, s, a);
}

/*
 * Inversion - returns 0 if z=0
 */
void
fe25519_invert(fe25519 out, const fe25519 z)
{
    fe25519 t0, t1, t2, t3;
    int     i;

    fe25519_sq(t0, z);
    fe25519_sq(t1, t0);
    fe25519_sq(t1, t1);
    fe25519_mul(t1, z, t1);
    fe25519_mul(t0, t0, t1);
    fe25519_sq(t2, t0);
    fe25519_mul(t1, t1, t2);
    fe25519_sq(t2, t1);
    for (i = 1; i < 5; ++i) {
        fe25519_sq(t2, t2);
    }
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t2, t1);
    for (i = 1; i < 10; ++i) {
        fe25519_sq(t2, t2);
    }
    fe25519_mul(t2, t2, t1);
    fe25519_sq(t3, t2);
    for (i = 1; i < 20; ++i) {
        fe25519_sq(t3, t3);
    }
    fe25519_mul(t2, t3, t2);
    fe25519_sq(t2, t2);
    for (i = 1; i < 10; ++i) {
        fe25519_sq(t2, t2);
    }
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t2, t1);
    for (i = 1; i < 50; ++i) {
        fe25519_sq(t2, t2);
    }
    fe25519_mul(t2, t2, t1);
    fe25519_sq(t3, t2);
    for (i = 1; i < 100; ++i) {
        fe25519_sq(t3, t3);
    }
    fe25519_mul(t2, t3, t2);
    fe25519_sq(t2, t2);
    for (i = 1; i < 50; ++i) {
        fe25519_sq(t2, t2);
    }
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t1, t1);
    for (i = 1; i < 5; ++i) {
        fe25519_sq(t1, t1);
    }
    fe25519_mul(out, t1, t0);
}

static void
fe25519_pow22523(fe25519 out, const fe25519 z)
{
    fe25519 t0, t1, t2;
    int     i;

    fe25519_sq(t0, z);
    fe25519_sq(t1, t0);
    fe25519_sq(t1, t1);
    fe25519_mul(t1, z, t1);
    fe25519_mul(t0, t0, t1);
    fe25519_sq(t0, t0);
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t1, t0);
    for (i = 1; i < 5; ++i) {
        fe25519_sq(t1, t1);
    }
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t1, t0);
    for (i = 1; i < 10; ++i) {
        fe25519_sq(t1, t1);
    }
    fe25519_mul(t1, t1, t0);
    fe25519_sq(t2, t1);
    for (i = 1; i < 20; ++i) {
        fe25519_sq(t2, t2);
    }
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t1, t1);
    for (i = 1; i < 10; ++i) {
        fe25519_sq(t1, t1);
    }
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t1, t0);
    for (i = 1; i < 50; ++i) {
        fe25519_sq(t1, t1);
    }
    fe25519_mul(t1, t1, t0);
    fe25519_sq(t2, t1);
    for (i = 1; i < 100; ++i) {
        fe25519_sq(t2, t2);
    }
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t1, t1);
    for (i = 1; i < 50; ++i) {
        fe25519_sq(t1, t1);
    }
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t0, t0);
    fe25519_sq(t0, t0);
    fe25519_mul(out, t0, z);
}

static inline void
fe25519_cneg(fe25519 h, const fe25519 f, unsigned int b)
{
    fe25519 negf;

    fe25519_neg(negf, f);
    fe25519_copy(h, f);
    fe25519_cmov(h, negf, b);
}

static inline void
fe25519_abs(fe25519 h, const fe25519 f)
{
    fe25519_cneg(h, f, fe25519_isnegative(f));
}

static void
fe25519_unchecked_sqrt(fe25519 x, const fe25519 x2)
{
    fe25519 p_root;
    fe25519 m_root;
    fe25519 m_root2;
    fe25519 e;

    fe25519_pow22523(e, x);
    fe25519_mul(p_root, e, x);
    fe25519_mul(m_root, p_root, fe25519_sqrtm1);
    fe25519_sq(m_root2, m_root);
    fe25519_sub(e, x2, m_root2);
    fe25519_copy(x, p_root);
    fe25519_cmov(x, m_root, fe25519_iszero(e));
}

static int
fe25519_sqrt(fe25519 x, const fe25519 x2)
{
    fe25519 check;
    fe25519 x2_copy;

    fe25519_copy(x2_copy, x2);
    fe25519_unchecked_sqrt(x, x2);
    fe25519_sq(check, x);
    fe25519_sub(check, check, x2_copy);

    return fe25519_iszero(check) - 1;
}

static int
fe25519_notsquare(const fe25519 x)
{
    fe25519       _10, _11, _1100, _1111, _11110000, _11111111;
    fe25519       t, u, v;
    unsigned char s[32];

    /* Jacobi symbol - x^((p-1)/2) */
    fe25519_mul(_10, x, x);
    fe25519_mul(_11, x, _10);
    fe25519_sq(_1100, _11);
    fe25519_sq(_1100, _1100);
    fe25519_mul(_1111, _11, _1100);
    fe25519_sq(_11110000, _1111);
    fe25519_sq(_11110000, _11110000);
    fe25519_sq(_11110000, _11110000);
    fe25519_sq(_11110000, _11110000);
    fe25519_mul(_11111111, _1111, _11110000);
    fe25519_copy(t, _11111111);
    fe25519_sqmul(t, 2, _11);
    fe25519_copy(u, t);
    fe25519_sqmul(t, 10, u);
    fe25519_sqmul(t, 10, u);
    fe25519_copy(v, t);
    fe25519_sqmul(t, 30, v);
    fe25519_copy(v, t);
    fe25519_sqmul(t, 60, v);
    fe25519_copy(v, t);
    fe25519_sqmul(t, 120, v);
    fe25519_sqmul(t, 10, u);
    fe25519_sqmul(t, 3, _11);
    fe25519_sq(t, t);

    fe25519_tobytes(s, t);

    return s[1] & 1;
}

/*
 r = p + q
 */

void
ge25519_add_cached(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_cached *q)
{
    fe25519 t0;

    fe25519_add(r->X, p->Y, p->X);
    fe25519_sub(r->Y, p->Y, p->X);
    fe25519_mul(r->Z, r->X, q->YplusX);
    fe25519_mul(r->Y, r->Y, q->YminusX);
    fe25519_mul(r->T, q->T2d, p->T);
    fe25519_mul(r->X, p->Z, q->Z);
    fe25519_add(t0, r->X, r->X);
    fe25519_sub(r->X, r->Z, r->Y);
    fe25519_add(r->Y, r->Z, r->Y);
    fe25519_add(r->Z, t0, r->T);
    fe25519_sub(r->T, t0, r->T);
}

static void
slide_vartime(signed char *r, const unsigned char *a)
{
    int i;
    int b;
    int k;
    int ribs;
    int cmp;

    for (i = 0; i < 256; ++i) {
        r[i] = 1 & (a[i >> 3] >> (i & 7));
    }
    for (i = 0; i < 256; ++i) {
        if (! r[i]) {
            continue;
        }
        for (b = 1; b <= 6 && i + b < 256; ++b) {
            if (! r[i + b]) {
                continue;
            }
            ribs = r[i + b] << b;
            cmp = r[i] + ribs;
            if (cmp <= 15) {
                r[i] = cmp;
                r[i + b] = 0;
            } else {
                cmp = r[i] - ribs;
                if (cmp < -15) {
                    break;
                }
                r[i] = cmp;
                for (k = i + b; k < 256; ++k) {
                    if (! r[k]) {
                        r[k] = 1;
                        break;
                    }
                    r[k] = 0;
                }
            }
        }
    }
}

int
ge25519_frombytes(ge25519_p3 *h, const unsigned char *s)
{
    fe25519 u;
    fe25519 v;
    fe25519 v3;
    fe25519 vxx;
    fe25519 m_root_check, p_root_check;
    fe25519 negx;
    fe25519 x_sqrtm1;
    int     has_m_root, has_p_root;

    fe25519_frombytes(h->Y, s);
    fe25519_1(h->Z);
    fe25519_sq(u, h->Y);
    fe25519_mul(v, u, ed25519_d);
    fe25519_sub(u, u, h->Z); /* u = y^2-1 */
    fe25519_add(v, v, h->Z); /* v = dy^2+1 */

    fe25519_sq(v3, v);
    fe25519_mul(v3, v3, v); /* v3 = v^3 */
    fe25519_sq(h->X, v3);
    fe25519_mul(h->X, h->X, v);
    fe25519_mul(h->X, h->X, u); /* x = uv^7 */

    fe25519_pow22523(h->X, h->X); /* x = (uv^7)^((q-5)/8) */
    fe25519_mul(h->X, h->X, v3);
    fe25519_mul(h->X, h->X, u); /* x = uv^3(uv^7)^((q-5)/8) */

    fe25519_sq(vxx, h->X);
    fe25519_mul(vxx, vxx, v);
    fe25519_sub(m_root_check, vxx, u); /* vx^2-u */
    fe25519_add(p_root_check, vxx, u); /* vx^2+u */
    has_m_root = fe25519_iszero(m_root_check);
    has_p_root = fe25519_iszero(p_root_check);
    fe25519_mul(x_sqrtm1, h->X, fe25519_sqrtm1); /* x*sqrt(-1) */
    fe25519_cmov(h->X, x_sqrtm1, 1 - has_m_root);

    fe25519_neg(negx, h->X);
    fe25519_cmov(h->X, negx, fe25519_isnegative(h->X) ^ (s[31] >> 7));
    fe25519_mul(h->T, h->X, h->Y);

    return (has_m_root | has_p_root) - 1;
}

int
ge25519_frombytes_negate_vartime(ge25519_p3 *h, const unsigned char *s)
{
    fe25519 u;
    fe25519 v;
    fe25519 v3;
    fe25519 vxx;
    fe25519 m_root_check, p_root_check;

    fe25519_frombytes(h->Y, s);
    fe25519_1(h->Z);
    fe25519_sq(u, h->Y);
    fe25519_mul(v, u, ed25519_d);
    fe25519_sub(u, u, h->Z); /* u = y^2-1 */
    fe25519_add(v, v, h->Z); /* v = dy^2+1 */

    fe25519_sq(v3, v);
    fe25519_mul(v3, v3, v); /* v3 = v^3 */
    fe25519_sq(h->X, v3);
    fe25519_mul(h->X, h->X, v);
    fe25519_mul(h->X, h->X, u); /* x = uv^7 */

    fe25519_pow22523(h->X, h->X); /* x = (uv^7)^((q-5)/8) */
    fe25519_mul(h->X, h->X, v3);
    fe25519_mul(h->X, h->X, u); /* x = uv^3(uv^7)^((q-5)/8) */

    fe25519_sq(vxx, h->X);
    fe25519_mul(vxx, vxx, v);
    fe25519_sub(m_root_check, vxx, u); /* vx^2-u */
    if (fe25519_iszero(m_root_check) == 0) {
        fe25519_add(p_root_check, vxx, u); /* vx^2+u */
        if (fe25519_iszero(p_root_check) == 0) {
            return -1;
        }
        fe25519_mul(h->X, h->X, fe25519_sqrtm1);
    }

    if (fe25519_isnegative(h->X) == (s[31] >> 7)) {
        fe25519_neg(h->X, h->X);
    }
    fe25519_mul(h->T, h->X, h->Y);

    return 0;
}

/*
 r = p + q
 */

static void
ge25519_add_precomp(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_precomp *q)
{
    fe25519 t0;

    fe25519_add(r->X, p->Y, p->X);
    fe25519_sub(r->Y, p->Y, p->X);
    fe25519_mul(r->Z, r->X, q->yplusx);
    fe25519_mul(r->Y, r->Y, q->yminusx);
    fe25519_mul(r->T, q->xy2d, p->T);
    fe25519_add(t0, p->Z, p->Z);
    fe25519_sub(r->X, r->Z, r->Y);
    fe25519_add(r->Y, r->Z, r->Y);
    fe25519_add(r->Z, t0, r->T);
    fe25519_sub(r->T, t0, r->T);
}

/*
 r = p - q
 */

static void
ge25519_sub_precomp(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_precomp *q)
{
    fe25519 t0;

    fe25519_add(r->X, p->Y, p->X);
    fe25519_sub(r->Y, p->Y, p->X);
    fe25519_mul(r->Z, r->X, q->yminusx);
    fe25519_mul(r->Y, r->Y, q->yplusx);
    fe25519_mul(r->T, q->xy2d, p->T);
    fe25519_add(t0, p->Z, p->Z);
    fe25519_sub(r->X, r->Z, r->Y);
    fe25519_add(r->Y, r->Z, r->Y);
    fe25519_sub(r->Z, t0, r->T);
    fe25519_add(r->T, t0, r->T);
}

/*
 r = p
 */

void
ge25519_p1p1_to_p2(ge25519_p2 *r, const ge25519_p1p1 *p)
{
    fe25519_mul(r->X, p->X, p->T);
    fe25519_mul(r->Y, p->Y, p->Z);
    fe25519_mul(r->Z, p->Z, p->T);
}

/*
 r = p
 */

void
ge25519_p1p1_to_p3(ge25519_p3 *r, const ge25519_p1p1 *p)
{
    fe25519_mul(r->X, p->X, p->T);
    fe25519_mul(r->Y, p->Y, p->Z);
    fe25519_mul(r->Z, p->Z, p->T);
    fe25519_mul(r->T, p->X, p->Y);
}

static void
ge25519_p2_0(ge25519_p2 *h)
{
    fe25519_0(h->X);
    fe25519_1(h->Y);
    fe25519_1(h->Z);
}

/*
 r = 2 * p
 */

static void
ge25519_p2_dbl(ge25519_p1p1 *r, const ge25519_p2 *p)
{
    fe25519 t0;

    fe25519_sq(r->X, p->X);
    fe25519_sq(r->Z, p->Y);
    fe25519_sq2(r->T, p->Z);
    fe25519_add(r->Y, p->X, p->Y);
    fe25519_sq(t0, r->Y);
    fe25519_add(r->Y, r->Z, r->X);
    fe25519_sub(r->Z, r->Z, r->X);
    fe25519_sub(r->X, t0, r->Y);
    fe25519_sub(r->T, r->T, r->Z);
}

static void
ge25519_p3_0(ge25519_p3 *h)
{
    fe25519_0(h->X);
    fe25519_1(h->Y);
    fe25519_1(h->Z);
    fe25519_0(h->T);
}

static void
ge25519_cached_0(ge25519_cached *h)
{
    fe25519_1(h->YplusX);
    fe25519_1(h->YminusX);
    fe25519_1(h->Z);
    fe25519_0(h->T2d);
}

/*
 r = p
 */

void
ge25519_p3_to_cached(ge25519_cached *r, const ge25519_p3 *p)
{
    fe25519_add(r->YplusX, p->Y, p->X);
    fe25519_sub(r->YminusX, p->Y, p->X);
    fe25519_copy(r->Z, p->Z);
    fe25519_mul(r->T2d, p->T, ed25519_d2);
}

static void
ge25519_p3_to_precomp(ge25519_precomp *pi, const ge25519_p3 *p)
{
    fe25519 recip;
    fe25519 x;
    fe25519 y;
    fe25519 xy;

    fe25519_invert(recip, p->Z);
    fe25519_mul(x, p->X, recip);
    fe25519_mul(y, p->Y, recip);
    fe25519_add(pi->yplusx, y, x);
    fe25519_sub(pi->yminusx, y, x);
    fe25519_mul(xy, x, y);
    fe25519_mul(pi->xy2d, xy, ed25519_d2);
}

/*
 r = p
 */

static void
ge25519_p3_to_p2(ge25519_p2 *r, const ge25519_p3 *p)
{
    fe25519_copy(r->X, p->X);
    fe25519_copy(r->Y, p->Y);
    fe25519_copy(r->Z, p->Z);
}

void
ge25519_p3_tobytes(unsigned char *s, const ge25519_p3 *h)
{
    fe25519 recip;
    fe25519 x;
    fe25519 y;

    fe25519_invert(recip, h->Z);
    fe25519_mul(x, h->X, recip);
    fe25519_mul(y, h->Y, recip);
    fe25519_tobytes(s, y);
    s[31] ^= fe25519_isnegative(x) << 7;
}

/*
 r = 2 * p
 */

static void
ge25519_p3_dbl(ge25519_p1p1 *r, const ge25519_p3 *p)
{
    ge25519_p2 q;
    ge25519_p3_to_p2(&q, p);
    ge25519_p2_dbl(r, &q);
}

static void
ge25519_precomp_0(ge25519_precomp *h)
{
    fe25519_1(h->yplusx);
    fe25519_1(h->yminusx);
    fe25519_0(h->xy2d);
}

static unsigned char
equal(signed char b, signed char c)
{
    unsigned char ub = b;
    unsigned char uc = c;
    unsigned char x  = ub ^ uc; /* 0: yes; 1..255: no */
    uint32_t      y  = (uint32_t) x; /* 0: yes; 1..255: no */

    y -= 1;   /* 4294967295: yes; 0..254: no */
    y >>= 31; /* 1: yes; 0: no */

    return y;
}

static unsigned char
negative(signed char b)
{
    /* 18446744073709551361..18446744073709551615: yes; 0..255: no */
    uint64_t x = b;

    x >>= 63; /* 1: yes; 0: no */

    return x;
}

static void
ge25519_cmov(ge25519_precomp *t, const ge25519_precomp *u, unsigned char b)
{
    fe25519_cmov(t->yplusx, u->yplusx, b);
    fe25519_cmov(t->yminusx, u->yminusx, b);
    fe25519_cmov(t->xy2d, u->xy2d, b);
}

static void
ge25519_cmov_cached(ge25519_cached *t, const ge25519_cached *u, unsigned char b)
{
    fe25519_cmov(t->YplusX, u->YplusX, b);
    fe25519_cmov(t->YminusX, u->YminusX, b);
    fe25519_cmov(t->Z, u->Z, b);
    fe25519_cmov(t->T2d, u->T2d, b);
}

static void
ge25519_cmov8(ge25519_precomp *t, const ge25519_precomp precomp[8], const signed char b)
{
    ge25519_precomp     minust;
    const unsigned char bnegative = negative(b);
    const unsigned char babs      = b - (((-bnegative) & b) * ((signed char) 1 << 1));

    ge25519_precomp_0(t);
    ge25519_cmov(t, &precomp[0], equal(babs, 1));
    ge25519_cmov(t, &precomp[1], equal(babs, 2));
    ge25519_cmov(t, &precomp[2], equal(babs, 3));
    ge25519_cmov(t, &precomp[3], equal(babs, 4));
    ge25519_cmov(t, &precomp[4], equal(babs, 5));
    ge25519_cmov(t, &precomp[5], equal(babs, 6));
    ge25519_cmov(t, &precomp[6], equal(babs, 7));
    ge25519_cmov(t, &precomp[7], equal(babs, 8));
    fe25519_copy(minust.yplusx, t->yminusx);
    fe25519_copy(minust.yminusx, t->yplusx);
    fe25519_neg(minust.xy2d, t->xy2d);
    ge25519_cmov(t, &minust, bnegative);
}

static void
ge25519_cmov8_base(ge25519_precomp *t, const int pos, const signed char b)
{
    static const ge25519_precomp base[32][8] = { /* base[i][j] = (j+1)*256^i*B */
#ifdef HAVE_TI_MODE
# include "fe_51/base.h"
#else
# include "fe_25_5/base.h"
#endif
    };
    ge25519_cmov8(t, base[pos], b);
}

static void
ge25519_cmov8_cached(ge25519_cached *t, const ge25519_cached cached[8], const signed char b)
{
    ge25519_cached      minust;
    const unsigned char bnegative = negative(b);
    const unsigned char babs      = b - (((-bnegative) & b) * ((signed char) 1 << 1));

    ge25519_cached_0(t);
    ge25519_cmov_cached(t, &cached[0], equal(babs, 1));
    ge25519_cmov_cached(t, &cached[1], equal(babs, 2));
    ge25519_cmov_cached(t, &cached[2], equal(babs, 3));
    ge25519_cmov_cached(t, &cached[3], equal(babs, 4));
    ge25519_cmov_cached(t, &cached[4], equal(babs, 5));
    ge25519_cmov_cached(t, &cached[5], equal(babs, 6));
    ge25519_cmov_cached(t, &cached[6], equal(babs, 7));
    ge25519_cmov_cached(t, &cached[7], equal(babs, 8));
    fe25519_copy(minust.YplusX, t->YminusX);
    fe25519_copy(minust.YminusX, t->YplusX);
    fe25519_copy(minust.Z, t->Z);
    fe25519_neg(minust.T2d, t->T2d);
    ge25519_cmov_cached(t, &minust, bnegative);
}

/*
 r = p - q
 */

void
ge25519_sub_cached(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_cached *q)
{
    fe25519 t0;

    fe25519_add(r->X, p->Y, p->X);
    fe25519_sub(r->Y, p->Y, p->X);
    fe25519_mul(r->Z, r->X, q->YminusX);
    fe25519_mul(r->Y, r->Y, q->YplusX);
    fe25519_mul(r->T, q->T2d, p->T);
    fe25519_mul(r->X, p->Z, q->Z);
    fe25519_add(t0, r->X, r->X);
    fe25519_sub(r->X, r->Z, r->Y);
    fe25519_add(r->Y, r->Z, r->Y);
    fe25519_sub(r->Z, t0, r->T);
    fe25519_add(r->T, t0, r->T);
}

void
ge25519_tobytes(unsigned char *s, const ge25519_p2 *h)
{
    fe25519 recip;
    fe25519 x;
    fe25519 y;

    fe25519_invert(recip, h->Z);
    fe25519_mul(x, h->X, recip);
    fe25519_mul(y, h->Y, recip);
    fe25519_tobytes(s, y);
    s[31] ^= fe25519_isnegative(x) << 7;
}

/*
 r = a * A + b * B
 where a = a[0]+256*a[1]+...+256^31 a[31].
 and b = b[0]+256*b[1]+...+256^31 b[31].
 B is the Ed25519 base point (x,4/5) with x positive.

 Only used for signatures verification.
 */

void
ge25519_double_scalarmult_vartime(ge25519_p2 *r, const unsigned char *a,
                                  const ge25519_p3 *A, const unsigned char *b)
{
    static const ge25519_precomp Bi[8] = {
#ifdef HAVE_TI_MODE
# include "fe_51/base2.h"
#else
# include "fe_25_5/base2.h"
#endif
    };
    signed char    aslide[256];
    signed char    bslide[256];
    ge25519_cached Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */
    ge25519_p1p1   t;
    ge25519_p3     u;
    ge25519_p3     A2;
    int            i;

    slide_vartime(aslide, a);
    slide_vartime(bslide, b);

    ge25519_p3_to_cached(&Ai[0], A);

    ge25519_p3_dbl(&t, A);
    ge25519_p1p1_to_p3(&A2, &t);

    ge25519_add_cached(&t, &A2, &Ai[0]);
    ge25519_p1p1_to_p3(&u, &t);
    ge25519_p3_to_cached(&Ai[1], &u);

    ge25519_add_cached(&t, &A2, &Ai[1]);
    ge25519_p1p1_to_p3(&u, &t);
    ge25519_p3_to_cached(&Ai[2], &u);

    ge25519_add_cached(&t, &A2, &Ai[2]);
    ge25519_p1p1_to_p3(&u, &t);
    ge25519_p3_to_cached(&Ai[3], &u);

    ge25519_add_cached(&t, &A2, &Ai[3]);
    ge25519_p1p1_to_p3(&u, &t);
    ge25519_p3_to_cached(&Ai[4], &u);

    ge25519_add_cached(&t, &A2, &Ai[4]);
    ge25519_p1p1_to_p3(&u, &t);
    ge25519_p3_to_cached(&Ai[5], &u);

    ge25519_add_cached(&t, &A2, &Ai[5]);
    ge25519_p1p1_to_p3(&u, &t);
    ge25519_p3_to_cached(&Ai[6], &u);

    ge25519_add_cached(&t, &A2, &Ai[6]);
    ge25519_p1p1_to_p3(&u, &t);
    ge25519_p3_to_cached(&Ai[7], &u);

    ge25519_p2_0(r);

    for (i = 255; i >= 0; --i) {
        if (aslide[i] || bslide[i]) {
            break;
        }
    }

    for (; i >= 0; --i) {
        ge25519_p2_dbl(&t, r);

        if (aslide[i] > 0) {
            ge25519_p1p1_to_p3(&u, &t);
            ge25519_add_cached(&t, &u, &Ai[aslide[i] / 2]);
        } else if (aslide[i] < 0) {
            ge25519_p1p1_to_p3(&u, &t);
            ge25519_sub_cached(&t, &u, &Ai[(-aslide[i]) / 2]);
        }

        if (bslide[i] > 0) {
            ge25519_p1p1_to_p3(&u, &t);
            ge25519_add_precomp(&t, &u, &Bi[bslide[i] / 2]);
        } else if (bslide[i] < 0) {
            ge25519_p1p1_to_p3(&u, &t);
            ge25519_sub_precomp(&t, &u, &Bi[(-bslide[i]) / 2]);
        }

        ge25519_p1p1_to_p2(r, &t);
    }
}

/*
 h = a * p
 where a = a[0]+256*a[1]+...+256^31 a[31]

 Preconditions:
 a[31] <= 127

 p is public
 */

void
ge25519_scalarmult(ge25519_p3 *h, const unsigned char *a, const ge25519_p3 *p)
{
    signed char     e[64];
    signed char     carry;
    ge25519_p1p1    r;
    ge25519_p2      s;
    ge25519_p1p1    t2, t3, t4, t5, t6, t7, t8;
    ge25519_p3      p2, p3, p4, p5, p6, p7, p8;
    ge25519_cached  pi[8];
    ge25519_cached  t;
    int             i;

    ge25519_p3_to_cached(&pi[1 - 1], p);   /* p */

    ge25519_p3_dbl(&t2, p);
    ge25519_p1p1_to_p3(&p2, &t2);
    ge25519_p3_to_cached(&pi[2 - 1], &p2); /* 2p = 2*p */

    ge25519_add_cached(&t3, p, &pi[2 - 1]);
    ge25519_p1p1_to_p3(&p3, &t3);
    ge25519_p3_to_cached(&pi[3 - 1], &p3); /* 3p = 2p+p */

    ge25519_p3_dbl(&t4, &p2);
    ge25519_p1p1_to_p3(&p4, &t4);
    ge25519_p3_to_cached(&pi[4 - 1], &p4); /* 4p = 2*2p */

    ge25519_add_cached(&t5, p, &pi[4 - 1]);
    ge25519_p1p1_to_p3(&p5, &t5);
    ge25519_p3_to_cached(&pi[5 - 1], &p5); /* 5p = 4p+p */

    ge25519_p3_dbl(&t6, &p3);
    ge25519_p1p1_to_p3(&p6, &t6);
    ge25519_p3_to_cached(&pi[6 - 1], &p6); /* 6p = 2*3p */

    ge25519_add_cached(&t7, p, &pi[6 - 1]);
    ge25519_p1p1_to_p3(&p7, &t7);
    ge25519_p3_to_cached(&pi[7 - 1], &p7); /* 7p = 6p+p */

    ge25519_p3_dbl(&t8, &p4);
    ge25519_p1p1_to_p3(&p8, &t8);
    ge25519_p3_to_cached(&pi[8 - 1], &p8); /* 8p = 2*4p */

    for (i = 0; i < 32; ++i) {
        e[2 * i + 0] = (a[i] >> 0) & 15;
        e[2 * i + 1] = (a[i] >> 4) & 15;
    }
    /* each e[i] is between 0 and 15 */
    /* e[63] is between 0 and 7 */

    carry = 0;
    for (i = 0; i < 63; ++i) {
        e[i] += carry;
        carry = e[i] + 8;
        carry >>= 4;
        e[i] -= carry * ((signed char) 1 << 4);
    }
    e[63] += carry;
    /* each e[i] is between -8 and 8 */

    ge25519_p3_0(h);

    for (i = 63; i != 0; i--) {
        ge25519_cmov8_cached(&t, pi, e[i]);
        ge25519_add_cached(&r, h, &t);

        ge25519_p1p1_to_p2(&s, &r);
        ge25519_p2_dbl(&r, &s);
        ge25519_p1p1_to_p2(&s, &r);
        ge25519_p2_dbl(&r, &s);
        ge25519_p1p1_to_p2(&s, &r);
        ge25519_p2_dbl(&r, &s);
        ge25519_p1p1_to_p2(&s, &r);
        ge25519_p2_dbl(&r, &s);

        ge25519_p1p1_to_p3(h, &r);  /* *16 */
    }
    ge25519_cmov8_cached(&t, pi, e[i]);
    ge25519_add_cached(&r, h, &t);

    ge25519_p1p1_to_p3(h, &r);
}

/*
 h = a * B (with precomputation)
 where a = a[0]+256*a[1]+...+256^31 a[31]
 B is the Ed25519 base point (x,4/5) with x positive
 (as bytes: 0x5866666666666666666666666666666666666666666666666666666666666666)

 Preconditions:
 a[31] <= 127
 */

void
ge25519_scalarmult_base(ge25519_p3 *h, const unsigned char *a)
{
    signed char     e[64];
    signed char     carry;
    ge25519_p1p1    r;
    ge25519_p2      s;
    ge25519_precomp t;
    int             i;

    for (i = 0; i < 32; ++i) {
        e[2 * i + 0] = (a[i] >> 0) & 15;
        e[2 * i + 1] = (a[i] >> 4) & 15;
    }
    /* each e[i] is between 0 and 15 */
    /* e[63] is between 0 and 7 */

    carry = 0;
    for (i = 0; i < 63; ++i) {
        e[i] += carry;
        carry = e[i] + 8;
        carry >>= 4;
        e[i] -= carry * ((signed char) 1 << 4);
    }
    e[63] += carry;
    /* each e[i] is between -8 and 8 */

    ge25519_p3_0(h);

    for (i = 1; i < 64; i += 2) {
        ge25519_cmov8_base(&t, i / 2, e[i]);
        ge25519_add_precomp(&r, h, &t);
        ge25519_p1p1_to_p3(h, &r);
    }

    ge25519_p3_dbl(&r, h);
    ge25519_p1p1_to_p2(&s, &r);
    ge25519_p2_dbl(&r, &s);
    ge25519_p1p1_to_p2(&s, &r);
    ge25519_p2_dbl(&r, &s);
    ge25519_p1p1_to_p2(&s, &r);
    ge25519_p2_dbl(&r, &s);
    ge25519_p1p1_to_p3(h, &r);

    for (i = 0; i < 64; i += 2) {
        ge25519_cmov8_base(&t, i / 2, e[i]);
        ge25519_add_precomp(&r, h, &t);
        ge25519_p1p1_to_p3(h, &r);
    }
}

/* r = 2p */
static void
ge25519_p3p3_dbl(ge25519_p3 *r, const ge25519_p3 *p)
{
    ge25519_p1p1 p1p1;

    ge25519_p3_dbl(&p1p1, p);
    ge25519_p1p1_to_p3(r, &p1p1);
}

/* r = p+q */
static void
ge25519_p3_add(ge25519_p3 *r, const ge25519_p3 *p, const ge25519_p3 *q)
{
    ge25519_cached q_cached;
    ge25519_p1p1   p1p1;

    ge25519_p3_to_cached(&q_cached, q);
    ge25519_add_cached(&p1p1, p, &q_cached);
    ge25519_p1p1_to_p3(r, &p1p1);
}

/* r = r*(2^n)+q */
static void
ge25519_p3_dbladd(ge25519_p3 *r, const int n, const ge25519_p3 *q)
{
    ge25519_p2   p2;
    ge25519_p1p1 p1p1;
    int          i;

    ge25519_p3_to_p2(&p2, r);
    for (i = 0; i < n; i++) {
        ge25519_p2_dbl(&p1p1, &p2);
        ge25519_p1p1_to_p2(&p2, &p1p1);
    }
    ge25519_p1p1_to_p3(r, &p1p1);
    ge25519_p3_add(r, r, q);
}

/* multiply by the order of the main subgroup l = 2^252+27742317777372353535851937790883648493 */
static void
ge25519_mul_l(ge25519_p3 *r, const ge25519_p3 *p)
{
    ge25519_p3 _10, _11, _100, _110, _1000, _1011, _10000, _100000, _100110,
        _1000000, _1010000, _1010011, _1100011, _1100111, _1101011, _10010011,
        _10010111, _10111101, _11010011, _11100111, _11101101, _11110101;

    ge25519_p3p3_dbl(&_10, p);
    ge25519_p3_add(&_11, p, &_10);
    ge25519_p3_add(&_100, p, &_11);
    ge25519_p3_add(&_110, &_10, &_100);
    ge25519_p3_add(&_1000, &_10, &_110);
    ge25519_p3_add(&_1011, &_11, &_1000);
    ge25519_p3p3_dbl(&_10000, &_1000);
    ge25519_p3p3_dbl(&_100000, &_10000);
    ge25519_p3_add(&_100110, &_110, &_100000);
    ge25519_p3p3_dbl(&_1000000, &_100000);
    ge25519_p3_add(&_1010000, &_10000, &_1000000);
    ge25519_p3_add(&_1010011, &_11, &_1010000);
    ge25519_p3_add(&_1100011, &_10000, &_1010011);
    ge25519_p3_add(&_1100111, &_100, &_1100011);
    ge25519_p3_add(&_1101011, &_100, &_1100111);
    ge25519_p3_add(&_10010011, &_1000000, &_1010011);
    ge25519_p3_add(&_10010111, &_100, &_10010011);
    ge25519_p3_add(&_10111101, &_100110, &_10010111);
    ge25519_p3_add(&_11010011, &_1000000, &_10010011);
    ge25519_p3_add(&_11100111, &_1010000, &_10010111);
    ge25519_p3_add(&_11101101, &_110, &_11100111);
    ge25519_p3_add(&_11110101, &_1000, &_11101101);

    ge25519_p3_add(r, &_1011, &_11110101);
    ge25519_p3_dbladd(r, 126, &_1010011);
    ge25519_p3_dbladd(r, 9, &_10);
    ge25519_p3_add(r, r, &_11110101);
    ge25519_p3_dbladd(r, 7, &_1100111);
    ge25519_p3_dbladd(r, 9, &_11110101);
    ge25519_p3_dbladd(r, 11, &_10111101);
    ge25519_p3_dbladd(r, 8, &_11100111);
    ge25519_p3_dbladd(r, 9, &_1101011);
    ge25519_p3_dbladd(r, 6, &_1011);
    ge25519_p3_dbladd(r, 14, &_10010011);
    ge25519_p3_dbladd(r, 10, &_1100011);
    ge25519_p3_dbladd(r, 9, &_10010111);
    ge25519_p3_dbladd(r, 10, &_11110101);
    ge25519_p3_dbladd(r, 8, &_11010011);
    ge25519_p3_dbladd(r, 8, &_11101101);
}

int
ge25519_is_on_curve(const ge25519_p3 *p)
{
    fe25519 x2;
    fe25519 y2;
    fe25519 z2;
    fe25519 z4;
    fe25519 t0;
    fe25519 t1;

    fe25519_sq(x2, p->X);
    fe25519_sq(y2, p->Y);
    fe25519_sq(z2, p->Z);
    fe25519_sub(t0, y2, x2);
    fe25519_mul(t0, t0, z2);

    fe25519_mul(t1, x2, y2);
    fe25519_mul(t1, t1, ed25519_d);
    fe25519_sq(z4, z2);
    fe25519_add(t1, t1, z4);
    fe25519_sub(t0, t0, t1);

    return fe25519_iszero(t0);
}

int
ge25519_is_on_main_subgroup(const ge25519_p3 *p)
{
    ge25519_p3 pl;

    ge25519_mul_l(&pl, p);

    return fe25519_iszero(pl.X);
}

int
ge25519_is_canonical(const unsigned char *s)
{
    unsigned char c;
    unsigned char d;
    unsigned int  i;

    c = (s[31] & 0x7f) ^ 0x7f;
    for (i = 30; i > 0; i--) {
        c |= s[i] ^ 0xff;
    }
    c = (((unsigned int) c) - 1U) >> 8;
    d = (0xed - 1U - (unsigned int) s[0]) >> 8;

    return 1 - (c & d & 1);
}

int
ge25519_has_small_order(const unsigned char s[32])
{
    CRYPTO_ALIGN(16)
    static const unsigned char blocklist[][32] = {
        /* 0 (order 4) */
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        /* 1 (order 1) */
        { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        /* 2707385501144840649318225287225658788936804267575313519463743609750303402022
           (order 8) */
        { 0x26, 0xe8, 0x95, 0x8f, 0xc2, 0xb2, 0x27, 0xb0, 0x45, 0xc3, 0xf4,
          0x89, 0xf2, 0xef, 0x98, 0xf0, 0xd5, 0xdf, 0xac, 0x05, 0xd3, 0xc6,
          0x33, 0x39, 0xb1, 0x38, 0x02, 0x88, 0x6d, 0x53, 0xfc, 0x05 },
        /* 55188659117513257062467267217118295137698188065244968500265048394206261417927
           (order 8) */
        { 0xc7, 0x17, 0x6a, 0x70, 0x3d, 0x4d, 0xd8, 0x4f, 0xba, 0x3c, 0x0b,
          0x76, 0x0d, 0x10, 0x67, 0x0f, 0x2a, 0x20, 0x53, 0xfa, 0x2c, 0x39,
          0xcc, 0xc6, 0x4e, 0xc7, 0xfd, 0x77, 0x92, 0xac, 0x03, 0x7a },
        /* p-1 (order 2) */
        { 0xec, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f },
        /* p (=0, order 4) */
        { 0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f },
        /* p+1 (=1, order 1) */
        { 0xee, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f }
    };
    unsigned char c[7] = { 0 };
    unsigned int  k;
    size_t        i, j;

    COMPILER_ASSERT(7 == sizeof blocklist / sizeof blocklist[0]);
    for (j = 0; j < 31; j++) {
        for (i = 0; i < sizeof blocklist / sizeof blocklist[0]; i++) {
            c[i] |= s[j] ^ blocklist[i][j];
        }
    }
    for (i = 0; i < sizeof blocklist / sizeof blocklist[0]; i++) {
        c[i] |= (s[j] & 0x7f) ^ blocklist[i][j];
    }
    k = 0;
    for (i = 0; i < sizeof blocklist / sizeof blocklist[0]; i++) {
        k |= (c[i] - 1);
    }
    return (int) ((k >> 8) & 1);
}

/*
 Input:
 a[0]+256*a[1]+...+256^31*a[31] = a
 b[0]+256*b[1]+...+256^31*b[31] = b
 *
 Output:
 s[0]+256*s[1]+...+256^31*s[31] = (ab) mod l
 where l = 2^252 + 27742317777372353535851937790883648493.
 */

void
sc25519_mul(unsigned char s[32], const unsigned char a[32], const unsigned char b[32])
{
    int64_t a0  = 2097151 & load_3(a);
    int64_t a1  = 2097151 & (load_4(a + 2) >> 5);
    int64_t a2  = 2097151 & (load_3(a + 5) >> 2);
    int64_t a3  = 2097151 & (load_4(a + 7) >> 7);
    int64_t a4  = 2097151 & (load_4(a + 10) >> 4);
    int64_t a5  = 2097151 & (load_3(a + 13) >> 1);
    int64_t a6  = 2097151 & (load_4(a + 15) >> 6);
    int64_t a7  = 2097151 & (load_3(a + 18) >> 3);
    int64_t a8  = 2097151 & load_3(a + 21);
    int64_t a9  = 2097151 & (load_4(a + 23) >> 5);
    int64_t a10 = 2097151 & (load_3(a + 26) >> 2);
    int64_t a11 = (load_4(a + 28) >> 7);

    int64_t b0  = 2097151 & load_3(b);
    int64_t b1  = 2097151 & (load_4(b + 2) >> 5);
    int64_t b2  = 2097151 & (load_3(b + 5) >> 2);
    int64_t b3  = 2097151 & (load_4(b + 7) >> 7);
    int64_t b4  = 2097151 & (load_4(b + 10) >> 4);
    int64_t b5  = 2097151 & (load_3(b + 13) >> 1);
    int64_t b6  = 2097151 & (load_4(b + 15) >> 6);
    int64_t b7  = 2097151 & (load_3(b + 18) >> 3);
    int64_t b8  = 2097151 & load_3(b + 21);
    int64_t b9  = 2097151 & (load_4(b + 23) >> 5);
    int64_t b10 = 2097151 & (load_3(b + 26) >> 2);
    int64_t b11 = (load_4(b + 28) >> 7);

    int64_t s0;
    int64_t s1;
    int64_t s2;
    int64_t s3;
    int64_t s4;
    int64_t s5;
    int64_t s6;
    int64_t s7;
    int64_t s8;
    int64_t s9;
    int64_t s10;
    int64_t s11;
    int64_t s12;
    int64_t s13;
    int64_t s14;
    int64_t s15;
    int64_t s16;
    int64_t s17;
    int64_t s18;
    int64_t s19;
    int64_t s20;
    int64_t s21;
    int64_t s22;
    int64_t s23;

    int64_t carry0;
    int64_t carry1;
    int64_t carry2;
    int64_t carry3;
    int64_t carry4;
    int64_t carry5;
    int64_t carry6;
    int64_t carry7;
    int64_t carry8;
    int64_t carry9;
    int64_t carry10;
    int64_t carry11;
    int64_t carry12;
    int64_t carry13;
    int64_t carry14;
    int64_t carry15;
    int64_t carry16;
    int64_t carry17;
    int64_t carry18;
    int64_t carry19;
    int64_t carry20;
    int64_t carry21;
    int64_t carry22;

    s0 = a0 * b0;
    s1 = a0 * b1 + a1 * b0;
    s2 = a0 * b2 + a1 * b1 + a2 * b0;
    s3 = a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
    s4 = a0 * b4 + a1 * b3 + a2 * b2 + a3 * b1 + a4 * b0;
    s5 = a0 * b5 + a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1 + a5 * b0;
    s6 = a0 * b6 + a1 * b5 + a2 * b4 + a3 * b3 + a4 * b2 + a5 * b1 + a6 * b0;
    s7 = a0 * b7 + a1 * b6 + a2 * b5 + a3 * b4 + a4 * b3 + a5 * b2 +
         a6 * b1 + a7 * b0;
    s8 = a0 * b8 + a1 * b7 + a2 * b6 + a3 * b5 + a4 * b4 + a5 * b3 +
         a6 * b2 + a7 * b1 + a8 * b0;
    s9 = a0 * b9 + a1 * b8 + a2 * b7 + a3 * b6 + a4 * b5 + a5 * b4 +
         a6 * b3 + a7 * b2 + a8 * b1 + a9 * b0;
    s10 = a0 * b10 + a1 * b9 + a2 * b8 + a3 * b7 + a4 * b6 + a5 * b5 +
          a6 * b4 + a7 * b3 + a8 * b2 + a9 * b1 + a10 * b0;
    s11 = a0 * b11 + a1 * b10 + a2 * b9 + a3 * b8 + a4 * b7 + a5 * b6 +
          a6 * b5 + a7 * b4 + a8 * b3 + a9 * b2 + a10 * b1 + a11 * b0;
    s12 = a1 * b11 + a2 * b10 + a3 * b9 + a4 * b8 + a5 * b7 + a6 * b6 +
          a7 * b5 + a8 * b4 + a9 * b3 + a10 * b2 + a11 * b1;
    s13 = a2 * b11 + a3 * b10 + a4 * b9 + a5 * b8 + a6 * b7 + a7 * b6 +
          a8 * b5 + a9 * b4 + a10 * b3 + a11 * b2;
    s14 = a3 * b11 + a4 * b10 + a5 * b9 + a6 * b8 + a7 * b7 + a8 * b6 +
          a9 * b5 + a10 * b4 + a11 * b3;
    s15 = a4 * b11 + a5 * b10 + a6 * b9 + a7 * b8 + a8 * b7 + a9 * b6 +
          a10 * b5 + a11 * b4;
    s16 =
        a5 * b11 + a6 * b10 + a7 * b9 + a8 * b8 + a9 * b7 + a10 * b6 + a11 * b5;
    s17 = a6 * b11 + a7 * b10 + a8 * b9 + a9 * b8 + a10 * b7 + a11 * b6;
    s18 = a7 * b11 + a8 * b10 + a9 * b9 + a10 * b8 + a11 * b7;
    s19 = a8 * b11 + a9 * b10 + a10 * b9 + a11 * b8;
    s20 = a9 * b11 + a10 * b10 + a11 * b9;
    s21 = a10 * b11 + a11 * b10;
    s22 = a11 * b11;
    s23 = 0;

    carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);
    carry18 = (s18 + (int64_t) (1L << 20)) >> 21;
    s19 += carry18;
    s18 -= carry18 * ((uint64_t) 1L << 21);
    carry20 = (s20 + (int64_t) (1L << 20)) >> 21;
    s21 += carry20;
    s20 -= carry20 * ((uint64_t) 1L << 21);
    carry22 = (s22 + (int64_t) (1L << 20)) >> 21;
    s23 += carry22;
    s22 -= carry22 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);
    carry17 = (s17 + (int64_t) (1L << 20)) >> 21;
    s18 += carry17;
    s17 -= carry17 * ((uint64_t) 1L << 21);
    carry19 = (s19 + (int64_t) (1L << 20)) >> 21;
    s20 += carry19;
    s19 -= carry19 * ((uint64_t) 1L << 21);
    carry21 = (s21 + (int64_t) (1L << 20)) >> 21;
    s22 += carry21;
    s21 -= carry21 * ((uint64_t) 1L << 21);

    s11 += s23 * 666643;
    s12 += s23 * 470296;
    s13 += s23 * 654183;
    s14 -= s23 * 997805;
    s15 += s23 * 136657;
    s16 -= s23 * 683901;

    s10 += s22 * 666643;
    s11 += s22 * 470296;
    s12 += s22 * 654183;
    s13 -= s22 * 997805;
    s14 += s22 * 136657;
    s15 -= s22 * 683901;

    s9 += s21 * 666643;
    s10 += s21 * 470296;
    s11 += s21 * 654183;
    s12 -= s21 * 997805;
    s13 += s21 * 136657;
    s14 -= s21 * 683901;

    s8 += s20 * 666643;
    s9 += s20 * 470296;
    s10 += s20 * 654183;
    s11 -= s20 * 997805;
    s12 += s20 * 136657;
    s13 -= s20 * 683901;

    s7 += s19 * 666643;
    s8 += s19 * 470296;
    s9 += s19 * 654183;
    s10 -= s19 * 997805;
    s11 += s19 * 136657;
    s12 -= s19 * 683901;

    s6 += s18 * 666643;
    s7 += s18 * 470296;
    s8 += s18 * 654183;
    s9 -= s18 * 997805;
    s10 += s18 * 136657;
    s11 -= s18 * 683901;

    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);

    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);

    s5 += s17 * 666643;
    s6 += s17 * 470296;
    s7 += s17 * 654183;
    s8 -= s17 * 997805;
    s9 += s17 * 136657;
    s10 -= s17 * 683901;

    s4 += s16 * 666643;
    s5 += s16 * 470296;
    s6 += s16 * 654183;
    s7 -= s16 * 997805;
    s8 += s16 * 136657;
    s9 -= s16 * 683901;

    s3 += s15 * 666643;
    s4 += s15 * 470296;
    s5 += s15 * 654183;
    s6 -= s15 * 997805;
    s7 += s15 * 136657;
    s8 -= s15 * 683901;

    s2 += s14 * 666643;
    s3 += s14 * 470296;
    s4 += s14 * 654183;
    s5 -= s14 * 997805;
    s6 += s14 * 136657;
    s7 -= s14 * 683901;

    s1 += s13 * 666643;
    s2 += s13 * 470296;
    s3 += s13 * 654183;
    s4 -= s13 * 997805;
    s5 += s13 * 136657;
    s6 -= s13 * 683901;

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry11 = s11 >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    s[0]  = s0 >> 0;
    s[1]  = s0 >> 8;
    s[2]  = (s0 >> 16) | (s1 * ((uint64_t) 1 << 5));
    s[3]  = s1 >> 3;
    s[4]  = s1 >> 11;
    s[5]  = (s1 >> 19) | (s2 * ((uint64_t) 1 << 2));
    s[6]  = s2 >> 6;
    s[7]  = (s2 >> 14) | (s3 * ((uint64_t) 1 << 7));
    s[8]  = s3 >> 1;
    s[9]  = s3 >> 9;
    s[10] = (s3 >> 17) | (s4 * ((uint64_t) 1 << 4));
    s[11] = s4 >> 4;
    s[12] = s4 >> 12;
    s[13] = (s4 >> 20) | (s5 * ((uint64_t) 1 << 1));
    s[14] = s5 >> 7;
    s[15] = (s5 >> 15) | (s6 * ((uint64_t) 1 << 6));
    s[16] = s6 >> 2;
    s[17] = s6 >> 10;
    s[18] = (s6 >> 18) | (s7 * ((uint64_t) 1 << 3));
    s[19] = s7 >> 5;
    s[20] = s7 >> 13;
    s[21] = s8 >> 0;
    s[22] = s8 >> 8;
    s[23] = (s8 >> 16) | (s9 * ((uint64_t) 1 << 5));
    s[24] = s9 >> 3;
    s[25] = s9 >> 11;
    s[26] = (s9 >> 19) | (s10 * ((uint64_t) 1 << 2));
    s[27] = s10 >> 6;
    s[28] = (s10 >> 14) | (s11 * ((uint64_t) 1 << 7));
    s[29] = s11 >> 1;
    s[30] = s11 >> 9;
    s[31] = s11 >> 17;
}

/*
 Input:
 a[0]+256*a[1]+...+256^31*a[31] = a
 b[0]+256*b[1]+...+256^31*b[31] = b
 c[0]+256*c[1]+...+256^31*c[31] = c
 *
 Output:
 s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
 where l = 2^252 + 27742317777372353535851937790883648493.
 */

void
sc25519_muladd(unsigned char s[32], const unsigned char a[32],
               const unsigned char b[32], const unsigned char c[32])
{
    int64_t a0  = 2097151 & load_3(a);
    int64_t a1  = 2097151 & (load_4(a + 2) >> 5);
    int64_t a2  = 2097151 & (load_3(a + 5) >> 2);
    int64_t a3  = 2097151 & (load_4(a + 7) >> 7);
    int64_t a4  = 2097151 & (load_4(a + 10) >> 4);
    int64_t a5  = 2097151 & (load_3(a + 13) >> 1);
    int64_t a6  = 2097151 & (load_4(a + 15) >> 6);
    int64_t a7  = 2097151 & (load_3(a + 18) >> 3);
    int64_t a8  = 2097151 & load_3(a + 21);
    int64_t a9  = 2097151 & (load_4(a + 23) >> 5);
    int64_t a10 = 2097151 & (load_3(a + 26) >> 2);
    int64_t a11 = (load_4(a + 28) >> 7);

    int64_t b0  = 2097151 & load_3(b);
    int64_t b1  = 2097151 & (load_4(b + 2) >> 5);
    int64_t b2  = 2097151 & (load_3(b + 5) >> 2);
    int64_t b3  = 2097151 & (load_4(b + 7) >> 7);
    int64_t b4  = 2097151 & (load_4(b + 10) >> 4);
    int64_t b5  = 2097151 & (load_3(b + 13) >> 1);
    int64_t b6  = 2097151 & (load_4(b + 15) >> 6);
    int64_t b7  = 2097151 & (load_3(b + 18) >> 3);
    int64_t b8  = 2097151 & load_3(b + 21);
    int64_t b9  = 2097151 & (load_4(b + 23) >> 5);
    int64_t b10 = 2097151 & (load_3(b + 26) >> 2);
    int64_t b11 = (load_4(b + 28) >> 7);

    int64_t c0  = 2097151 & load_3(c);
    int64_t c1  = 2097151 & (load_4(c + 2) >> 5);
    int64_t c2  = 2097151 & (load_3(c + 5) >> 2);
    int64_t c3  = 2097151 & (load_4(c + 7) >> 7);
    int64_t c4  = 2097151 & (load_4(c + 10) >> 4);
    int64_t c5  = 2097151 & (load_3(c + 13) >> 1);
    int64_t c6  = 2097151 & (load_4(c + 15) >> 6);
    int64_t c7  = 2097151 & (load_3(c + 18) >> 3);
    int64_t c8  = 2097151 & load_3(c + 21);
    int64_t c9  = 2097151 & (load_4(c + 23) >> 5);
    int64_t c10 = 2097151 & (load_3(c + 26) >> 2);
    int64_t c11 = (load_4(c + 28) >> 7);

    int64_t s0;
    int64_t s1;
    int64_t s2;
    int64_t s3;
    int64_t s4;
    int64_t s5;
    int64_t s6;
    int64_t s7;
    int64_t s8;
    int64_t s9;
    int64_t s10;
    int64_t s11;
    int64_t s12;
    int64_t s13;
    int64_t s14;
    int64_t s15;
    int64_t s16;
    int64_t s17;
    int64_t s18;
    int64_t s19;
    int64_t s20;
    int64_t s21;
    int64_t s22;
    int64_t s23;

    int64_t carry0;
    int64_t carry1;
    int64_t carry2;
    int64_t carry3;
    int64_t carry4;
    int64_t carry5;
    int64_t carry6;
    int64_t carry7;
    int64_t carry8;
    int64_t carry9;
    int64_t carry10;
    int64_t carry11;
    int64_t carry12;
    int64_t carry13;
    int64_t carry14;
    int64_t carry15;
    int64_t carry16;
    int64_t carry17;
    int64_t carry18;
    int64_t carry19;
    int64_t carry20;
    int64_t carry21;
    int64_t carry22;

    s0 = c0 + a0 * b0;
    s1 = c1 + a0 * b1 + a1 * b0;
    s2 = c2 + a0 * b2 + a1 * b1 + a2 * b0;
    s3 = c3 + a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
    s4 = c4 + a0 * b4 + a1 * b3 + a2 * b2 + a3 * b1 + a4 * b0;
    s5 = c5 + a0 * b5 + a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1 + a5 * b0;
    s6 = c6 + a0 * b6 + a1 * b5 + a2 * b4 + a3 * b3 + a4 * b2 + a5 * b1 +
         a6 * b0;
    s7 = c7 + a0 * b7 + a1 * b6 + a2 * b5 + a3 * b4 + a4 * b3 + a5 * b2 +
         a6 * b1 + a7 * b0;
    s8 = c8 + a0 * b8 + a1 * b7 + a2 * b6 + a3 * b5 + a4 * b4 + a5 * b3 +
         a6 * b2 + a7 * b1 + a8 * b0;
    s9 = c9 + a0 * b9 + a1 * b8 + a2 * b7 + a3 * b6 + a4 * b5 + a5 * b4 +
         a6 * b3 + a7 * b2 + a8 * b1 + a9 * b0;
    s10 = c10 + a0 * b10 + a1 * b9 + a2 * b8 + a3 * b7 + a4 * b6 + a5 * b5 +
          a6 * b4 + a7 * b3 + a8 * b2 + a9 * b1 + a10 * b0;
    s11 = c11 + a0 * b11 + a1 * b10 + a2 * b9 + a3 * b8 + a4 * b7 + a5 * b6 +
          a6 * b5 + a7 * b4 + a8 * b3 + a9 * b2 + a10 * b1 + a11 * b0;
    s12 = a1 * b11 + a2 * b10 + a3 * b9 + a4 * b8 + a5 * b7 + a6 * b6 +
          a7 * b5 + a8 * b4 + a9 * b3 + a10 * b2 + a11 * b1;
    s13 = a2 * b11 + a3 * b10 + a4 * b9 + a5 * b8 + a6 * b7 + a7 * b6 +
          a8 * b5 + a9 * b4 + a10 * b3 + a11 * b2;
    s14 = a3 * b11 + a4 * b10 + a5 * b9 + a6 * b8 + a7 * b7 + a8 * b6 +
          a9 * b5 + a10 * b4 + a11 * b3;
    s15 = a4 * b11 + a5 * b10 + a6 * b9 + a7 * b8 + a8 * b7 + a9 * b6 +
          a10 * b5 + a11 * b4;
    s16 =
        a5 * b11 + a6 * b10 + a7 * b9 + a8 * b8 + a9 * b7 + a10 * b6 + a11 * b5;
    s17 = a6 * b11 + a7 * b10 + a8 * b9 + a9 * b8 + a10 * b7 + a11 * b6;
    s18 = a7 * b11 + a8 * b10 + a9 * b9 + a10 * b8 + a11 * b7;
    s19 = a8 * b11 + a9 * b10 + a10 * b9 + a11 * b8;
    s20 = a9 * b11 + a10 * b10 + a11 * b9;
    s21 = a10 * b11 + a11 * b10;
    s22 = a11 * b11;
    s23 = 0;

    carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);
    carry18 = (s18 + (int64_t) (1L << 20)) >> 21;
    s19 += carry18;
    s18 -= carry18 * ((uint64_t) 1L << 21);
    carry20 = (s20 + (int64_t) (1L << 20)) >> 21;
    s21 += carry20;
    s20 -= carry20 * ((uint64_t) 1L << 21);
    carry22 = (s22 + (int64_t) (1L << 20)) >> 21;
    s23 += carry22;
    s22 -= carry22 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);
    carry17 = (s17 + (int64_t) (1L << 20)) >> 21;
    s18 += carry17;
    s17 -= carry17 * ((uint64_t) 1L << 21);
    carry19 = (s19 + (int64_t) (1L << 20)) >> 21;
    s20 += carry19;
    s19 -= carry19 * ((uint64_t) 1L << 21);
    carry21 = (s21 + (int64_t) (1L << 20)) >> 21;
    s22 += carry21;
    s21 -= carry21 * ((uint64_t) 1L << 21);

    s11 += s23 * 666643;
    s12 += s23 * 470296;
    s13 += s23 * 654183;
    s14 -= s23 * 997805;
    s15 += s23 * 136657;
    s16 -= s23 * 683901;

    s10 += s22 * 666643;
    s11 += s22 * 470296;
    s12 += s22 * 654183;
    s13 -= s22 * 997805;
    s14 += s22 * 136657;
    s15 -= s22 * 683901;

    s9 += s21 * 666643;
    s10 += s21 * 470296;
    s11 += s21 * 654183;
    s12 -= s21 * 997805;
    s13 += s21 * 136657;
    s14 -= s21 * 683901;

    s8 += s20 * 666643;
    s9 += s20 * 470296;
    s10 += s20 * 654183;
    s11 -= s20 * 997805;
    s12 += s20 * 136657;
    s13 -= s20 * 683901;

    s7 += s19 * 666643;
    s8 += s19 * 470296;
    s9 += s19 * 654183;
    s10 -= s19 * 997805;
    s11 += s19 * 136657;
    s12 -= s19 * 683901;

    s6 += s18 * 666643;
    s7 += s18 * 470296;
    s8 += s18 * 654183;
    s9 -= s18 * 997805;
    s10 += s18 * 136657;
    s11 -= s18 * 683901;

    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);

    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);

    s5 += s17 * 666643;
    s6 += s17 * 470296;
    s7 += s17 * 654183;
    s8 -= s17 * 997805;
    s9 += s17 * 136657;
    s10 -= s17 * 683901;

    s4 += s16 * 666643;
    s5 += s16 * 470296;
    s6 += s16 * 654183;
    s7 -= s16 * 997805;
    s8 += s16 * 136657;
    s9 -= s16 * 683901;

    s3 += s15 * 666643;
    s4 += s15 * 470296;
    s5 += s15 * 654183;
    s6 -= s15 * 997805;
    s7 += s15 * 136657;
    s8 -= s15 * 683901;

    s2 += s14 * 666643;
    s3 += s14 * 470296;
    s4 += s14 * 654183;
    s5 -= s14 * 997805;
    s6 += s14 * 136657;
    s7 -= s14 * 683901;

    s1 += s13 * 666643;
    s2 += s13 * 470296;
    s3 += s13 * 654183;
    s4 -= s13 * 997805;
    s5 += s13 * 136657;
    s6 -= s13 * 683901;

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry11 = s11 >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    s[0]  = s0 >> 0;
    s[1]  = s0 >> 8;
    s[2]  = (s0 >> 16) | (s1 * ((uint64_t) 1 << 5));
    s[3]  = s1 >> 3;
    s[4]  = s1 >> 11;
    s[5]  = (s1 >> 19) | (s2 * ((uint64_t) 1 << 2));
    s[6]  = s2 >> 6;
    s[7]  = (s2 >> 14) | (s3 * ((uint64_t) 1 << 7));
    s[8]  = s3 >> 1;
    s[9]  = s3 >> 9;
    s[10] = (s3 >> 17) | (s4 * ((uint64_t) 1 << 4));
    s[11] = s4 >> 4;
    s[12] = s4 >> 12;
    s[13] = (s4 >> 20) | (s5 * ((uint64_t) 1 << 1));
    s[14] = s5 >> 7;
    s[15] = (s5 >> 15) | (s6 * ((uint64_t) 1 << 6));
    s[16] = s6 >> 2;
    s[17] = s6 >> 10;
    s[18] = (s6 >> 18) | (s7 * ((uint64_t) 1 << 3));
    s[19] = s7 >> 5;
    s[20] = s7 >> 13;
    s[21] = s8 >> 0;
    s[22] = s8 >> 8;
    s[23] = (s8 >> 16) | (s9 * ((uint64_t) 1 << 5));
    s[24] = s9 >> 3;
    s[25] = s9 >> 11;
    s[26] = (s9 >> 19) | (s10 * ((uint64_t) 1 << 2));
    s[27] = s10 >> 6;
    s[28] = (s10 >> 14) | (s11 * ((uint64_t) 1 << 7));
    s[29] = s11 >> 1;
    s[30] = s11 >> 9;
    s[31] = s11 >> 17;
}

/*
 Input:
 a[0]+256*a[1]+...+256^31*a[31] = a
 *
 Output:
 s[0]+256*s[1]+...+256^31*s[31] = a^2 mod l
 where l = 2^252 + 27742317777372353535851937790883648493.
 */

static inline void
sc25519_sq(unsigned char *s, const unsigned char *a)
{
    sc25519_mul(s, a, a);
}

/*
 Input:
 s[0]+256*a[1]+...+256^31*a[31] = a
 n
 *
 Output:
 s[0]+256*s[1]+...+256^31*s[31] = x * s^(s^n) mod l
 where l = 2^252 + 27742317777372353535851937790883648493.
 Overwrites s in place.
 */

static inline void
sc25519_sqmul(unsigned char s[32], const int n, const unsigned char a[32])
{
    int i;

    for (i = 0; i < n; i++) {
        sc25519_sq(s, s);
    }
    sc25519_mul(s, s, a);
}

void
sc25519_invert(unsigned char recip[32], const unsigned char s[32])
{
    unsigned char _10[32], _100[32], _1000[32], _10000[32], _100000[32],
        _1000000[32], _10010011[32], _10010111[32], _100110[32], _1010[32],
        _1010000[32], _1010011[32], _1011[32], _10110[32], _10111101[32],
        _11[32], _1100011[32], _1100111[32], _11010011[32], _1101011[32],
        _11100111[32], _11101011[32], _11110101[32];

    sc25519_sq(_10, s);
    sc25519_mul(_11, s, _10);
    sc25519_mul(_100, s, _11);
    sc25519_sq(_1000, _100);
    sc25519_mul(_1010, _10, _1000);
    sc25519_mul(_1011, s, _1010);
    sc25519_sq(_10000, _1000);
    sc25519_sq(_10110, _1011);
    sc25519_mul(_100000, _1010, _10110);
    sc25519_mul(_100110, _10000, _10110);
    sc25519_sq(_1000000, _100000);
    sc25519_mul(_1010000, _10000, _1000000);
    sc25519_mul(_1010011, _11, _1010000);
    sc25519_mul(_1100011, _10000, _1010011);
    sc25519_mul(_1100111, _100, _1100011);
    sc25519_mul(_1101011, _100, _1100111);
    sc25519_mul(_10010011, _1000000, _1010011);
    sc25519_mul(_10010111, _100, _10010011);
    sc25519_mul(_10111101, _100110, _10010111);
    sc25519_mul(_11010011, _10110, _10111101);
    sc25519_mul(_11100111, _1010000, _10010111);
    sc25519_mul(_11101011, _100, _11100111);
    sc25519_mul(_11110101, _1010, _11101011);

    sc25519_mul(recip, _1011, _11110101);
    sc25519_sqmul(recip, 126, _1010011);
    sc25519_sqmul(recip, 9, _10);
    sc25519_mul(recip, recip, _11110101);
    sc25519_sqmul(recip, 7, _1100111);
    sc25519_sqmul(recip, 9, _11110101);
    sc25519_sqmul(recip, 11, _10111101);
    sc25519_sqmul(recip, 8, _11100111);
    sc25519_sqmul(recip, 9, _1101011);
    sc25519_sqmul(recip, 6, _1011);
    sc25519_sqmul(recip, 14, _10010011);
    sc25519_sqmul(recip, 10, _1100011);
    sc25519_sqmul(recip, 9, _10010111);
    sc25519_sqmul(recip, 10, _11110101);
    sc25519_sqmul(recip, 8, _11010011);
    sc25519_sqmul(recip, 8, _11101011);
}

/*
 Input:
 s[0]+256*s[1]+...+256^63*s[63] = s
 *
 Output:
 s[0]+256*s[1]+...+256^31*s[31] = s mod l
 where l = 2^252 + 27742317777372353535851937790883648493.
 Overwrites s in place.
 */

void
sc25519_reduce(unsigned char s[64])
{
    int64_t s0  = 2097151 & load_3(s);
    int64_t s1  = 2097151 & (load_4(s + 2) >> 5);
    int64_t s2  = 2097151 & (load_3(s + 5) >> 2);
    int64_t s3  = 2097151 & (load_4(s + 7) >> 7);
    int64_t s4  = 2097151 & (load_4(s + 10) >> 4);
    int64_t s5  = 2097151 & (load_3(s + 13) >> 1);
    int64_t s6  = 2097151 & (load_4(s + 15) >> 6);
    int64_t s7  = 2097151 & (load_3(s + 18) >> 3);
    int64_t s8  = 2097151 & load_3(s + 21);
    int64_t s9  = 2097151 & (load_4(s + 23) >> 5);
    int64_t s10 = 2097151 & (load_3(s + 26) >> 2);
    int64_t s11 = 2097151 & (load_4(s + 28) >> 7);
    int64_t s12 = 2097151 & (load_4(s + 31) >> 4);
    int64_t s13 = 2097151 & (load_3(s + 34) >> 1);
    int64_t s14 = 2097151 & (load_4(s + 36) >> 6);
    int64_t s15 = 2097151 & (load_3(s + 39) >> 3);
    int64_t s16 = 2097151 & load_3(s + 42);
    int64_t s17 = 2097151 & (load_4(s + 44) >> 5);
    int64_t s18 = 2097151 & (load_3(s + 47) >> 2);
    int64_t s19 = 2097151 & (load_4(s + 49) >> 7);
    int64_t s20 = 2097151 & (load_4(s + 52) >> 4);
    int64_t s21 = 2097151 & (load_3(s + 55) >> 1);
    int64_t s22 = 2097151 & (load_4(s + 57) >> 6);
    int64_t s23 = (load_4(s + 60) >> 3);

    int64_t carry0;
    int64_t carry1;
    int64_t carry2;
    int64_t carry3;
    int64_t carry4;
    int64_t carry5;
    int64_t carry6;
    int64_t carry7;
    int64_t carry8;
    int64_t carry9;
    int64_t carry10;
    int64_t carry11;
    int64_t carry12;
    int64_t carry13;
    int64_t carry14;
    int64_t carry15;
    int64_t carry16;

    s11 += s23 * 666643;
    s12 += s23 * 470296;
    s13 += s23 * 654183;
    s14 -= s23 * 997805;
    s15 += s23 * 136657;
    s16 -= s23 * 683901;

    s10 += s22 * 666643;
    s11 += s22 * 470296;
    s12 += s22 * 654183;
    s13 -= s22 * 997805;
    s14 += s22 * 136657;
    s15 -= s22 * 683901;

    s9 += s21 * 666643;
    s10 += s21 * 470296;
    s11 += s21 * 654183;
    s12 -= s21 * 997805;
    s13 += s21 * 136657;
    s14 -= s21 * 683901;

    s8 += s20 * 666643;
    s9 += s20 * 470296;
    s10 += s20 * 654183;
    s11 -= s20 * 997805;
    s12 += s20 * 136657;
    s13 -= s20 * 683901;

    s7 += s19 * 666643;
    s8 += s19 * 470296;
    s9 += s19 * 654183;
    s10 -= s19 * 997805;
    s11 += s19 * 136657;
    s12 -= s19 * 683901;

    s6 += s18 * 666643;
    s7 += s18 * 470296;
    s8 += s18 * 654183;
    s9 -= s18 * 997805;
    s10 += s18 * 136657;
    s11 -= s18 * 683901;

    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);

    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);

    s5 += s17 * 666643;
    s6 += s17 * 470296;
    s7 += s17 * 654183;
    s8 -= s17 * 997805;
    s9 += s17 * 136657;
    s10 -= s17 * 683901;

    s4 += s16 * 666643;
    s5 += s16 * 470296;
    s6 += s16 * 654183;
    s7 -= s16 * 997805;
    s8 += s16 * 136657;
    s9 -= s16 * 683901;

    s3 += s15 * 666643;
    s4 += s15 * 470296;
    s5 += s15 * 654183;
    s6 -= s15 * 997805;
    s7 += s15 * 136657;
    s8 -= s15 * 683901;

    s2 += s14 * 666643;
    s3 += s14 * 470296;
    s4 += s14 * 654183;
    s5 -= s14 * 997805;
    s6 += s14 * 136657;
    s7 -= s14 * 683901;

    s1 += s13 * 666643;
    s2 += s13 * 470296;
    s3 += s13 * 654183;
    s4 -= s13 * 997805;
    s5 += s13 * 136657;
    s6 -= s13 * 683901;

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry11 = s11 >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    s[0]  = s0 >> 0;
    s[1]  = s0 >> 8;
    s[2]  = (s0 >> 16) | (s1 * ((uint64_t) 1 << 5));
    s[3]  = s1 >> 3;
    s[4]  = s1 >> 11;
    s[5]  = (s1 >> 19) | (s2 * ((uint64_t) 1 << 2));
    s[6]  = s2 >> 6;
    s[7]  = (s2 >> 14) | (s3 * ((uint64_t) 1 << 7));
    s[8]  = s3 >> 1;
    s[9]  = s3 >> 9;
    s[10] = (s3 >> 17) | (s4 * ((uint64_t) 1 << 4));
    s[11] = s4 >> 4;
    s[12] = s4 >> 12;
    s[13] = (s4 >> 20) | (s5 * ((uint64_t) 1 << 1));
    s[14] = s5 >> 7;
    s[15] = (s5 >> 15) | (s6 * ((uint64_t) 1 << 6));
    s[16] = s6 >> 2;
    s[17] = s6 >> 10;
    s[18] = (s6 >> 18) | (s7 * ((uint64_t) 1 << 3));
    s[19] = s7 >> 5;
    s[20] = s7 >> 13;
    s[21] = s8 >> 0;
    s[22] = s8 >> 8;
    s[23] = (s8 >> 16) | (s9 * ((uint64_t) 1 << 5));
    s[24] = s9 >> 3;
    s[25] = s9 >> 11;
    s[26] = (s9 >> 19) | (s10 * ((uint64_t) 1 << 2));
    s[27] = s10 >> 6;
    s[28] = (s10 >> 14) | (s11 * ((uint64_t) 1 << 7));
    s[29] = s11 >> 1;
    s[30] = s11 >> 9;
    s[31] = s11 >> 17;
}

int
sc25519_is_canonical(const unsigned char s[32])
{
    /* 2^252+27742317777372353535851937790883648493 */
    static const unsigned char L[32] = {
        0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
        0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
    };
    unsigned char c = 0;
    unsigned char n = 1;
    unsigned int  i = 32;

    do {
        i--;
        c |= ((s[i] - L[i]) >> 8) & n;
        n &= ((s[i] ^ L[i]) - 1) >> 8;
    } while (i != 0);

    return (c != 0);
}

/* montgomery to edwards */
static void
ge25519_mont_to_ed(fe25519 xed, fe25519 yed, const fe25519 x, const fe25519 y)
{
    fe25519 one;
    fe25519 x_plus_one;
    fe25519 x_minus_one;
    fe25519 x_plus_one_y_inv;

    fe25519_1(one);
    fe25519_add(x_plus_one, x, one);
    fe25519_sub(x_minus_one, x, one);

    /* xed = sqrt(-A-2)*x/y */
    fe25519_mul(x_plus_one_y_inv, x_plus_one, y);
    fe25519_invert(x_plus_one_y_inv, x_plus_one_y_inv); /* 1/((x+1)*y) */
    fe25519_mul(xed, x, ed25519_sqrtam2);
    fe25519_mul(xed, xed, x_plus_one_y_inv);            /* sqrt(-A-2)*x/((x+1)*y) */
    fe25519_mul(xed, xed, x_plus_one);

    /* yed = (x-1)/(x+1) */
    fe25519_mul(yed, x_plus_one_y_inv, y);              /* 1/(x+1) */
    fe25519_mul(yed, yed, x_minus_one);
    fe25519_cmov(yed, one, fe25519_iszero(x_plus_one_y_inv));
}

/* montgomery -- recover y = sqrt(x^3 + A*x^2 + x) */
static int
ge25519_xmont_to_ymont(fe25519 y, const fe25519 x)
{
    fe25519 x2;
    fe25519 x3;

    fe25519_sq(x2, x);
    fe25519_mul(x3, x, x2);
    fe25519_mul32(x2, x2, ed25519_A_32);
    fe25519_add(y, x3, x);
    fe25519_add(y, y, x2);

    return fe25519_sqrt(y, y);
}

/* multiply by the cofactor */
static void
ge25519_clear_cofactor(ge25519_p3 *p3)
{
    ge25519_p1p1 p1;
    ge25519_p2   p2;

    ge25519_p3_dbl(&p1, p3);
    ge25519_p1p1_to_p2(&p2, &p1);
    ge25519_p2_dbl(&p1, &p2);
    ge25519_p1p1_to_p2(&p2, &p1);
    ge25519_p2_dbl(&p1, &p2);
    ge25519_p1p1_to_p3(p3, &p1);
}

static void
ge25519_elligator2(fe25519 x, fe25519 y, const fe25519 r, int *notsquare_p)
{
    fe25519       e;
    fe25519       gx1;
    fe25519       rr2;
    fe25519       x2, x3, negx;
    int           notsquare;

    fe25519_sq2(rr2, r);
    rr2[0]++;
    fe25519_invert(rr2, rr2);
    fe25519_mul32(x, rr2, ed25519_A_32);
    fe25519_neg(x, x); /* x=x1 */

    fe25519_sq(x2, x);
    fe25519_mul(x3, x, x2);
    fe25519_mul32(x2, x2, ed25519_A_32); /* x2 = A*x1^2 */
    fe25519_add(gx1, x3, x);
    fe25519_add(gx1, gx1, x2); /* gx1 = x1^3 + A*x1^2 + x1 */

    notsquare = fe25519_notsquare(gx1);

    /* gx1 not a square  => x = -x1-A */
    fe25519_neg(negx, x);
    fe25519_cmov(x, negx, notsquare);
    fe25519_0(x2);
    fe25519_cmov(x2, ed25519_A, notsquare);
    fe25519_sub(x, x, x2);

    /* y = sqrt(gx1) or sqrt(gx2) with gx2 = gx1 * (A+x1) / -x1 */
    /* but it is about as fast to just recompute from the curve equation. */
    if (ge25519_xmont_to_ymont(y, x) != 0) {
        abort();
    }
    *notsquare_p = notsquare;
}

void
ge25519_from_uniform(unsigned char s[32], const unsigned char r[32])
{
    ge25519_p3    p3;
    fe25519       x, y, negxed;
    fe25519       r_fe;
    int           notsquare;
    unsigned char x_sign;

    memcpy(s, r, 32);
    x_sign = s[31] >> 7;
    s[31] &= 0x7f;
    fe25519_frombytes(r_fe, s);

    ge25519_elligator2(x, y, r_fe, &notsquare);

    ge25519_mont_to_ed(p3.X, p3.Y, x, y);
    fe25519_neg(negxed, p3.X);
    fe25519_cmov(p3.X, negxed, fe25519_isnegative(p3.X) ^ x_sign);

    fe25519_1(p3.Z);
    fe25519_mul(p3.T, p3.X, p3.Y);
    ge25519_clear_cofactor(&p3);
    ge25519_p3_tobytes(s, &p3);
}

void
ge25519_from_hash(unsigned char s[32], const unsigned char h[64])
{
    unsigned char fl[32];
    unsigned char gl[32];
    ge25519_p3    p3;
    fe25519       x, y, negy;
    fe25519       fe_f;
    fe25519       fe_g;
    size_t        i;
    int           notsquare;
    unsigned char y_sign;

    for (i = 0; i < 32; i++) {
        fl[i] = h[63 - i];
        gl[i] = h[31 - i];
    }
    fl[31] &= 0x7f;
    gl[31] &= 0x7f;
    fe25519_frombytes(fe_f, fl);
    fe25519_frombytes(fe_g, gl);
    fe_f[0] += (h[32] >> 7) * 19;
    for (i = 0; i < sizeof (fe25519) / sizeof fe_f[0]; i++) {
        fe_f[i] += 38 * fe_g[i];
    }
    fe25519_reduce(fe_f, fe_f);

    ge25519_elligator2(x, y, fe_f, &notsquare);

    y_sign = notsquare;
    fe25519_neg(negy, y);
    fe25519_cmov(y, negy, fe25519_isnegative(y) ^ y_sign);

    ge25519_mont_to_ed(p3.X, p3.Y, x, y);

    fe25519_1(p3.Z);
    fe25519_mul(p3.T, p3.X, p3.Y);
    ge25519_clear_cofactor(&p3);
    ge25519_p3_tobytes(s, &p3);
}

/* Ristretto group */

static int
ristretto255_sqrt_ratio_m1(fe25519 x, const fe25519 u, const fe25519 v)
{
    fe25519 v3;
    fe25519 vxx;
    fe25519 m_root_check, p_root_check, f_root_check;
    fe25519 x_sqrtm1;
    int     has_m_root, has_p_root, has_f_root;

    fe25519_sq(v3, v);
    fe25519_mul(v3, v3, v); /* v3 = v^3 */
    fe25519_sq(x, v3);
    fe25519_mul(x, x, v);
    fe25519_mul(x, x, u); /* x = uv^7 */

    fe25519_pow22523(x, x); /* x = (uv^7)^((q-5)/8) */
    fe25519_mul(x, x, v3);
    fe25519_mul(x, x, u); /* x = uv^3(uv^7)^((q-5)/8) */

    fe25519_sq(vxx, x);
    fe25519_mul(vxx, vxx, v); /* vx^2 */
    fe25519_sub(m_root_check, vxx, u); /* vx^2-u */
    fe25519_add(p_root_check, vxx, u); /* vx^2+u */
    fe25519_mul(f_root_check, u, fe25519_sqrtm1); /* u*sqrt(-1) */
    fe25519_add(f_root_check, vxx, f_root_check); /* vx^2+u*sqrt(-1) */
    has_m_root = fe25519_iszero(m_root_check);
    has_p_root = fe25519_iszero(p_root_check);
    has_f_root = fe25519_iszero(f_root_check);
    fe25519_mul(x_sqrtm1, x, fe25519_sqrtm1); /* x*sqrt(-1) */

    fe25519_cmov(x, x_sqrtm1, has_p_root | has_f_root);
    fe25519_abs(x, x);

    return has_m_root | has_p_root;
}

static int
ristretto255_is_canonical(const unsigned char *s)
{
    unsigned char c;
    unsigned char d;
    unsigned char e;
    unsigned int  i;

    c = (s[31] & 0x7f) ^ 0x7f;
    for (i = 30; i > 0; i--) {
        c |= s[i] ^ 0xff;
    }
    c = (((unsigned int) c) - 1U) >> 8;
    d = (0xed - 1U - (unsigned int) s[0]) >> 8;
    e = s[31] >> 7;

    return 1 - (((c & d) | e | s[0]) & 1);
}

int
ristretto255_frombytes(ge25519_p3 *h, const unsigned char *s)
{
    fe25519 inv_sqrt;
    fe25519 one;
    fe25519 s_;
    fe25519 ss;
    fe25519 u1, u2;
    fe25519 u1u1, u2u2;
    fe25519 v;
    fe25519 v_u2u2;
    int     notsquare;

    if (ristretto255_is_canonical(s) == 0) {
        return -1;
    }
    fe25519_frombytes(s_, s);
    fe25519_sq(ss, s_);                /* ss = s^2 */

    fe25519_1(u1);
    fe25519_sub(u1, u1, ss);           /* u1 = 1-ss */
    fe25519_sq(u1u1, u1);              /* u1u1 = u1^2 */

    fe25519_1(u2);
    fe25519_add(u2, u2, ss);           /* u2 = 1+ss */
    fe25519_sq(u2u2, u2);              /* u2u2 = u2^2 */

    fe25519_mul(v, ed25519_d, u1u1);   /* v = d*u1^2 */
    fe25519_neg(v, v);                 /* v = -d*u1^2 */
    fe25519_sub(v, v, u2u2);           /* v = -(d*u1^2)-u2^2 */

    fe25519_mul(v_u2u2, v, u2u2);      /* v_u2u2 = v*u2^2 */

    fe25519_1(one);
    notsquare = ristretto255_sqrt_ratio_m1(inv_sqrt, one, v_u2u2);
    fe25519_mul(h->X, inv_sqrt, u2);
    fe25519_mul(h->Y, inv_sqrt, h->X);
    fe25519_mul(h->Y, h->Y, v);

    fe25519_mul(h->X, h->X, s_);
    fe25519_add(h->X, h->X, h->X);
    fe25519_abs(h->X, h->X);
    fe25519_mul(h->Y, u1, h->Y);
    fe25519_1(h->Z);
    fe25519_mul(h->T, h->X, h->Y);

    return - ((1 - notsquare) |
              fe25519_isnegative(h->T) | fe25519_iszero(h->Y));
}

void
ristretto255_p3_tobytes(unsigned char *s, const ge25519_p3 *h)
{
    fe25519 den1, den2;
    fe25519 den_inv;
    fe25519 eden;
    fe25519 inv_sqrt;
    fe25519 ix, iy;
    fe25519 one;
    fe25519 s_;
    fe25519 t_z_inv;
    fe25519 u1, u2;
    fe25519 u1_u2u2;
    fe25519 x_, y_;
    fe25519 x_z_inv;
    fe25519 z_inv;
    fe25519 zmy;
    int     rotate;

    fe25519_add(u1, h->Z, h->Y);       /* u1 = Z+Y */
    fe25519_sub(zmy, h->Z, h->Y);      /* zmy = Z-Y */
    fe25519_mul(u1, u1, zmy);          /* u1 = (Z+Y)*(Z-Y) */
    fe25519_mul(u2, h->X, h->Y);       /* u2 = X*Y */

    fe25519_sq(u1_u2u2, u2);           /* u1_u2u2 = u2^2 */
    fe25519_mul(u1_u2u2, u1, u1_u2u2); /* u1_u2u2 = u1*u2^2 */

    fe25519_1(one);
    (void) ristretto255_sqrt_ratio_m1(inv_sqrt, one, u1_u2u2);
    fe25519_mul(den1, inv_sqrt, u1);   /* den1 = inv_sqrt*u1 */
    fe25519_mul(den2, inv_sqrt, u2);   /* den2 = inv_sqrt*u2 */
    fe25519_mul(z_inv, den1, den2);    /* z_inv = den1*den2 */
    fe25519_mul(z_inv, z_inv, h->T);   /* z_inv = den1*den2*T */

    fe25519_mul(ix, h->X, fe25519_sqrtm1);       /* ix = X*sqrt(-1) */
    fe25519_mul(iy, h->Y, fe25519_sqrtm1);       /* iy = Y*sqrt(-1) */
    fe25519_mul(eden, den1, ed25519_invsqrtamd); /* eden = den1*sqrt(a-d) */

    fe25519_mul(t_z_inv, h->T, z_inv); /* t_z_inv = T*z_inv */
    rotate = fe25519_isnegative(t_z_inv);

    fe25519_copy(x_, h->X);
    fe25519_copy(y_, h->Y);
    fe25519_copy(den_inv, den2);

    fe25519_cmov(x_, iy, rotate);
    fe25519_cmov(y_, ix, rotate);
    fe25519_cmov(den_inv, eden, rotate);

    fe25519_mul(x_z_inv, x_, z_inv);
    fe25519_cneg(y_, y_, fe25519_isnegative(x_z_inv));

    fe25519_sub(s_, h->Z, y_);
    fe25519_mul(s_, den_inv, s_);
    fe25519_abs(s_, s_);
    fe25519_tobytes(s, s_);
}

static void
ristretto255_elligator(ge25519_p3 *p, const fe25519 t)
{
    fe25519 c;
    fe25519 n;
    fe25519 one;
    fe25519 r;
    fe25519 rpd;
    fe25519 s, s_prime;
    fe25519 ss;
    fe25519 u, v;
    fe25519 w0, w1, w2, w3;
    int     wasnt_square;

    fe25519_1(one);
    fe25519_sq(r, t);                  /* r = t^2 */
    fe25519_mul(r, fe25519_sqrtm1, r); /* r = sqrt(-1)*t^2 */
    fe25519_add(u, r, one);            /* u = r+1 */
    fe25519_mul(u, u, ed25519_onemsqd);/* u = (r+1)*(1-d^2) */
    fe25519_1(c);
    fe25519_neg(c, c);                 /* c = -1 */
    fe25519_add(rpd, r, ed25519_d);    /* rpd = r*d */
    fe25519_mul(v, r, ed25519_d);      /* v = r*d */
    fe25519_sub(v, c, v);              /* v = c-r*d */
    fe25519_mul(v, v, rpd);            /* v = (c-r*d)*(r+d) */

    wasnt_square = 1 - ristretto255_sqrt_ratio_m1(s, u, v);
    fe25519_mul(s_prime, s, t);
    fe25519_abs(s_prime, s_prime);
    fe25519_neg(s_prime, s_prime);     /* s_prime = -|s*t| */
    fe25519_cmov(s, s_prime, wasnt_square);
    fe25519_cmov(c, r, wasnt_square);

    fe25519_sub(n, r, one);            /* n = r-1 */
    fe25519_mul(n, n, c);              /* n = c*(r-1) */
    fe25519_mul(n, n, ed25519_sqdmone); /* n = c*(r-1)*(d-1)^2 */
    fe25519_sub(n, n, v);              /* n =  c*(r-1)*(d-1)^2-v */

    fe25519_add(w0, s, s);             /* w0 = 2s */
    fe25519_mul(w0, w0, v);            /* w0 = 2s*v */
    fe25519_mul(w1, n, ed25519_sqrtadm1); /* w1 = n*sqrt(ad-1) */
    fe25519_sq(ss, s);                 /* ss = s^2 */
    fe25519_sub(w2, one, ss);          /* w2 = 1-s^2 */
    fe25519_add(w3, one, ss);          /* w3 = 1+s^2 */

    fe25519_mul(p->X, w0, w3);
    fe25519_mul(p->Y, w2, w1);
    fe25519_mul(p->Z, w1, w3);
    fe25519_mul(p->T, w0, w2);
}

void
ristretto255_from_hash(unsigned char s[32], const unsigned char h[64])
{
    fe25519        r0, r1;
    ge25519_cached p1_cached;
    ge25519_p1p1   p_p1p1;
    ge25519_p3     p0, p1;
    ge25519_p3     p;

    fe25519_frombytes(r0, h);
    fe25519_frombytes(r1, h + 32);
    ristretto255_elligator(&p0, r0);
    ristretto255_elligator(&p1, r1);
    ge25519_p3_to_cached(&p1_cached, &p1);
    ge25519_add_cached(&p_p1p1, &p0, &p1_cached);
    ge25519_p1p1_to_p3(&p, &p_p1p1);
    ristretto255_p3_tobytes(s, &p);
}
