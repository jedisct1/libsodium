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
 *
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

#include <string.h>
#include <stdint.h>
#include "crypto_scalarmult.h"

typedef uint8_t u8;
typedef uint64_t felem;
// This is a special gcc mode for 128-bit integers. It's implemented on 64-bit
// platforms only as far as I know.
typedef unsigned uint128_t __attribute__((mode(TI)));

/* Sum two numbers: output += in */
static void fsum(felem *output, const felem *in) {
  unsigned i;
  for (i = 0; i < 5; ++i) output[i] += in[i];
}

/* Find the difference of two numbers: output = in - output
 * (note the order of the arguments!)
 */
static void fdifference_backwards(felem *ioutput, const felem *iin) {
  static const int64_t twotothe51 = (1l << 51);
  const int64_t *in = (const int64_t *) iin;
  int64_t *out = (int64_t *) ioutput;

  out[0] = in[0] - out[0];
  out[1] = in[1] - out[1];
  out[2] = in[2] - out[2];
  out[3] = in[3] - out[3];
  out[4] = in[4] - out[4];

  // An arithmetic shift right of 63 places turns a positive number to 0 and a
  // negative number to all 1's. This gives us a bitmask that lets us avoid
  // side-channel prone branches.
  int64_t t;

#define NEGCHAIN(a,b) \
  t = out[a] >> 63; \
  out[a] += twotothe51 & t; \
  out[b] -= 1 & t;

#define NEGCHAIN19(a,b) \
  t = out[a] >> 63; \
  out[a] += twotothe51 & t; \
  out[b] -= 19 & t;

  NEGCHAIN(0, 1);
  NEGCHAIN(1, 2);
  NEGCHAIN(2, 3);
  NEGCHAIN(3, 4);
  NEGCHAIN19(4, 0);
  NEGCHAIN(0, 1);
  NEGCHAIN(1, 2);
  NEGCHAIN(2, 3);
  NEGCHAIN(3, 4);
}

/* Multiply a number by a scalar: output = in * scalar */
static void fscalar_product(felem *output, const felem *in, const felem scalar) {
  uint128_t a;

  a = ((uint128_t) in[0]) * scalar;
  output[0] = a & 0x7ffffffffffff;

  a = ((uint128_t) in[1]) * scalar + (a >> 51);
  output[1] = a & 0x7ffffffffffff;

  a = ((uint128_t) in[2]) * scalar + (a >> 51);
  output[2] = a & 0x7ffffffffffff;

  a = ((uint128_t) in[3]) * scalar + (a >> 51);
  output[3] = a & 0x7ffffffffffff;

  a = ((uint128_t) in[4]) * scalar + (a >> 51);
  output[4] = a & 0x7ffffffffffff;

  output[0] += (a >> 51) * 19;
}

/* Multiply two numbers: output = in2 * in
 *
 * output must be distinct to both inputs. The inputs are reduced coefficient
 * form, the output is not.
 */
