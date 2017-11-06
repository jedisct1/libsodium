/* Copyright 2008, Google Inc.
 * All rights reserved.
 *
 * Code released into the public domain.
 *
 * curve25519-donna: Curve25519 elliptic curve, public key function
 *
 * http://code.google.com/p/curve25519-donna/
 *
 * Adam Langley <agl@imperialviolet.org>
 * Parts optimised by floodyberry
 * Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>
 *
 * More information about curve25519 can be found here
 *   http://cr.yp.to/ecdh.html
 *
 * djb's sample implementation of curve25519 is written in a special assembly
 * language called qhasm and uses the floating point registers.
 *
 * This is, almost, a clean room reimplementation from the curve25519 paper. It
 * uses many of the tricks described therein. Only the crecip function is taken
 * from the sample implementation.
 */

#include <stdint.h>
#include <string.h>

#ifdef HAVE_TI_MODE

#include "../scalarmult_curve25519.h"
#include "curve25519_donna_c64.h"
#include "private/curve25519_ref10.h"
#include "private/common.h"
#include "utils.h"

/* Sum two numbers: output += in */
static inline void
fe_add_self(fe output, const fe in)
{
    output[0] += in[0];
    output[1] += in[1];
    output[2] += in[2];
    output[3] += in[3];
    output[4] += in[4];
}

/* Find the difference of two numbers: output = in - output
 * (note the order of the arguments!)
 *
 * Assumes that out[i] < 2**52
 * On return, out[i] < 2**55
 */
static inline void
fe_sub_backwards(fe out, const fe in)
{
    /* 152 is 19 << 3 */
    static const uint64_t two54m152 = (((uint64_t) 1) << 54) - 152;
    static const uint64_t two54m8 = (((uint64_t) 1) << 54) - 8;

    out[0] = in[0] + two54m152 - out[0];
    out[1] = in[1] + two54m8   - out[1];
    out[2] = in[2] + two54m8   - out[2];
    out[3] = in[3] + two54m8   - out[3];
    out[4] = in[4] + two54m8   - out[4];
}

/* Multiply two numbers: output = in2 * in
 *
 * output must be distinct to both inputs. The inputs are reduced coefficient
 * form, the output is not.
 *
 * Assumes that in[i] < 2**55 and likewise for in2.
 * On return, output[i] < 2**52
 */
static inline void
fe_mul_restrict(fe output, const fe in2, const fe in)
{
    const uint64_t mask = 0x7ffffffffffffULL;
    uint128_t t[5];
    uint64_t  r0, r1, r2, r3, r4, s0, s1, s2, s3, s4, c;

    r0 = in[0];
    r1 = in[1];
    r2 = in[2];
    r3 = in[3];
    r4 = in[4];

    s0 = in2[0];
    s1 = in2[1];
    s2 = in2[2];
    s3 = in2[3];
    s4 = in2[4];

    t[0] = ((uint128_t) r0) * s0;
    t[1] = ((uint128_t) r0) * s1 + ((uint128_t) r1) * s0;
    t[2] = ((uint128_t) r0) * s2 + ((uint128_t) r2) * s0 + ((uint128_t) r1) * s1;
    t[3] = ((uint128_t) r0) * s3 + ((uint128_t) r3) * s0 + ((uint128_t) r1) * s2
           + ((uint128_t) r2) * s1;
    t[4] = ((uint128_t) r0) * s4 + ((uint128_t) r4) * s0 + ((uint128_t) r3) * s1
           + ((uint128_t) r1) * s3 + ((uint128_t) r2) * s2;

    r4 *= 19ULL;
    r1 *= 19ULL;
    r2 *= 19ULL;
    r3 *= 19ULL;

    t[0] += ((uint128_t) r4) * s1 + ((uint128_t) r1) * s4 + ((uint128_t) r2) * s3
            + ((uint128_t) r3) * s2;
    t[1] += ((uint128_t) r4) * s2 + ((uint128_t) r2) * s4 + ((uint128_t) r3) * s3;
    t[2] += ((uint128_t) r4) * s3 + ((uint128_t) r3) * s4;
    t[3] += ((uint128_t) r4) * s4;

    r0 = (uint64_t) t[0] & mask;
    c = (uint64_t) (t[0] >> 51);
    t[1] += c;
    r1 = (uint64_t) t[1] & mask;
    c = (uint64_t) (t[1] >> 51);
    t[2] += c;
    r2 = (uint64_t) t[2] & mask;
    c = (uint64_t) (t[2] >> 51);
    t[3] += c;
    r3 = (uint64_t) t[3] & mask;
    c = (uint64_t) (t[3] >> 51);
    t[4] += c;
    r4 = (uint64_t) t[4] & mask;
    c = (uint64_t) (t[4] >> 51);
    r0 += c * 19ULL;
    c = r0 >> 51;
    r0 = r0 & mask;
    r1 += c;
    c = r1 >> 51;
    r1 = r1 & mask;
    r2 += c;

    output[0] = r0;
    output[1] = r1;
    output[2] = r2;
    output[3] = r3;
    output[4] = r4;
}

