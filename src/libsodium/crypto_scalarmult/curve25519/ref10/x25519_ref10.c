
#include <stddef.h>
#include <stdint.h>

#ifndef HAVE_TI_MODE

#include "utils.h"
#include "x25519_ref10.h"
#include "../scalarmult_curve25519.h"
#include "private/curve25519_ref10.h"

/*
Replace (f,g) with (g,f) if b == 1;
replace (f,g) with (f,g) if b == 0.

Preconditions: b in {0,1}.
*/

static void
fe_cswap(fe f,fe g,unsigned int b)
{
  int32_t f0 = f[0];
  int32_t f1 = f[1];
  int32_t f2 = f[2];
  int32_t f3 = f[3];
  int32_t f4 = f[4];
  int32_t f5 = f[5];
  int32_t f6 = f[6];
  int32_t f7 = f[7];
  int32_t f8 = f[8];
  int32_t f9 = f[9];
  int32_t g0 = g[0];
  int32_t g1 = g[1];
  int32_t g2 = g[2];
  int32_t g3 = g[3];
  int32_t g4 = g[4];
  int32_t g5 = g[5];
  int32_t g6 = g[6];
  int32_t g7 = g[7];
  int32_t g8 = g[8];
  int32_t g9 = g[9];
  int32_t x0 = f0 ^ g0;
  int32_t x1 = f1 ^ g1;
  int32_t x2 = f2 ^ g2;
  int32_t x3 = f3 ^ g3;
  int32_t x4 = f4 ^ g4;
  int32_t x5 = f5 ^ g5;
  int32_t x6 = f6 ^ g6;
  int32_t x7 = f7 ^ g7;
  int32_t x8 = f8 ^ g8;
  int32_t x9 = f9 ^ g9;
  b = (unsigned int) (- (int) b);
  x0 &= b;
  x1 &= b;
  x2 &= b;
  x3 &= b;
  x4 &= b;
  x5 &= b;
  x6 &= b;
  x7 &= b;
  x8 &= b;
  x9 &= b;
  f[0] = f0 ^ x0;
  f[1] = f1 ^ x1;
  f[2] = f2 ^ x2;
  f[3] = f3 ^ x3;
  f[4] = f4 ^ x4;
  f[5] = f5 ^ x5;
  f[6] = f6 ^ x6;
  f[7] = f7 ^ x7;
  f[8] = f8 ^ x8;
  f[9] = f9 ^ x9;
  g[0] = g0 ^ x0;
  g[1] = g1 ^ x1;
  g[2] = g2 ^ x2;
  g[3] = g3 ^ x3;
  g[4] = g4 ^ x4;
  g[5] = g5 ^ x5;
  g[6] = g6 ^ x6;
  g[7] = g7 ^ x7;
  g[8] = g8 ^ x8;
  g[9] = g9 ^ x9;
}

/*
h = f * 121666
Can overlap h with f.

Preconditions:
   |f| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.

Postconditions:
   |h| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
*/

static void
fe_mul121666(fe h,const fe f)
{
  int32_t f0 = f[0];
  int32_t f1 = f[1];
  int32_t f2 = f[2];
  int32_t f3 = f[3];
  int32_t f4 = f[4];
  int32_t f5 = f[5];
  int32_t f6 = f[6];
  int32_t f7 = f[7];
  int32_t f8 = f[8];
  int32_t f9 = f[9];
  int64_t h0 = f0 * (int64_t) 121666;
  int64_t h1 = f1 * (int64_t) 121666;
  int64_t h2 = f2 * (int64_t) 121666;
  int64_t h3 = f3 * (int64_t) 121666;
  int64_t h4 = f4 * (int64_t) 121666;
  int64_t h5 = f5 * (int64_t) 121666;
  int64_t h6 = f6 * (int64_t) 121666;
  int64_t h7 = f7 * (int64_t) 121666;
  int64_t h8 = f8 * (int64_t) 121666;
  int64_t h9 = f9 * (int64_t) 121666;
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

  carry9 = (h9 + ((int64_t) 1 << 24)) >> 25; h0 += carry9 * 19; h9 -= carry9 * ((int64_t) 1 << 25);
  carry1 = (h1 + ((int64_t) 1 << 24)) >> 25; h2 += carry1; h1 -= carry1 * ((int64_t) 1 << 25);
  carry3 = (h3 + ((int64_t) 1 << 24)) >> 25; h4 += carry3; h3 -= carry3 * ((int64_t) 1 << 25);
  carry5 = (h5 + ((int64_t) 1 << 24)) >> 25; h6 += carry5; h5 -= carry5 * ((int64_t) 1 << 25);
  carry7 = (h7 + ((int64_t) 1 << 24)) >> 25; h8 += carry7; h7 -= carry7 * ((int64_t) 1 << 25);

  carry0 = (h0 + ((int64_t) 1 << 25)) >> 26; h1 += carry0; h0 -= carry0 * ((int64_t) 1 << 26);
  carry2 = (h2 + ((int64_t) 1 << 25)) >> 26; h3 += carry2; h2 -= carry2 * ((int64_t) 1 << 26);
  carry4 = (h4 + ((int64_t) 1 << 25)) >> 26; h5 += carry4; h4 -= carry4 * ((int64_t) 1 << 26);
  carry6 = (h6 + ((int64_t) 1 << 25)) >> 26; h7 += carry6; h6 -= carry6 * ((int64_t) 1 << 26);
  carry8 = (h8 + ((int64_t) 1 << 25)) >> 26; h9 += carry8; h8 -= carry8 * ((int64_t) 1 << 26);

  h[0] = h0;
  h[1] = h1;
  h[2] = h2;
  h[3] = h3;
  h[4] = h4;
  h[5] = h5;
  h[6] = h6;
  h[7] = h7;
  h[8] = h8;
  h[9] = h9;
}