static void fmul(felem *output, const felem *in2, const felem *in) {
  uint128_t t[9];

  t[0] = ((uint128_t) in[0]) * in2[0];
  t[1] = ((uint128_t) in[0]) * in2[1] +
         ((uint128_t) in[1]) * in2[0];
  t[2] = ((uint128_t) in[0]) * in2[2] +
         ((uint128_t) in[2]) * in2[0] +
         ((uint128_t) in[1]) * in2[1];
  t[3] = ((uint128_t) in[0]) * in2[3] +
         ((uint128_t) in[3]) * in2[0] +
         ((uint128_t) in[1]) * in2[2] +
         ((uint128_t) in[2]) * in2[1];
  t[4] = ((uint128_t) in[0]) * in2[4] +
         ((uint128_t) in[4]) * in2[0] +
         ((uint128_t) in[3]) * in2[1] +
         ((uint128_t) in[1]) * in2[3] +
         ((uint128_t) in[2]) * in2[2];
  t[5] = ((uint128_t) in[4]) * in2[1] +
         ((uint128_t) in[1]) * in2[4] +
         ((uint128_t) in[2]) * in2[3] +
         ((uint128_t) in[3]) * in2[2];
  t[6] = ((uint128_t) in[4]) * in2[2] +
         ((uint128_t) in[2]) * in2[4] +
         ((uint128_t) in[3]) * in2[3];
  t[7] = ((uint128_t) in[3]) * in2[4] +
         ((uint128_t) in[4]) * in2[3];
  t[8] = ((uint128_t) in[4]) * in2[4];

  t[0] += t[5] * 19;
  t[1] += t[6] * 19;
  t[2] += t[7] * 19;
  t[3] += t[8] * 19;

  t[1] += t[0] >> 51;
  t[0] &= 0x7ffffffffffff;
  t[2] += t[1] >> 51;
  t[1] &= 0x7ffffffffffff;
  t[3] += t[2] >> 51;
  t[2] &= 0x7ffffffffffff;
  t[4] += t[3] >> 51;
  t[3] &= 0x7ffffffffffff;
  t[0] += 19 * (t[4] >> 51);
  t[4] &= 0x7ffffffffffff;
  t[1] += t[0] >> 51;
  t[0] &= 0x7ffffffffffff;
  t[2] += t[1] >> 51;
  t[1] &= 0x7ffffffffffff;

  output[0] = t[0];
  output[1] = t[1];
  output[2] = t[2];
  output[3] = t[3];
  output[4] = t[4];
}

static void
fsquare(felem *output, const felem *in) {
  uint128_t t[9];

  t[0] = ((uint128_t) in[0]) * in[0];
  t[1] = ((uint128_t) in[0]) * in[1] * 2;
  t[2] = ((uint128_t) in[0]) * in[2] * 2 +
         ((uint128_t) in[1]) * in[1];
  t[3] = ((uint128_t) in[0]) * in[3] * 2 +
         ((uint128_t) in[1]) * in[2] * 2;
  t[4] = ((uint128_t) in[0]) * in[4] * 2 +
         ((uint128_t) in[3]) * in[1] * 2 +
         ((uint128_t) in[2]) * in[2];
  t[5] = ((uint128_t) in[4]) * in[1] * 2 +
         ((uint128_t) in[2]) * in[3] * 2;
  t[6] = ((uint128_t) in[4]) * in[2] * 2 +
         ((uint128_t) in[3]) * in[3];
  t[7] = ((uint128_t) in[3]) * in[4] * 2;
  t[8] = ((uint128_t) in[4]) * in[4];

  t[0] += t[5] * 19;
  t[1] += t[6] * 19;
  t[2] += t[7] * 19;
  t[3] += t[8] * 19;

  t[1] += t[0] >> 51;
  t[0] &= 0x7ffffffffffff;
  t[2] += t[1] >> 51;
  t[1] &= 0x7ffffffffffff;
  t[3] += t[2] >> 51;
  t[2] &= 0x7ffffffffffff;
  t[4] += t[3] >> 51;
  t[3] &= 0x7ffffffffffff;
  t[0] += 19 * (t[4] >> 51);
  t[4] &= 0x7ffffffffffff;
  t[1] += t[0] >> 51;
  t[0] &= 0x7ffffffffffff;

  output[0] = t[0];
  output[1] = t[1];
  output[2] = t[2];
  output[3] = t[3];
  output[4] = t[4];
}

/* Take a little-endian, 32-byte number and expand it into polynomial form */
static void
fexpand(felem *output, const u8 *in) {
  output[0] = *((const uint64_t *)(in)) & 0x7ffffffffffff;
  output[1] = (*((const uint64_t *)(in+6)) >> 3) & 0x7ffffffffffff;
  output[2] = (*((const uint64_t *)(in+12)) >> 6) & 0x7ffffffffffff;
  output[3] = (*((const uint64_t *)(in+19)) >> 1) & 0x7ffffffffffff;
  output[4] = (*((const uint64_t *)(in+25)) >> 4) & 0x7ffffffffffff;
}