static inline void
fe_square_times(fe output, const fe in, uint64_t count)
{
    const uint64_t mask = 0x7ffffffffffffULL;
    uint128_t t[5];
    uint64_t  r0, r1, r2, r3, r4, c;
    uint64_t  d0, d1, d2, d4, d419;

    r0 = in[0];
    r1 = in[1];
    r2 = in[2];
    r3 = in[3];
    r4 = in[4];

    do {
        d0 = r0 * 2;
        d1 = r1 * 2;
        d2 = r2 * 2 * 19ULL;
        d419 = r4 * 19ULL;
        d4 = d419 * 2;

        t[0] = ((uint128_t) r0) * r0 + ((uint128_t) d4) * r1
               + (((uint128_t) d2) * (r3));
        t[1] = ((uint128_t) d0) * r1 + ((uint128_t) d4) * r2
               + (((uint128_t) r3) * (r3 * 19ULL));
        t[2] = ((uint128_t) d0) * r2 + ((uint128_t) r1) * r1
               + (((uint128_t) d4) * (r3));
        t[3] = ((uint128_t) d0) * r3 + ((uint128_t) d1) * r2
               + (((uint128_t) r4) * (d419));
        t[4] = ((uint128_t) d0) * r4 + ((uint128_t) d1) * r3
               + (((uint128_t) r2) * (r2));

        r0 = (uint64_t) t[0] & mask;
        c = (uint64_t) (t[0] >> 51);
        t[1] += c;
        r1 = (uint64_t) t[1] & mask;
        c = (uint64_t) (t[1] >> 51);
        t[2] += c;
        r2 = (uint64_t) t[2] & mask;
        c = (uint64_t) (t[2] >> 51);
        t[3] += c;
        r3 = (uint64_t) t[3] & mask;
        c = (uint64_t) (t[3] >> 51);
        t[4] += c;
        r4 = (uint64_t) t[4] & mask;
        c = (uint64_t) (t[4] >> 51);
        r0 += c * 19ULL;
        c = r0 >> 51;
        r0 = r0 & mask;
        r1 += c;
        c = r1 >> 51;
        r1 = r1 & mask;
        r2 += c;
    } while (--count);

    output[0] = r0;
    output[1] = r1;
    output[2] = r2;
    output[3] = r3;
    output[4] = r4;
}

/* Input: Q, Q', Q-Q'
 * Output: 2Q, Q+Q'
 *
 *   x2 z2: long form
 *   x3 z3: long form
 *   x z: short form, destroyed
 *   xprime zprime: short form, destroyed
 *   qmqp: short form, preserved
 */
static void
fe_mont_y(fe x2,     fe z2,     /* output 2Q */
          fe x3,     fe z3,     /* output Q + Q' */
          fe x,      fe z,      /* input Q */
          fe xprime, fe zprime, /* input Q' */
          const fe qmqp         /* input Q - Q' */)
{
    fe origx, origxprime, zzz, xx, zz, xxprime, zzprime, zzzprime;

    memcpy(origx, x, 5 * sizeof(uint64_t));
    fe_add_self(x, z);
    fe_sub_backwards(z, origx); /* does x - z */

    memcpy(origxprime, xprime, sizeof(uint64_t) * 5);
    fe_add_self(xprime, zprime);
    fe_sub_backwards(zprime, origxprime);
    fe_mul_restrict(xxprime, xprime, z);
    fe_mul_restrict(zzprime, x, zprime);
    memcpy(origxprime, xxprime, sizeof(uint64_t) * 5);
    fe_add_self(xxprime, zzprime);
    fe_sub_backwards(zzprime, origxprime);
    fe_square_times(x3, xxprime, 1);
    fe_square_times(zzzprime, zzprime, 1);
    fe_mul_restrict(z3, zzzprime, qmqp);

    fe_square_times(xx, x, 1);
    fe_square_times(zz, z, 1);
    fe_mul_restrict(x2, xx, zz);
    fe_sub_backwards(zz, xx); /* does zz = xx - zz */
    fe_scalar_product(zzz, zz, 121665);
    fe_add_self(zzz, xx);
    fe_mul_restrict(z2, zz, zzz);
}

/* Calculates nQ where Q is the x-coordinate of a point on the curve
 *
 *   resultx/resultz: the x coordinate of the resulting curve point (short form)
 *   n: a little endian, 32-byte number
 *   q: a point of the curve (short form)
 */