static int
crypto_scalarmult_curve25519_ref10(unsigned char *q,
                                   const unsigned char *n,
                                   const unsigned char *p)
{
  unsigned char e[32];
  unsigned int i;
  fe x1;
  fe x2;
  fe z2;
  fe x3;
  fe z3;
  fe tmp0;
  fe tmp1;
  int pos;
  unsigned int swap;
  unsigned int b;

  for (i = 0;i < 32;++i) e[i] = n[i];
  e[0] &= 248;
  e[31] &= 127;
  e[31] |= 64;
  fe_frombytes(x1,p);
  fe_1(x2);
  fe_0(z2);
  fe_copy(x3,x1);
  fe_1(z3);

  swap = 0;
  for (pos = 254;pos >= 0;--pos) {
    b = e[pos / 8] >> (pos & 7);
    b &= 1;
    swap ^= b;
    fe_cswap(x2,x3,swap);
    fe_cswap(z2,z3,swap);
    swap = b;
    fe_sub(tmp0,x3,z3);
    fe_sub(tmp1,x2,z2);
    fe_add(x2,x2,z2);
    fe_add(z2,x3,z3);
    fe_mul(z3,tmp0,x2);
    fe_mul(z2,z2,tmp1);
    fe_sq(tmp0,tmp1);
    fe_sq(tmp1,x2);
    fe_add(x3,z3,z2);
    fe_sub(z2,z3,z2);
    fe_mul(x2,tmp1,tmp0);
    fe_sub(tmp1,tmp1,tmp0);
    fe_sq(z2,z2);
    fe_mul121666(z3,tmp1);
    fe_sq(x3,x3);
    fe_add(tmp0,tmp0,z3);
    fe_mul(z3,x1,z2);
    fe_mul(z2,tmp1,tmp0);
  }
  fe_cswap(x2,x3,swap);
  fe_cswap(z2,z3,swap);

  fe_invert(z2,z2);
  fe_mul(x2,x2,z2);
  fe_tobytes(q,x2);
  return 0;
}

static void
edwards_to_montgomery(fe montgomeryX, const fe edwardsY, const fe edwardsZ)
{
  fe tempX;
  fe tempZ;

  fe_add(tempX, edwardsZ, edwardsY);
  fe_sub(tempZ, edwardsZ, edwardsY);
  fe_invert(tempZ, tempZ);
  fe_mul(montgomeryX, tempX, tempZ);
}

static int
crypto_scalarmult_curve25519_ref10_base(unsigned char *q,
                                        const unsigned char *n)
{
  unsigned char e[32];
  ge_p3 A;
  fe pk;
  unsigned int i;

  for (i = 0;i < 32;++i) e[i] = n[i];
  e[0] &= 248;
  e[31] &= 127;
  e[31] |= 64;
  ge_scalarmult_base(&A, e);
  edwards_to_montgomery(pk, A.Y, A.Z);
  fe_tobytes(q, pk);
  return 0;
}

struct crypto_scalarmult_curve25519_implementation
crypto_scalarmult_curve25519_ref10_implementation = {
        SODIUM_C99(.mult = ) crypto_scalarmult_curve25519_ref10,
        SODIUM_C99(.mult_base = ) crypto_scalarmult_curve25519_ref10_base
};

#endif