/* Take a fully reduced polynomial form number and contract it into a
 * little-endian, 32-byte array
 */
static void
fcontract(u8 *output, const felem *input) {
  uint128_t t[5];

  t[0] = input[0];
  t[1] = input[1];
  t[2] = input[2];
  t[3] = input[3];
  t[4] = input[4];

  t[1] += t[0] >> 51; t[0] &= 0x7ffffffffffff;
  t[2] += t[1] >> 51; t[1] &= 0x7ffffffffffff;
  t[3] += t[2] >> 51; t[2] &= 0x7ffffffffffff;
  t[4] += t[3] >> 51; t[3] &= 0x7ffffffffffff;
  t[0] += 19 * (t[4] >> 51); t[4] &= 0x7ffffffffffff;

  t[1] += t[0] >> 51; t[0] &= 0x7ffffffffffff;
  t[2] += t[1] >> 51; t[1] &= 0x7ffffffffffff;
  t[3] += t[2] >> 51; t[2] &= 0x7ffffffffffff;
  t[4] += t[3] >> 51; t[3] &= 0x7ffffffffffff;
  t[0] += 19 * (t[4] >> 51); t[4] &= 0x7ffffffffffff;

  /* now t is between 0 and 2^255-1, properly carried. */
  /* case 1: between 0 and 2^255-20. case 2: between 2^255-19 and 2^255-1. */

  t[0] += 19;

  t[1] += t[0] >> 51; t[0] &= 0x7ffffffffffff;
  t[2] += t[1] >> 51; t[1] &= 0x7ffffffffffff;
  t[3] += t[2] >> 51; t[2] &= 0x7ffffffffffff;
  t[4] += t[3] >> 51; t[3] &= 0x7ffffffffffff;
  t[0] += 19 * (t[4] >> 51); t[4] &= 0x7ffffffffffff;

  /* now between 19 and 2^255-1 in both cases, and offset by 19. */

  t[0] += 0x8000000000000 - 19;
  t[1] += 0x8000000000000 - 1;
  t[2] += 0x8000000000000 - 1;
  t[3] += 0x8000000000000 - 1;
  t[4] += 0x8000000000000 - 1;

  /* now between 2^255 and 2^256-20, and offset by 2^255. */

  t[1] += t[0] >> 51; t[0] &= 0x7ffffffffffff;
  t[2] += t[1] >> 51; t[1] &= 0x7ffffffffffff;
  t[3] += t[2] >> 51; t[2] &= 0x7ffffffffffff;
  t[4] += t[3] >> 51; t[3] &= 0x7ffffffffffff;
  t[4] &= 0x7ffffffffffff;

  *((uint64_t *)(output)) = t[0] | (t[1] << 51);
  *((uint64_t *)(output+8)) = (t[1] >> 13) | (t[2] << 38);
  *((uint64_t *)(output+16)) = (t[2] >> 26) | (t[3] << 25);
  *((uint64_t *)(output+24)) = (t[3] >> 39) | (t[4] << 12);
}

/* Input: Q, Q', Q-Q'
 * Output: 2Q, Q+Q'
 *
 *   x2 z3: long form
 *   x3 z3: long form
 *   x z: short form, destroyed
 *   xprime zprime: short form, destroyed
 *   qmqp: short form, preserved
 */