static void
cmult(fe resultx, fe resultz, const uint8_t *n, const fe q)
{
    fe a = { 0 }, b = { 1 }, c = { 1 }, d = { 0 };
    uint64_t *nqpqx = a, *nqpqz = b, *nqx = c, *nqz = d, *t;
    fe e = { 0 }, f = { 1 }, g = { 0 }, h = { 1 };
    uint64_t *nqpqx2 = e, *nqpqz2 = f, *nqx2 = g, *nqz2 = h;

    unsigned i, j;

    memcpy(nqpqx, q, sizeof(uint64_t) * 5);

    for (i = 0; i < 32; ++i) {
        uint8_t byte = n[31 - i];
        for (j = 0; j < 8; ++j) {
            const unsigned int bit = byte >> 7;

            fe_cswap(nqx, nqpqx, bit);
            fe_cswap(nqz, nqpqz, bit);
            fe_mont_y(nqx2, nqz2, nqpqx2, nqpqz2, nqx, nqz, nqpqx, nqpqz, q);
            fe_cswap(nqx2, nqpqx2, bit);
            fe_cswap(nqz2, nqpqz2, bit);

            t = nqx;
            nqx = nqx2;
            nqx2 = t;
            t = nqz;
            nqz = nqz2;
            nqz2 = t;
            t = nqpqx;
            nqpqx = nqpqx2;
            nqpqx2 = t;
            t = nqpqz;
            nqpqz = nqpqz2;
            nqpqz2 = t;

            byte <<= 1;
        }
    }

    memcpy(resultx, nqx, sizeof(uint64_t) * 5);
    memcpy(resultz, nqz, sizeof(uint64_t) * 5);
}

/* -----------------------------------------------------------------------------
   Shamelessly copied from djb's code, tightened a little
   -----------------------------------------------------------------------------
   */
static void
crecip(fe out, const fe z)
{
    fe a, t0, b, c;

    /* 2 */              fe_square_times(a, z, 1); /* a = 2 */
    /* 8 */              fe_square_times(t0, a, 2);
    /* 9 */              fe_mul_restrict(b, t0, z); /* b = 9 */
    /* 11 */             fe_mul_restrict(a, b, a); /* a = 11 */
    /* 22 */             fe_square_times(t0, a, 1);
    /* 2^5 - 2^0 = 31 */ fe_mul_restrict(b, t0, b);
    /* 2^10 - 2^5 */     fe_square_times(t0, b, 5);
    /* 2^10 - 2^0 */     fe_mul_restrict(b, t0, b);
    /* 2^20 - 2^10 */    fe_square_times(t0, b, 10);
    /* 2^20 - 2^0 */     fe_mul_restrict(c, t0, b);
    /* 2^40 - 2^20 */    fe_square_times(t0, c, 20);
    /* 2^40 - 2^0 */     fe_mul_restrict(t0, t0, c);
    /* 2^50 - 2^10 */    fe_square_times(t0, t0, 10);
    /* 2^50 - 2^0 */     fe_mul_restrict(b, t0, b);
    /* 2^100 - 2^50 */   fe_square_times(t0, b, 50);
    /* 2^100 - 2^0 */    fe_mul_restrict(c, t0, b);
    /* 2^200 - 2^100 */  fe_square_times(t0, c, 100);
    /* 2^200 - 2^0 */    fe_mul_restrict(t0, t0, c);
    /* 2^250 - 2^50 */   fe_square_times(t0, t0, 50);
    /* 2^250 - 2^0 */    fe_mul_restrict(t0, t0, b);
    /* 2^255 - 2^5 */    fe_square_times(t0, t0, 5);
    /* 2^255 - 21 */     fe_mul_restrict(out, t0, a);
}

static int
crypto_scalarmult_curve25519_donna_c64(unsigned char *q,
                                       const unsigned char *n,
                                       const unsigned char *p)
{
    fe             bp, x, z, zmone;
    unsigned char *t = q;
    int            i;

    for (i = 0; i < 32; ++i) {
        t[i] = n[i];
    }
    t[0] &= 248;
    t[31] &= 127;
    t[31] |= 64;

    fe_frombytes(bp, p);
    cmult(x, z, t, bp);
    crecip(zmone, z);
    fe_mul_restrict(z, x, zmone);
    fe_tobytes(q, z);

    return 0;
}

static int
crypto_scalarmult_curve25519_donna_c64_base(unsigned char *q,
                                            const unsigned char *n)
{
    static const unsigned char basepoint[32] = { 9 };

    return crypto_scalarmult_curve25519_donna_c64(q, n, basepoint);
}

struct crypto_scalarmult_curve25519_implementation
    crypto_scalarmult_curve25519_donna_c64_implementation = {
        SODIUM_C99(.mult =) crypto_scalarmult_curve25519_donna_c64,
        SODIUM_C99(.mult_base =) crypto_scalarmult_curve25519_donna_c64_base
    };

#endif
