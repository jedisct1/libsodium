#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "crypto_verify_32.h"
#include "private/common.h"
#include "private/curve25519_ref10.h"
#include "utils.h"

#ifdef HAVE_TI_MODE
# include "fe_51/constants.h"
#else
# include "fe_25_5/constants.h"
#endif

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

#ifdef HAVE_TI_MODE
# include "fe_51/fe.h"
#else
# include "fe_25_5/fe.h"
#endif

void
fe_invert(fe out, const fe z)
{
    fe  t0;
    fe  t1;
    fe  t2;
    fe  t3;
    int i;

    fe_sq(t0, z);
    fe_sq(t1, t0);
    fe_sq(t1, t1);
    fe_mul(t1, z, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t2, t0);
    fe_mul(t1, t1, t2);
    fe_sq(t2, t1);
    for (i = 1; i < 5; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t2, t1);
    for (i = 1; i < 10; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t2, t2, t1);
    fe_sq(t3, t2);
    for (i = 1; i < 20; ++i) {
        fe_sq(t3, t3);
    }
    fe_mul(t2, t3, t2);
    fe_sq(t2, t2);
    for (i = 1; i < 10; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t2, t1);
    for (i = 1; i < 50; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t2, t2, t1);
    fe_sq(t3, t2);
    for (i = 1; i < 100; ++i) {
        fe_sq(t3, t3);
    }
    fe_mul(t2, t3, t2);
    fe_sq(t2, t2);
    for (i = 1; i < 50; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t1, t1);
    for (i = 1; i < 5; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(out, t1, t0);
}

static void
fe_pow22523(fe out, const fe z)
{
    fe  t0;
    fe  t1;
    fe  t2;
    int i;

    fe_sq(t0, z);
    fe_sq(t1, t0);
    fe_sq(t1, t1);
    fe_mul(t1, z, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t0, t0);
    fe_mul(t0, t1, t0);
    fe_sq(t1, t0);
    for (i = 1; i < 5; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t0, t1, t0);
    fe_sq(t1, t0);
    for (i = 1; i < 10; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t1, t1, t0);
    fe_sq(t2, t1);
    for (i = 1; i < 20; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t1, t1);
    for (i = 1; i < 10; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t0, t1, t0);
    fe_sq(t1, t0);
    for (i = 1; i < 50; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t1, t1, t0);
    fe_sq(t2, t1);
    for (i = 1; i < 100; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t1, t1);
    for (i = 1; i < 50; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t0, t1, t0);
    fe_sq(t0, t0);
    fe_sq(t0, t0);
    fe_mul(out, t0, z);
}

/*
 r = p + q
 */

void
ge_add(ge_p1p1 *r, const ge_p3 *p, const ge_cached *q)
{
    fe t0;

    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->YplusX);
    fe_mul(r->Y, r->Y, q->YminusX);
    fe_mul(r->T, q->T2d, p->T);
    fe_mul(r->X, p->Z, q->Z);
    fe_add(t0, r->X, r->X);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_add(r->Z, t0, r->T);
    fe_sub(r->T, t0, r->T);
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
        if (r[i]) {
            for (b = 1; b <= 6 && i + b < 256; ++b) {
                if (r[i + b]) {
                    ribs = r[i + b] << b;
                    cmp = r[i] + ribs;
                    if (cmp <= 15) {
                        r[i] = cmp;
                        r[i + b] = 0;
                    } else {
                        cmp = r[i] - ribs;
                        if (cmp >= -15) {
                            r[i] = cmp;
                            for (k = i + b; k < 256; ++k) {
                                if (!r[k]) {
                                    r[k] = 1;
                                    break;
                                }
                                r[k] = 0;
                            }
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }
}

int
ge_frombytes(ge_p3 *h, const unsigned char *s)
{
    fe  u;
    fe  v;
    fe  v3;
    fe  vxx;
    fe  m_root_check, p_root_check;
    fe  negx;
    fe  x_sqrtm1;
    int has_m_root, has_p_root;

    fe_frombytes(h->Y, s);
    fe_1(h->Z);
    fe_sq(u, h->Y);
    fe_mul(v, u, d);
    fe_sub(u, u, h->Z); /* u = y^2-1 */
    fe_add(v, v, h->Z); /* v = dy^2+1 */

    fe_sq(v3, v);
    fe_mul(v3, v3, v); /* v3 = v^3 */
    fe_sq(h->X, v3);
    fe_mul(h->X, h->X, v);
    fe_mul(h->X, h->X, u); /* x = uv^7 */

    fe_pow22523(h->X, h->X); /* x = (uv^7)^((q-5)/8) */
    fe_mul(h->X, h->X, v3);
    fe_mul(h->X, h->X, u); /* x = uv^3(uv^7)^((q-5)/8) */

    fe_sq(vxx, h->X);
    fe_mul(vxx, vxx, v);
    fe_sub(m_root_check, vxx, u); /* vx^2-u */
    fe_add(p_root_check, vxx, u); /* vx^2+u */
    has_m_root = fe_iszero(m_root_check);
    has_p_root = fe_iszero(p_root_check);
    fe_mul(x_sqrtm1, h->X, sqrtm1); /* x*sqrt(-1) */
    fe_cmov(h->X, x_sqrtm1, 1 - has_m_root);

    fe_neg(negx, h->X);
    fe_cmov(h->X, negx, fe_isnegative(h->X) ^ (s[31] >> 7));
    fe_mul(h->T, h->X, h->Y);

    return (has_m_root | has_p_root) - 1;
}

int
ge_frombytes_negate_vartime(ge_p3 *h, const unsigned char *s)
{
    fe u;
    fe v;
    fe v3;
    fe vxx;
    fe m_root_check, p_root_check;

    fe_frombytes(h->Y, s);
    fe_1(h->Z);
    fe_sq(u, h->Y);
    fe_mul(v, u, d);
    fe_sub(u, u, h->Z); /* u = y^2-1 */
    fe_add(v, v, h->Z); /* v = dy^2+1 */

    fe_sq(v3, v);
    fe_mul(v3, v3, v); /* v3 = v^3 */
    fe_sq(h->X, v3);
    fe_mul(h->X, h->X, v);
    fe_mul(h->X, h->X, u); /* x = uv^7 */

    fe_pow22523(h->X, h->X); /* x = (uv^7)^((q-5)/8) */
    fe_mul(h->X, h->X, v3);
    fe_mul(h->X, h->X, u); /* x = uv^3(uv^7)^((q-5)/8) */

    fe_sq(vxx, h->X);
    fe_mul(vxx, vxx, v);
    fe_sub(m_root_check, vxx, u); /* vx^2-u */
    if (fe_iszero(m_root_check) == 0) {
        fe_add(p_root_check, vxx, u); /* vx^2+u */
        if (fe_iszero(p_root_check) == 0) {
            return -1;
        }
        fe_mul(h->X, h->X, sqrtm1);
    }

    if (fe_isnegative(h->X) == (s[31] >> 7)) {
        fe_neg(h->X, h->X);
    }
    fe_mul(h->T, h->X, h->Y);

    return 0;
}

/*
 r = p + q
 */

static void
ge_madd(ge_p1p1 *r, const ge_p3 *p, const ge_precomp *q)
{
    fe t0;

    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->yplusx);
    fe_mul(r->Y, r->Y, q->yminusx);
    fe_mul(r->T, q->xy2d, p->T);
    fe_add(t0, p->Z, p->Z);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_add(r->Z, t0, r->T);
    fe_sub(r->T, t0, r->T);
}

/*
 r = p - q
 */

static void
ge_msub(ge_p1p1 *r, const ge_p3 *p, const ge_precomp *q)
{
    fe t0;

    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->yminusx);
    fe_mul(r->Y, r->Y, q->yplusx);
    fe_mul(r->T, q->xy2d, p->T);
    fe_add(t0, p->Z, p->Z);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_sub(r->Z, t0, r->T);
    fe_add(r->T, t0, r->T);
}

/*
 r = p
 */

void
ge_p1p1_to_p2(ge_p2 *r, const ge_p1p1 *p)
{
    fe_mul(r->X, p->X, p->T);
    fe_mul(r->Y, p->Y, p->Z);
    fe_mul(r->Z, p->Z, p->T);
}

/*
 r = p
 */

static void
ge_p1p1_to_p3(ge_p3 *r, const ge_p1p1 *p)
{
    fe_mul(r->X, p->X, p->T);
    fe_mul(r->Y, p->Y, p->Z);
    fe_mul(r->Z, p->Z, p->T);
    fe_mul(r->T, p->X, p->Y);
}

static void
ge_p2_0(ge_p2 *h)
{
    fe_0(h->X);
    fe_1(h->Y);
    fe_1(h->Z);
}

/*
 r = 2 * p
 */

static void
ge_p2_dbl(ge_p1p1 *r, const ge_p2 *p)
{
    fe t0;

    fe_sq(r->X, p->X);
    fe_sq(r->Z, p->Y);
    fe_sq2(r->T, p->Z);
    fe_add(r->Y, p->X, p->Y);
    fe_sq(t0, r->Y);
    fe_add(r->Y, r->Z, r->X);
    fe_sub(r->Z, r->Z, r->X);
    fe_sub(r->X, t0, r->Y);
    fe_sub(r->T, r->T, r->Z);
}

static void
ge_p3_0(ge_p3 *h)
{
    fe_0(h->X);
    fe_1(h->Y);
    fe_1(h->Z);
    fe_0(h->T);
}

/*
 r = p
 */

void
ge_p3_to_cached(ge_cached *r, const ge_p3 *p)
{
    fe_add(r->YplusX, p->Y, p->X);
    fe_sub(r->YminusX, p->Y, p->X);
    fe_copy(r->Z, p->Z);
    fe_mul(r->T2d, p->T, d2);
}

static void
ge_p3_to_precomp(ge_precomp *pi, const ge_p3 *p)
{
    fe recip;
    fe x;
    fe y;
    fe xy;

    fe_invert(recip, p->Z);
    fe_mul(x, p->X, recip);
    fe_mul(y, p->Y, recip);
    fe_add(pi->yplusx, y, x);
    fe_sub(pi->yminusx, y, x);
    fe_mul(xy, x, y);
    fe_mul(pi->xy2d, xy, d2);
}

/*
 r = p
 */

static void
ge_p3_to_p2(ge_p2 *r, const ge_p3 *p)
{
    fe_copy(r->X, p->X);
    fe_copy(r->Y, p->Y);
    fe_copy(r->Z, p->Z);
}

void
ge_p3_tobytes(unsigned char *s, const ge_p3 *h)
{
    fe recip;
    fe x;
    fe y;

    fe_invert(recip, h->Z);
    fe_mul(x, h->X, recip);
    fe_mul(y, h->Y, recip);
    fe_tobytes(s, y);
    s[31] ^= fe_isnegative(x) << 7;
}

/*
 r = 2 * p
 */

static void
ge_p3_dbl(ge_p1p1 *r, const ge_p3 *p)
{
    ge_p2 q;
    ge_p3_to_p2(&q, p);
    ge_p2_dbl(r, &q);
}

static void
ge_precomp_0(ge_precomp *h)
{
    fe_1(h->yplusx);
    fe_1(h->yminusx);
    fe_0(h->xy2d);
}

static unsigned char
equal(signed char b, signed char c)
{
    unsigned char ub = b;
    unsigned char uc = c;
    unsigned char x  = ub ^ uc; /* 0: yes; 1..255: no */
    uint32_t      y  = x;       /* 0: yes; 1..255: no */

    y -= 1;   /* 4294967295: yes; 0..254: no */
    y >>= 31; /* 1: yes; 0: no */

    return y;
}

static unsigned char
negative(signed char b)
{
    uint64_t x =
        b; /* 18446744073709551361..18446744073709551615: yes; 0..255: no */

    x >>= 63; /* 1: yes; 0: no */

    return x;
}

static void
ge_cmov(ge_precomp *t, const ge_precomp *u, unsigned char b)
{
    fe_cmov(t->yplusx, u->yplusx, b);
    fe_cmov(t->yminusx, u->yminusx, b);
    fe_cmov(t->xy2d, u->xy2d, b);
}

static void
ge_select(ge_precomp *t, const ge_precomp precomp[8], const signed char b)
{
    ge_precomp          minust;
    const unsigned char bnegative = negative(b);
    const unsigned char babs      = b - (((-bnegative) & b) * ((signed char) 1 << 1));

    ge_precomp_0(t);
    ge_cmov(t, &precomp[0], equal(babs, 1));
    ge_cmov(t, &precomp[1], equal(babs, 2));
    ge_cmov(t, &precomp[2], equal(babs, 3));
    ge_cmov(t, &precomp[3], equal(babs, 4));
    ge_cmov(t, &precomp[4], equal(babs, 5));
    ge_cmov(t, &precomp[5], equal(babs, 6));
    ge_cmov(t, &precomp[6], equal(babs, 7));
    ge_cmov(t, &precomp[7], equal(babs, 8));
    fe_copy(minust.yplusx, t->yminusx);
    fe_copy(minust.yminusx, t->yplusx);
    fe_neg(minust.xy2d, t->xy2d);
    ge_cmov(t, &minust, bnegative);
}

static void
ge_select_base(ge_precomp *t, const int pos, const signed char b)
{
    static const ge_precomp base[32][8] = { /* base[i][j] = (j+1)*256^i*B */
#ifdef HAVE_TI_MODE
# include "fe_51/base.h"
#else
# include "fe_25_5/base.h"
#endif
    };
    ge_select(t, base[pos], b);
}

/*
 r = p - q
 */

static void
ge_sub(ge_p1p1 *r, const ge_p3 *p, const ge_cached *q)
{
    fe t0;

    fe_add(r->X, p->Y, p->X);
    fe_sub(r->Y, p->Y, p->X);
    fe_mul(r->Z, r->X, q->YminusX);
    fe_mul(r->Y, r->Y, q->YplusX);
    fe_mul(r->T, q->T2d, p->T);
    fe_mul(r->X, p->Z, q->Z);
    fe_add(t0, r->X, r->X);
    fe_sub(r->X, r->Z, r->Y);
    fe_add(r->Y, r->Z, r->Y);
    fe_sub(r->Z, t0, r->T);
    fe_add(r->T, t0, r->T);
}

void
ge_tobytes(unsigned char *s, const ge_p2 *h)
{
    fe recip;
    fe x;
    fe y;

    fe_invert(recip, h->Z);
    fe_mul(x, h->X, recip);
    fe_mul(y, h->Y, recip);
    fe_tobytes(s, y);
    s[31] ^= fe_isnegative(x) << 7;
}

/*
 r = a * A + b * B
 where a = a[0]+256*a[1]+...+256^31 a[31].
 and b = b[0]+256*b[1]+...+256^31 b[31].
 B is the Ed25519 base point (x,4/5) with x positive.

 Only used for signatures verification.
 */

void
ge_double_scalarmult_vartime(ge_p2 *r, const unsigned char *a, const ge_p3 *A,
                             const unsigned char *b)
{
    static const ge_precomp Bi[8] = {
#ifdef HAVE_TI_MODE
# include "fe_51/base2.h"
#else
# include "fe_25_5/base2.h"
#endif
    };
    signed char aslide[256];
    signed char bslide[256];
    ge_cached   Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */
    ge_p1p1     t;
    ge_p3       u;
    ge_p3       A2;
    int         i;

    slide_vartime(aslide, a);
    slide_vartime(bslide, b);

    ge_p3_to_cached(&Ai[0], A);
    ge_p3_dbl(&t, A);
    ge_p1p1_to_p3(&A2, &t);
    ge_add(&t, &A2, &Ai[0]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[1], &u);
    ge_add(&t, &A2, &Ai[1]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[2], &u);
    ge_add(&t, &A2, &Ai[2]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[3], &u);
    ge_add(&t, &A2, &Ai[3]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[4], &u);
    ge_add(&t, &A2, &Ai[4]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[5], &u);
    ge_add(&t, &A2, &Ai[5]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[6], &u);
    ge_add(&t, &A2, &Ai[6]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[7], &u);

    ge_p2_0(r);

    for (i = 255; i >= 0; --i) {
        if (aslide[i] || bslide[i]) {
            break;
        }
    }

    for (; i >= 0; --i) {
        ge_p2_dbl(&t, r);

        if (aslide[i] > 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_add(&t, &u, &Ai[aslide[i] / 2]);
        } else if (aslide[i] < 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_sub(&t, &u, &Ai[(-aslide[i]) / 2]);
        }

        if (bslide[i] > 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_madd(&t, &u, &Bi[bslide[i] / 2]);
        } else if (bslide[i] < 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_msub(&t, &u, &Bi[(-bslide[i]) / 2]);
        }

        ge_p1p1_to_p2(r, &t);
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
ge_scalarmult(ge_p3 *h, const unsigned char *a, const ge_p3 *p)
{
    signed char e[64];
    signed char carry;
    ge_p1p1     r;
    ge_p2       s;
    ge_p1p1     t2, t3, t4, t5, t6, t7, t8;
    ge_p3       p2, p3, p4, p5, p6, p7, p8;
    ge_precomp  pi[8];
    ge_precomp  t;
    int         i;

    ge_p3_to_precomp(&pi[1 - 1], p);   /* p */

    ge_p3_dbl(&t2, p);
    ge_p1p1_to_p3(&p2, &t2);
    ge_p3_to_precomp(&pi[2 - 1], &p2); /* 2p = 2*p */

    ge_madd(&t3, p, &pi[2 - 1]);
    ge_p1p1_to_p3(&p3, &t3);
    ge_p3_to_precomp(&pi[3 - 1], &p3); /* 3p = 2p+p */

    ge_p3_dbl(&t4, &p2);
    ge_p1p1_to_p3(&p4, &t4);
    ge_p3_to_precomp(&pi[4 - 1], &p4); /* 4p = 2*2p */

    ge_madd(&t5, p, &pi[4 - 1]);
    ge_p1p1_to_p3(&p5, &t5);
    ge_p3_to_precomp(&pi[5 - 1], &p5); /* 5p = 4p+p */

    ge_p3_dbl(&t6, &p3);
    ge_p1p1_to_p3(&p6, &t6);
    ge_p3_to_precomp(&pi[6 - 1], &p6); /* 6p = 2*3p */

    ge_madd(&t7, p, &pi[6 - 1]);
    ge_p1p1_to_p3(&p7, &t7);
    ge_p3_to_precomp(&pi[7 - 1], &p7); /* 7p = 6p+p */

    ge_p3_dbl(&t8, &p4);
    ge_p1p1_to_p3(&p8, &t8);
    ge_p3_to_precomp(&pi[8 - 1], &p8); /* 8p = 2*4p */

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

    ge_p3_0(h);

    for (i = 63; i != 0; i--) {
        ge_select(&t, pi, e[i]);
        ge_madd(&r, h, &t);

        ge_p1p1_to_p2(&s, &r);
        ge_p2_dbl(&r, &s);
        ge_p1p1_to_p2(&s, &r);
        ge_p2_dbl(&r, &s);
        ge_p1p1_to_p2(&s, &r);
        ge_p2_dbl(&r, &s);
        ge_p1p1_to_p2(&s, &r);
        ge_p2_dbl(&r, &s);

        ge_p1p1_to_p3(h, &r);  /* *16 */
    }
    ge_select(&t, pi, e[i]);
    ge_madd(&r, h, &t);

    ge_p1p1_to_p3(h, &r);
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
ge_scalarmult_base(ge_p3 *h, const unsigned char *a)
{
    signed char e[64];
    signed char carry;
    ge_p1p1     r;
    ge_p2       s;
    ge_precomp  t;
    int         i;

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

    ge_p3_0(h);
    for (i = 1; i < 64; i += 2) {
        ge_select_base(&t, i / 2, e[i]);
        ge_madd(&r, h, &t);
        ge_p1p1_to_p3(h, &r);
    }

    ge_p3_dbl(&r, h);
    ge_p1p1_to_p2(&s, &r);
    ge_p2_dbl(&r, &s);
    ge_p1p1_to_p2(&s, &r);
    ge_p2_dbl(&r, &s);
    ge_p1p1_to_p2(&s, &r);
    ge_p2_dbl(&r, &s);
    ge_p1p1_to_p3(h, &r);

    for (i = 0; i < 64; i += 2) {
        ge_select_base(&t, i / 2, e[i]);
        ge_madd(&r, h, &t);
        ge_p1p1_to_p3(h, &r);
    }
}

/* multiply by the order of the main subgroup l = 2^252+27742317777372353535851937790883648493 */
static void
ge_mul_l(ge_p3 *r, const ge_p3 *A)
{
    static const signed char aslide[253] = {
        13, 0, 0, 0, 0, -1, 0, 0, 0, 0, -11, 0, 0, 0, 0, 0, 0, -5, 0, 0, 0, 0, 0, 0, -3, 0, 0, 0, 0, -13, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, -13, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, -13, 0, 0, 0, 0, 0, 0, -3, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 3, 0, 0, 0, 0, -11, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
    };
    ge_cached Ai[8];
    ge_p1p1   t;
    ge_p3     u;
    ge_p3     A2;
    int       i;

    ge_p3_to_cached(&Ai[0], A);
    ge_p3_dbl(&t, A);
    ge_p1p1_to_p3(&A2, &t);
    ge_add(&t, &A2, &Ai[0]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[1], &u);
    ge_add(&t, &A2, &Ai[1]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[2], &u);
    ge_add(&t, &A2, &Ai[2]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[3], &u);
    ge_add(&t, &A2, &Ai[3]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[4], &u);
    ge_add(&t, &A2, &Ai[4]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[5], &u);
    ge_add(&t, &A2, &Ai[5]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[6], &u);
    ge_add(&t, &A2, &Ai[6]);
    ge_p1p1_to_p3(&u, &t);
    ge_p3_to_cached(&Ai[7], &u);

    ge_p3_0(r);

    for (i = 252; i >= 0; --i) {
        ge_p3_dbl(&t, r);

        if (aslide[i] > 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_add(&t, &u, &Ai[aslide[i] / 2]);
        } else if (aslide[i] < 0) {
            ge_p1p1_to_p3(&u, &t);
            ge_sub(&t, &u, &Ai[(-aslide[i]) / 2]);
        }

        ge_p1p1_to_p3(r, &t);
    }
}

int
ge_is_on_curve(const ge_p3 *p)
{
    fe x2;
    fe y2;
    fe z2;
    fe z4;
    fe t0;
    fe t1;

    fe_sq(x2, p->X);
    fe_sq(y2, p->Y);
    fe_sq(z2, p->Z);
    fe_sub(t0, y2, x2);
    fe_mul(t0, t0, z2);

    fe_mul(t1, x2, y2);
    fe_mul(t1, t1, d);
    fe_sq(z4, z2);
    fe_add(t1, t1, z4);
    fe_sub(t0, t0, t1);

    return fe_iszero(t0);
}

int
ge_is_on_main_subgroup(const ge_p3 *p)
{
    ge_p3 pl;

    ge_mul_l(&pl, p);

    return fe_iszero(pl.X);
}

int
ge_is_canonical(const unsigned char *s)
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
ge_has_small_order(const unsigned char s[32])
{
    CRYPTO_ALIGN(16)
    static const unsigned char blacklist[][32] = {
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
    size_t        i, j;
    unsigned char c;

    for (i = 0; i < sizeof blacklist / sizeof blacklist[0]; i++) {
        c = 0;
        for (j = 0; j < 31; j++) {
            c |= s[j] ^ blacklist[i][j];
        }
        c |= (s[j] & 0x7f) ^ blacklist[i][j];
        if (c == 0) {
            return 1;
        }
    }
    return 0;
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
sc_muladd(unsigned char *s, const unsigned char *a, const unsigned char *b,
          const unsigned char *c)
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

    carry0 = (s0 + (int64_t)(1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t)(1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t)(1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t)(1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t)(1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t)(1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t)(1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t)(1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t)(1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);
    carry18 = (s18 + (int64_t)(1L << 20)) >> 21;
    s19 += carry18;
    s18 -= carry18 * ((uint64_t) 1L << 21);
    carry20 = (s20 + (int64_t)(1L << 20)) >> 21;
    s21 += carry20;
    s20 -= carry20 * ((uint64_t) 1L << 21);
    carry22 = (s22 + (int64_t)(1L << 20)) >> 21;
    s23 += carry22;
    s22 -= carry22 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t)(1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t)(1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t)(1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t)(1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t)(1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t)(1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t)(1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t)(1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);
    carry17 = (s17 + (int64_t)(1L << 20)) >> 21;
    s18 += carry17;
    s17 -= carry17 * ((uint64_t) 1L << 21);
    carry19 = (s19 + (int64_t)(1L << 20)) >> 21;
    s20 += carry19;
    s19 -= carry19 * ((uint64_t) 1L << 21);
    carry21 = (s21 + (int64_t)(1L << 20)) >> 21;
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

    carry6 = (s6 + (int64_t)(1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t)(1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t)(1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t)(1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t)(1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t)(1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);

    carry7 = (s7 + (int64_t)(1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t)(1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t)(1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t)(1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t)(1L << 20)) >> 21;
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

    carry0 = (s0 + (int64_t)(1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t)(1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t)(1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t)(1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t)(1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t)(1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t)(1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t)(1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t)(1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t)(1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t)(1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t)(1L << 20)) >> 21;
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
 s[0]+256*s[1]+...+256^63*s[63] = s
 *
 Output:
 s[0]+256*s[1]+...+256^31*s[31] = s mod l
 where l = 2^252 + 27742317777372353535851937790883648493.
 Overwrites s in place.
 */

void
sc_reduce(unsigned char *s)
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

    carry6 = (s6 + (int64_t)(1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t)(1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t)(1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t)(1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t)(1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t)(1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);

    carry7 = (s7 + (int64_t)(1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t)(1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t)(1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t)(1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t)(1L << 20)) >> 21;
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

    carry0 = (s0 + (int64_t)(1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t)(1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t)(1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t)(1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t)(1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t)(1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t)(1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t)(1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t)(1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t)(1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t)(1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t)(1L << 20)) >> 21;
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
sc_is_canonical(const unsigned char *s)
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