static void
fmonty(felem *x2, felem *z2, /* output 2Q */
       felem *x3, felem *z3, /* output Q + Q' */
       felem *x, felem *z,   /* input Q */
       felem *xprime, felem *zprime, /* input Q' */
       const felem *qmqp /* input Q - Q' */) {
  felem origx[5], origxprime[5], zzz[5], xx[5], zz[5], xxprime[5],
        zzprime[5], zzzprime[5];

  memcpy(origx, x, 5 * sizeof(felem));
  fsum(x, z);
  fdifference_backwards(z, origx);  // does x - z

  memcpy(origxprime, xprime, sizeof(felem) * 5);
  fsum(xprime, zprime);
  fdifference_backwards(zprime, origxprime);
  fmul(xxprime, xprime, z);
  fmul(zzprime, x, zprime);
  memcpy(origxprime, xxprime, sizeof(felem) * 5);
  fsum(xxprime, zzprime);
  fdifference_backwards(zzprime, origxprime);
  fsquare(x3, xxprime);
  fsquare(zzzprime, zzprime);
  fmul(z3, zzzprime, qmqp);

  fsquare(xx, x);
  fsquare(zz, z);
  fmul(x2, xx, zz);
  fdifference_backwards(zz, xx);  // does zz = xx - zz
  fscalar_product(zzz, zz, 121665);
  fsum(zzz, xx);
  fmul(z2, zz, zzz);
}

// -----------------------------------------------------------------------------
// Maybe swap the contents of two felem arrays (@a and @b), each @len elements
// long. Perform the swap iff @swap is non-zero.
//
// This function performs the swap without leaking any side-channel
// information.
// -----------------------------------------------------------------------------
static void
swap_conditional(felem *a, felem *b, unsigned len, felem iswap) {
  unsigned i;
  const felem swap = -iswap;

  for (i = 0; i < len; ++i) {
    const felem x = swap & (a[i] ^ b[i]);
    a[i] ^= x;
    b[i] ^= x;
  }
}

/* Calculates nQ where Q is the x-coordinate of a point on the curve
 *
 *   resultx/resultz: the x coordinate of the resulting curve point (short form)
 *   n: a little endian, 32-byte number
 *   q: a point of the curve (short form)
 */
static void
cmult(felem *resultx, felem *resultz, const u8 *n, const felem *q) {
  felem a[5] = {0}, b[5] = {1}, c[5] = {1}, d[5] = {0};
  felem *nqpqx = a, *nqpqz = b, *nqx = c, *nqz = d, *t;
  felem e[5] = {0}, f[5] = {1}, g[5] = {0}, h[5] = {1};
  felem *nqpqx2 = e, *nqpqz2 = f, *nqx2 = g, *nqz2 = h;

  unsigned i, j;

  memcpy(nqpqx, q, sizeof(felem) * 5);

  for (i = 0; i < 32; ++i) {
    u8 byte = n[31 - i];
    for (j = 0; j < 8; ++j) {
      const felem bit = byte >> 7;

      swap_conditional(nqx, nqpqx, 5, bit);
      swap_conditional(nqz, nqpqz, 5, bit);
      fmonty(nqx2, nqz2,
             nqpqx2, nqpqz2,
             nqx, nqz,
             nqpqx, nqpqz,
             q);
      swap_conditional(nqx2, nqpqx2, 5, bit);
      swap_conditional(nqz2, nqpqz2, 5, bit);

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

  memcpy(resultx, nqx, sizeof(felem) * 5);
  memcpy(resultz, nqz, sizeof(felem) * 5);
}

// -----------------------------------------------------------------------------
// Shamelessly copied from djb's code
// -----------------------------------------------------------------------------
static void
crecip(felem *out, const felem *z) {
  felem z2[5];
  felem z9[5];
  felem z11[5];
  felem z2_5_0[5];
  felem z2_10_0[5];
  felem z2_20_0[5];
  felem z2_50_0[5];
  felem z2_100_0[5];
  felem t0[5];
  felem t1[5];
  int i;

  /* 2 */ fsquare(z2,z);
  /* 4 */ fsquare(t1,z2);
  /* 8 */ fsquare(t0,t1);
  /* 9 */ fmul(z9,t0,z);
  /* 11 */ fmul(z11,z9,z2);
  /* 22 */ fsquare(t0,z11);
  /* 2^5 - 2^0 = 31 */ fmul(z2_5_0,t0,z9);

  /* 2^6 - 2^1 */ fsquare(t0,z2_5_0);
  /* 2^7 - 2^2 */ fsquare(t1,t0);
  /* 2^8 - 2^3 */ fsquare(t0,t1);
  /* 2^9 - 2^4 */ fsquare(t1,t0);
  /* 2^10 - 2^5 */ fsquare(t0,t1);
  /* 2^10 - 2^0 */ fmul(z2_10_0,t0,z2_5_0);

  /* 2^11 - 2^1 */ fsquare(t0,z2_10_0);
  /* 2^12 - 2^2 */ fsquare(t1,t0);
  /* 2^20 - 2^10 */ for (i = 2;i < 10;i += 2) { fsquare(t0,t1); fsquare(t1,t0); }
  /* 2^20 - 2^0 */ fmul(z2_20_0,t1,z2_10_0);

  /* 2^21 - 2^1 */ fsquare(t0,z2_20_0);
  /* 2^22 - 2^2 */ fsquare(t1,t0);
  /* 2^40 - 2^20 */ for (i = 2;i < 20;i += 2) { fsquare(t0,t1); fsquare(t1,t0); }
  /* 2^40 - 2^0 */ fmul(t0,t1,z2_20_0);

  /* 2^41 - 2^1 */ fsquare(t1,t0);
  /* 2^42 - 2^2 */ fsquare(t0,t1);
  /* 2^50 - 2^10 */ for (i = 2;i < 10;i += 2) { fsquare(t1,t0); fsquare(t0,t1); }
  /* 2^50 - 2^0 */ fmul(z2_50_0,t0,z2_10_0);

  /* 2^51 - 2^1 */ fsquare(t0,z2_50_0);
  /* 2^52 - 2^2 */ fsquare(t1,t0);
  /* 2^100 - 2^50 */ for (i = 2;i < 50;i += 2) { fsquare(t0,t1); fsquare(t1,t0); }
  /* 2^100 - 2^0 */ fmul(z2_100_0,t1,z2_50_0);

  /* 2^101 - 2^1 */ fsquare(t1,z2_100_0);
  /* 2^102 - 2^2 */ fsquare(t0,t1);
  /* 2^200 - 2^100 */ for (i = 2;i < 100;i += 2) { fsquare(t1,t0); fsquare(t0,t1); }
  /* 2^200 - 2^0 */ fmul(t1,t0,z2_100_0);

  /* 2^201 - 2^1 */ fsquare(t0,t1);
  /* 2^202 - 2^2 */ fsquare(t1,t0);
  /* 2^250 - 2^50 */ for (i = 2;i < 50;i += 2) { fsquare(t0,t1); fsquare(t1,t0); }
  /* 2^250 - 2^0 */ fmul(t0,t1,z2_50_0);

  /* 2^251 - 2^1 */ fsquare(t1,t0);
  /* 2^252 - 2^2 */ fsquare(t0,t1);
  /* 2^253 - 2^3 */ fsquare(t1,t0);
  /* 2^254 - 2^4 */ fsquare(t0,t1);
  /* 2^255 - 2^5 */ fsquare(t1,t0);
  /* 2^255 - 21 */ fmul(out,t1,z11);
}

int
crypto_scalarmult(u8 *mypublic, const u8 *secret, const u8 *basepoint) {
  felem bp[5], x[5], z[5], zmone[5];
  unsigned char e[32];
  int i;
  for (i = 0;i < 32;++i) e[i] = secret[i];
  e[0] &= 248;
  e[31] &= 127;
  e[31] |= 64;
  fexpand(bp, basepoint);
  cmult(x, z, e, bp);
  crecip(zmone, z);
  fmul(z, x, zmone);
  fcontract(mypublic, z);
  return 0;
}
