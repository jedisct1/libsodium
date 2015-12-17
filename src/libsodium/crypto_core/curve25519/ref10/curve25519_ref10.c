#include "curve25519_ref10.h"
#include "crypto_int32.h"
#include "crypto_int64.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"
#include "crypto_verify_32.h"

static crypto_uint64 load_3(const unsigned char *in)
{
  crypto_uint64 result;
  result = (crypto_uint64) in[0];
  result |= ((crypto_uint64) in[1]) << 8;
  result |= ((crypto_uint64) in[2]) << 16;
  return result;
}

static crypto_uint64 load_4(const unsigned char *in)
{
  crypto_uint64 result;
  result = (crypto_uint64) in[0];
  result |= ((crypto_uint64) in[1]) << 8;
  result |= ((crypto_uint64) in[2]) << 16;
  result |= ((crypto_uint64) in[3]) << 24;
  return result;
}

/*
h = 0
*/

void fe_0(fe h)
{
  h[0] = 0;
  h[1] = 0;
  h[2] = 0;
  h[3] = 0;
  h[4] = 0;
  h[5] = 0;
  h[6] = 0;
  h[7] = 0;
  h[8] = 0;
  h[9] = 0;
}

/*
h = 1
*/

void fe_1(fe h)
{
  h[0] = 1;
  h[1] = 0;
  h[2] = 0;
  h[3] = 0;
  h[4] = 0;
  h[5] = 0;
  h[6] = 0;
  h[7] = 0;
  h[8] = 0;
  h[9] = 0;
}

/*
h = f + g
Can overlap h with f or g.

Preconditions:
   |f| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
   |g| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.

Postconditions:
   |h| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/

void fe_add(fe h,const fe f,const fe g)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  crypto_int32 g0 = g[0];
  crypto_int32 g1 = g[1];
  crypto_int32 g2 = g[2];
  crypto_int32 g3 = g[3];
  crypto_int32 g4 = g[4];
  crypto_int32 g5 = g[5];
  crypto_int32 g6 = g[6];
  crypto_int32 g7 = g[7];
  crypto_int32 g8 = g[8];
  crypto_int32 g9 = g[9];
  crypto_int32 h0 = f0 + g0;
  crypto_int32 h1 = f1 + g1;
  crypto_int32 h2 = f2 + g2;
  crypto_int32 h3 = f3 + g3;
  crypto_int32 h4 = f4 + g4;
  crypto_int32 h5 = f5 + g5;
  crypto_int32 h6 = f6 + g6;
  crypto_int32 h7 = f7 + g7;
  crypto_int32 h8 = f8 + g8;
  crypto_int32 h9 = f9 + g9;
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

/*
Replace (f,g) with (g,g) if b == 1;
replace (f,g) with (f,g) if b == 0.

Preconditions: b in {0,1}.
*/

void fe_cmov(fe f,const fe g,unsigned int b)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  crypto_int32 g0 = g[0];
  crypto_int32 g1 = g[1];
  crypto_int32 g2 = g[2];
  crypto_int32 g3 = g[3];
  crypto_int32 g4 = g[4];
  crypto_int32 g5 = g[5];
  crypto_int32 g6 = g[6];
  crypto_int32 g7 = g[7];
  crypto_int32 g8 = g[8];
  crypto_int32 g9 = g[9];
  crypto_int32 x0 = f0 ^ g0;
  crypto_int32 x1 = f1 ^ g1;
  crypto_int32 x2 = f2 ^ g2;
  crypto_int32 x3 = f3 ^ g3;
  crypto_int32 x4 = f4 ^ g4;
  crypto_int32 x5 = f5 ^ g5;
  crypto_int32 x6 = f6 ^ g6;
  crypto_int32 x7 = f7 ^ g7;
  crypto_int32 x8 = f8 ^ g8;
  crypto_int32 x9 = f9 ^ g9;
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
}

/*
h = f
*/

void fe_copy(fe h,const fe f)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  h[0] = f0;
  h[1] = f1;
  h[2] = f2;
  h[3] = f3;
  h[4] = f4;
  h[5] = f5;
  h[6] = f6;
  h[7] = f7;
  h[8] = f8;
  h[9] = f9;
}

/*
Ignores top bit of h.
*/

void fe_frombytes(fe h,const unsigned char *s)
{
  crypto_int64 h0 = load_4(s);
  crypto_int64 h1 = load_3(s + 4) * (1L << 6);
  crypto_int64 h2 = load_3(s + 7) * (1L << 5);
  crypto_int64 h3 = load_3(s + 10) * (1L << 3);
  crypto_int64 h4 = load_3(s + 13) * (1L << 2);
  crypto_int64 h5 = load_4(s + 16);
  crypto_int64 h6 = load_3(s + 20) * (1L << 7);
  crypto_int64 h7 = load_3(s + 23) * (1L << 5);
  crypto_int64 h8 = load_3(s + 26) * (1L << 4);
  crypto_int64 h9 = (load_3(s + 29) & 8388607) * (1L << 2);
  crypto_int64 carry0;
  crypto_int64 carry1;
  crypto_int64 carry2;
  crypto_int64 carry3;
  crypto_int64 carry4;
  crypto_int64 carry5;
  crypto_int64 carry6;
  crypto_int64 carry7;
  crypto_int64 carry8;
  crypto_int64 carry9;

  carry9 = (h9 + (crypto_int64) (1L << 24)) >> 25; h0 += carry9 * 19; h9 -= carry9 * (1L << 25);
  carry1 = (h1 + (crypto_int64) (1L << 24)) >> 25; h2 += carry1; h1 -= carry1 * (1L << 25);
  carry3 = (h3 + (crypto_int64) (1L << 24)) >> 25; h4 += carry3; h3 -= carry3 * (1L << 25);
  carry5 = (h5 + (crypto_int64) (1L << 24)) >> 25; h6 += carry5; h5 -= carry5 * (1L << 25);
  carry7 = (h7 + (crypto_int64) (1L << 24)) >> 25; h8 += carry7; h7 -= carry7 * (1L << 25);

  carry0 = (h0 + (crypto_int64) (1L << 25)) >> 26; h1 += carry0; h0 -= carry0 * (1L << 26);
  carry2 = (h2 + (crypto_int64) (1L << 25)) >> 26; h3 += carry2; h2 -= carry2 * (1L << 26);
  carry4 = (h4 + (crypto_int64) (1L << 25)) >> 26; h5 += carry4; h4 -= carry4 * (1L << 26);
  carry6 = (h6 + (crypto_int64) (1L << 25)) >> 26; h7 += carry6; h6 -= carry6 * (1L << 26);
  carry8 = (h8 + (crypto_int64) (1L << 25)) >> 26; h9 += carry8; h8 -= carry8 * (1L << 26);

  h[0] = (crypto_int32) h0;
  h[1] = (crypto_int32) h1;
  h[2] = (crypto_int32) h2;
  h[3] = (crypto_int32) h3;
  h[4] = (crypto_int32) h4;
  h[5] = (crypto_int32) h5;
  h[6] = (crypto_int32) h6;
  h[7] = (crypto_int32) h7;
  h[8] = (crypto_int32) h8;
  h[9] = (crypto_int32) h9;
}

/*
Preconditions:
  |h| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.

Write p=2^255-19; q=floor(h/p).
Basic claim: q = floor(2^(-255)(h + 19 2^(-25)h9 + 2^(-1))).

Proof:
  Have |h|<=p so |q|<=1 so |19^2 2^(-255) q|<1/4.
  Also have |h-2^230 h9|<2^231 so |19 2^(-255)(h-2^230 h9)|<1/4.

  Write y=2^(-1)-19^2 2^(-255)q-19 2^(-255)(h-2^230 h9).
  Then 0<y<1.

  Write r=h-pq.
  Have 0<=r<=p-1=2^255-20.
  Thus 0<=r+19(2^-255)r<r+19(2^-255)2^255<=2^255-1.

  Write x=r+19(2^-255)r+y.
  Then 0<x<2^255 so floor(2^(-255)x) = 0 so floor(q+2^(-255)x) = q.

  Have q+2^(-255)x = 2^(-255)(h + 19 2^(-25) h9 + 2^(-1))
  so floor(2^(-255)(h + 19 2^(-25) h9 + 2^(-1))) = q.
*/

void fe_tobytes(unsigned char *s,const fe h)
{
  crypto_int32 h0 = h[0];
  crypto_int32 h1 = h[1];
  crypto_int32 h2 = h[2];
  crypto_int32 h3 = h[3];
  crypto_int32 h4 = h[4];
  crypto_int32 h5 = h[5];
  crypto_int32 h6 = h[6];
  crypto_int32 h7 = h[7];
  crypto_int32 h8 = h[8];
  crypto_int32 h9 = h[9];
  crypto_int32 q;
  crypto_int32 carry0;
  crypto_int32 carry1;
  crypto_int32 carry2;
  crypto_int32 carry3;
  crypto_int32 carry4;
  crypto_int32 carry5;
  crypto_int32 carry6;
  crypto_int32 carry7;
  crypto_int32 carry8;
  crypto_int32 carry9;

  q = (19 * h9 + (((crypto_int32) 1) << 24)) >> 25;
  q = (h0 + q) >> 26;
  q = (h1 + q) >> 25;
  q = (h2 + q) >> 26;
  q = (h3 + q) >> 25;
  q = (h4 + q) >> 26;
  q = (h5 + q) >> 25;
  q = (h6 + q) >> 26;
  q = (h7 + q) >> 25;
  q = (h8 + q) >> 26;
  q = (h9 + q) >> 25;

  /* Goal: Output h-(2^255-19)q, which is between 0 and 2^255-20. */
  h0 += 19 * q;
  /* Goal: Output h-2^255 q, which is between 0 and 2^255-20. */

  carry0 = h0 >> 26; h1 += carry0; h0 -= carry0 * (1 << 26);
  carry1 = h1 >> 25; h2 += carry1; h1 -= carry1 * (1 << 25);
  carry2 = h2 >> 26; h3 += carry2; h2 -= carry2 * (1 << 26);
  carry3 = h3 >> 25; h4 += carry3; h3 -= carry3 * (1 << 25);
  carry4 = h4 >> 26; h5 += carry4; h4 -= carry4 * (1 << 26);
  carry5 = h5 >> 25; h6 += carry5; h5 -= carry5 * (1 << 25);
  carry6 = h6 >> 26; h7 += carry6; h6 -= carry6 * (1 << 26);
  carry7 = h7 >> 25; h8 += carry7; h7 -= carry7 * (1 << 25);
  carry8 = h8 >> 26; h9 += carry8; h8 -= carry8 * (1 << 26);
  carry9 = h9 >> 25;               h9 -= carry9 * (1 << 25);
                  /* h10 = carry9 */

  /*
  Goal: Output h0+...+2^255 h10-2^255 q, which is between 0 and 2^255-20.
  Have h0+...+2^230 h9 between 0 and 2^255-1;
  evidently 2^255 h10-2^255 q = 0.
  Goal: Output h0+...+2^230 h9.
  */

  s[0] = h0 >> 0;
  s[1] = h0 >> 8;
  s[2] = h0 >> 16;
  s[3] = (h0 >> 24) | (h1 * (1 << 2));
  s[4] = h1 >> 6;
  s[5] = h1 >> 14;
  s[6] = (h1 >> 22) | (h2 * (1 << 3));
  s[7] = h2 >> 5;
  s[8] = h2 >> 13;
  s[9] = (h2 >> 21) | (h3 * (1 << 5));
  s[10] = h3 >> 3;
  s[11] = h3 >> 11;
  s[12] = (h3 >> 19) | (h4 * (1 << 6));
  s[13] = h4 >> 2;
  s[14] = h4 >> 10;
  s[15] = h4 >> 18;
  s[16] = h5 >> 0;
  s[17] = h5 >> 8;
  s[18] = h5 >> 16;
  s[19] = (h5 >> 24) | (h6 * (1 << 1));
  s[20] = h6 >> 7;
  s[21] = h6 >> 15;
  s[22] = (h6 >> 23) | (h7 * (1 << 3));
  s[23] = h7 >> 5;
  s[24] = h7 >> 13;
  s[25] = (h7 >> 21) | (h8 * (1 << 4));
  s[26] = h8 >> 4;
  s[27] = h8 >> 12;
  s[28] = (h8 >> 20) | (h9 * (1 << 6));
  s[29] = h9 >> 2;
  s[30] = h9 >> 10;
  s[31] = h9 >> 18;
}

/*
return 1 if f is in {1,3,5,...,q-2}
return 0 if f is in {0,2,4,...,q-1}

Preconditions:
   |f| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/

int fe_isnegative(const fe f)
{
  unsigned char s[32];
  fe_tobytes(s,f);
  return s[0] & 1;
}

/*
return 1 if f == 0
return 0 if f != 0

Preconditions:
   |f| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/

static unsigned char zero[32];

int fe_isnonzero(const fe f)
{
  unsigned char s[32];
  fe_tobytes(s,f);
  return crypto_verify_32(s,zero);
}

/*
h = f * g
Can overlap h with f or g.

Preconditions:
   |f| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.
   |g| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.

Postconditions:
   |h| bounded by 1.01*2^25,1.01*2^24,1.01*2^25,1.01*2^24,etc.
*/

/*
Notes on implementation strategy:

Using schoolbook multiplication.
Karatsuba would save a little in some cost models.

Most multiplications by 2 and 19 are 32-bit precomputations;
cheaper than 64-bit postcomputations.

There is one remaining multiplication by 19 in the carry chain;
one *19 precomputation can be merged into this,
but the resulting data flow is considerably less clean.

There are 12 carries below.
10 of them are 2-way parallelizable and vectorizable.
Can get away with 11 carries, but then data flow is much deeper.

With tighter constraints on inputs can squeeze carries into int32.
*/

void fe_mul(fe h,const fe f,const fe g)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  crypto_int32 g0 = g[0];
  crypto_int32 g1 = g[1];
  crypto_int32 g2 = g[2];
  crypto_int32 g3 = g[3];
  crypto_int32 g4 = g[4];
  crypto_int32 g5 = g[5];
  crypto_int32 g6 = g[6];
  crypto_int32 g7 = g[7];
  crypto_int32 g8 = g[8];
  crypto_int32 g9 = g[9];
  crypto_int32 g1_19 = 19 * g1; /* 1.959375*2^29 */
  crypto_int32 g2_19 = 19 * g2; /* 1.959375*2^30; still ok */
  crypto_int32 g3_19 = 19 * g3;
  crypto_int32 g4_19 = 19 * g4;
  crypto_int32 g5_19 = 19 * g5;
  crypto_int32 g6_19 = 19 * g6;
  crypto_int32 g7_19 = 19 * g7;
  crypto_int32 g8_19 = 19 * g8;
  crypto_int32 g9_19 = 19 * g9;
  crypto_int32 f1_2 = 2 * f1;
  crypto_int32 f3_2 = 2 * f3;
  crypto_int32 f5_2 = 2 * f5;
  crypto_int32 f7_2 = 2 * f7;
  crypto_int32 f9_2 = 2 * f9;
  crypto_int64 f0g0    = f0   * (crypto_int64) g0;
  crypto_int64 f0g1    = f0   * (crypto_int64) g1;
  crypto_int64 f0g2    = f0   * (crypto_int64) g2;
  crypto_int64 f0g3    = f0   * (crypto_int64) g3;
  crypto_int64 f0g4    = f0   * (crypto_int64) g4;
  crypto_int64 f0g5    = f0   * (crypto_int64) g5;
  crypto_int64 f0g6    = f0   * (crypto_int64) g6;
  crypto_int64 f0g7    = f0   * (crypto_int64) g7;
  crypto_int64 f0g8    = f0   * (crypto_int64) g8;
  crypto_int64 f0g9    = f0   * (crypto_int64) g9;
  crypto_int64 f1g0    = f1   * (crypto_int64) g0;
  crypto_int64 f1g1_2  = f1_2 * (crypto_int64) g1;
  crypto_int64 f1g2    = f1   * (crypto_int64) g2;
  crypto_int64 f1g3_2  = f1_2 * (crypto_int64) g3;
  crypto_int64 f1g4    = f1   * (crypto_int64) g4;
  crypto_int64 f1g5_2  = f1_2 * (crypto_int64) g5;
  crypto_int64 f1g6    = f1   * (crypto_int64) g6;
  crypto_int64 f1g7_2  = f1_2 * (crypto_int64) g7;
  crypto_int64 f1g8    = f1   * (crypto_int64) g8;
  crypto_int64 f1g9_38 = f1_2 * (crypto_int64) g9_19;
  crypto_int64 f2g0    = f2   * (crypto_int64) g0;
  crypto_int64 f2g1    = f2   * (crypto_int64) g1;
  crypto_int64 f2g2    = f2   * (crypto_int64) g2;
  crypto_int64 f2g3    = f2   * (crypto_int64) g3;
  crypto_int64 f2g4    = f2   * (crypto_int64) g4;
  crypto_int64 f2g5    = f2   * (crypto_int64) g5;
  crypto_int64 f2g6    = f2   * (crypto_int64) g6;
  crypto_int64 f2g7    = f2   * (crypto_int64) g7;
  crypto_int64 f2g8_19 = f2   * (crypto_int64) g8_19;
  crypto_int64 f2g9_19 = f2   * (crypto_int64) g9_19;
  crypto_int64 f3g0    = f3   * (crypto_int64) g0;
  crypto_int64 f3g1_2  = f3_2 * (crypto_int64) g1;
  crypto_int64 f3g2    = f3   * (crypto_int64) g2;
  crypto_int64 f3g3_2  = f3_2 * (crypto_int64) g3;
  crypto_int64 f3g4    = f3   * (crypto_int64) g4;
  crypto_int64 f3g5_2  = f3_2 * (crypto_int64) g5;
  crypto_int64 f3g6    = f3   * (crypto_int64) g6;
  crypto_int64 f3g7_38 = f3_2 * (crypto_int64) g7_19;
  crypto_int64 f3g8_19 = f3   * (crypto_int64) g8_19;
  crypto_int64 f3g9_38 = f3_2 * (crypto_int64) g9_19;
  crypto_int64 f4g0    = f4   * (crypto_int64) g0;
  crypto_int64 f4g1    = f4   * (crypto_int64) g1;
  crypto_int64 f4g2    = f4   * (crypto_int64) g2;
  crypto_int64 f4g3    = f4   * (crypto_int64) g3;
  crypto_int64 f4g4    = f4   * (crypto_int64) g4;
  crypto_int64 f4g5    = f4   * (crypto_int64) g5;
  crypto_int64 f4g6_19 = f4   * (crypto_int64) g6_19;
  crypto_int64 f4g7_19 = f4   * (crypto_int64) g7_19;
  crypto_int64 f4g8_19 = f4   * (crypto_int64) g8_19;
  crypto_int64 f4g9_19 = f4   * (crypto_int64) g9_19;
  crypto_int64 f5g0    = f5   * (crypto_int64) g0;
  crypto_int64 f5g1_2  = f5_2 * (crypto_int64) g1;
  crypto_int64 f5g2    = f5   * (crypto_int64) g2;
  crypto_int64 f5g3_2  = f5_2 * (crypto_int64) g3;
  crypto_int64 f5g4    = f5   * (crypto_int64) g4;
  crypto_int64 f5g5_38 = f5_2 * (crypto_int64) g5_19;
  crypto_int64 f5g6_19 = f5   * (crypto_int64) g6_19;
  crypto_int64 f5g7_38 = f5_2 * (crypto_int64) g7_19;
  crypto_int64 f5g8_19 = f5   * (crypto_int64) g8_19;
  crypto_int64 f5g9_38 = f5_2 * (crypto_int64) g9_19;
  crypto_int64 f6g0    = f6   * (crypto_int64) g0;
  crypto_int64 f6g1    = f6   * (crypto_int64) g1;
  crypto_int64 f6g2    = f6   * (crypto_int64) g2;
  crypto_int64 f6g3    = f6   * (crypto_int64) g3;
  crypto_int64 f6g4_19 = f6   * (crypto_int64) g4_19;
  crypto_int64 f6g5_19 = f6   * (crypto_int64) g5_19;
  crypto_int64 f6g6_19 = f6   * (crypto_int64) g6_19;
  crypto_int64 f6g7_19 = f6   * (crypto_int64) g7_19;
  crypto_int64 f6g8_19 = f6   * (crypto_int64) g8_19;
  crypto_int64 f6g9_19 = f6   * (crypto_int64) g9_19;
  crypto_int64 f7g0    = f7   * (crypto_int64) g0;
  crypto_int64 f7g1_2  = f7_2 * (crypto_int64) g1;
  crypto_int64 f7g2    = f7   * (crypto_int64) g2;
  crypto_int64 f7g3_38 = f7_2 * (crypto_int64) g3_19;
  crypto_int64 f7g4_19 = f7   * (crypto_int64) g4_19;
  crypto_int64 f7g5_38 = f7_2 * (crypto_int64) g5_19;
  crypto_int64 f7g6_19 = f7   * (crypto_int64) g6_19;
  crypto_int64 f7g7_38 = f7_2 * (crypto_int64) g7_19;
  crypto_int64 f7g8_19 = f7   * (crypto_int64) g8_19;
  crypto_int64 f7g9_38 = f7_2 * (crypto_int64) g9_19;
  crypto_int64 f8g0    = f8   * (crypto_int64) g0;
  crypto_int64 f8g1    = f8   * (crypto_int64) g1;
  crypto_int64 f8g2_19 = f8   * (crypto_int64) g2_19;
  crypto_int64 f8g3_19 = f8   * (crypto_int64) g3_19;
  crypto_int64 f8g4_19 = f8   * (crypto_int64) g4_19;
  crypto_int64 f8g5_19 = f8   * (crypto_int64) g5_19;
  crypto_int64 f8g6_19 = f8   * (crypto_int64) g6_19;
  crypto_int64 f8g7_19 = f8   * (crypto_int64) g7_19;
  crypto_int64 f8g8_19 = f8   * (crypto_int64) g8_19;
  crypto_int64 f8g9_19 = f8   * (crypto_int64) g9_19;
  crypto_int64 f9g0    = f9   * (crypto_int64) g0;
  crypto_int64 f9g1_38 = f9_2 * (crypto_int64) g1_19;
  crypto_int64 f9g2_19 = f9   * (crypto_int64) g2_19;
  crypto_int64 f9g3_38 = f9_2 * (crypto_int64) g3_19;
  crypto_int64 f9g4_19 = f9   * (crypto_int64) g4_19;
  crypto_int64 f9g5_38 = f9_2 * (crypto_int64) g5_19;
  crypto_int64 f9g6_19 = f9   * (crypto_int64) g6_19;
  crypto_int64 f9g7_38 = f9_2 * (crypto_int64) g7_19;
  crypto_int64 f9g8_19 = f9   * (crypto_int64) g8_19;
  crypto_int64 f9g9_38 = f9_2 * (crypto_int64) g9_19;
  crypto_int64 h0 = f0g0+f1g9_38+f2g8_19+f3g7_38+f4g6_19+f5g5_38+f6g4_19+f7g3_38+f8g2_19+f9g1_38;
  crypto_int64 h1 = f0g1+f1g0   +f2g9_19+f3g8_19+f4g7_19+f5g6_19+f6g5_19+f7g4_19+f8g3_19+f9g2_19;
  crypto_int64 h2 = f0g2+f1g1_2 +f2g0   +f3g9_38+f4g8_19+f5g7_38+f6g6_19+f7g5_38+f8g4_19+f9g3_38;
  crypto_int64 h3 = f0g3+f1g2   +f2g1   +f3g0   +f4g9_19+f5g8_19+f6g7_19+f7g6_19+f8g5_19+f9g4_19;
  crypto_int64 h4 = f0g4+f1g3_2 +f2g2   +f3g1_2 +f4g0   +f5g9_38+f6g8_19+f7g7_38+f8g6_19+f9g5_38;
  crypto_int64 h5 = f0g5+f1g4   +f2g3   +f3g2   +f4g1   +f5g0   +f6g9_19+f7g8_19+f8g7_19+f9g6_19;
  crypto_int64 h6 = f0g6+f1g5_2 +f2g4   +f3g3_2 +f4g2   +f5g1_2 +f6g0   +f7g9_38+f8g8_19+f9g7_38;
  crypto_int64 h7 = f0g7+f1g6   +f2g5   +f3g4   +f4g3   +f5g2   +f6g1   +f7g0   +f8g9_19+f9g8_19;
  crypto_int64 h8 = f0g8+f1g7_2 +f2g6   +f3g5_2 +f4g4   +f5g3_2 +f6g2   +f7g1_2 +f8g0   +f9g9_38;
  crypto_int64 h9 = f0g9+f1g8   +f2g7   +f3g6   +f4g5   +f5g4   +f6g3   +f7g2   +f8g1   +f9g0   ;
  crypto_int64 carry0;
  crypto_int64 carry1;
  crypto_int64 carry2;
  crypto_int64 carry3;
  crypto_int64 carry4;
  crypto_int64 carry5;
  crypto_int64 carry6;
  crypto_int64 carry7;
  crypto_int64 carry8;
  crypto_int64 carry9;

  /*
  |h0| <= (1.65*1.65*2^52*(1+19+19+19+19)+1.65*1.65*2^50*(38+38+38+38+38))
    i.e. |h0| <= 1.4*2^60; narrower ranges for h2, h4, h6, h8
  |h1| <= (1.65*1.65*2^51*(1+1+19+19+19+19+19+19+19+19))
    i.e. |h1| <= 1.7*2^59; narrower ranges for h3, h5, h7, h9
  */

  carry0 = (h0 + (crypto_int64) (1L << 25)) >> 26; h1 += carry0; h0 -= carry0 * (1L << 26);
  carry4 = (h4 + (crypto_int64) (1L << 25)) >> 26; h5 += carry4; h4 -= carry4 * (1L << 26);
  /* |h0| <= 2^25 */
  /* |h4| <= 2^25 */
  /* |h1| <= 1.71*2^59 */
  /* |h5| <= 1.71*2^59 */

  carry1 = (h1 + (crypto_int64) (1L << 24)) >> 25; h2 += carry1; h1 -= carry1 * (1L << 25);
  carry5 = (h5 + (crypto_int64) (1L << 24)) >> 25; h6 += carry5; h5 -= carry5 * (1L << 25);
  /* |h1| <= 2^24; from now on fits into int32 */
  /* |h5| <= 2^24; from now on fits into int32 */
  /* |h2| <= 1.41*2^60 */
  /* |h6| <= 1.41*2^60 */

  carry2 = (h2 + (crypto_int64) (1L << 25)) >> 26; h3 += carry2; h2 -= carry2 * (1L << 26);
  carry6 = (h6 + (crypto_int64) (1L << 25)) >> 26; h7 += carry6; h6 -= carry6 * (1L << 26);
  /* |h2| <= 2^25; from now on fits into int32 unchanged */
  /* |h6| <= 2^25; from now on fits into int32 unchanged */
  /* |h3| <= 1.71*2^59 */
  /* |h7| <= 1.71*2^59 */

  carry3 = (h3 + (crypto_int64) (1L << 24)) >> 25; h4 += carry3; h3 -= carry3 * (1L << 25);
  carry7 = (h7 + (crypto_int64) (1L << 24)) >> 25; h8 += carry7; h7 -= carry7 * (1L << 25);
  /* |h3| <= 2^24; from now on fits into int32 unchanged */
  /* |h7| <= 2^24; from now on fits into int32 unchanged */
  /* |h4| <= 1.72*2^34 */
  /* |h8| <= 1.41*2^60 */

  carry4 = (h4 + (crypto_int64) (1L << 25)) >> 26; h5 += carry4; h4 -= carry4 * (1L << 26);
  carry8 = (h8 + (crypto_int64) (1L << 25)) >> 26; h9 += carry8; h8 -= carry8 * (1L << 26);
  /* |h4| <= 2^25; from now on fits into int32 unchanged */
  /* |h8| <= 2^25; from now on fits into int32 unchanged */
  /* |h5| <= 1.01*2^24 */
  /* |h9| <= 1.71*2^59 */

  carry9 = (h9 + (crypto_int64) (1L << 24)) >> 25; h0 += carry9 * 19; h9 -= carry9 * (1L << 25);
  /* |h9| <= 2^24; from now on fits into int32 unchanged */
  /* |h0| <= 1.1*2^39 */

  carry0 = (h0 + (crypto_int64) (1L << 25)) >> 26; h1 += carry0; h0 -= carry0 * (1L << 26);
  /* |h0| <= 2^25; from now on fits into int32 unchanged */
  /* |h1| <= 1.01*2^24 */

  h[0] = (crypto_int32) h0;
  h[1] = (crypto_int32) h1;
  h[2] = (crypto_int32) h2;
  h[3] = (crypto_int32) h3;
  h[4] = (crypto_int32) h4;
  h[5] = (crypto_int32) h5;
  h[6] = (crypto_int32) h6;
  h[7] = (crypto_int32) h7;
  h[8] = (crypto_int32) h8;
  h[9] = (crypto_int32) h9;
}

/*
h = -f

Preconditions:
   |f| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.

Postconditions:
   |h| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
*/

void fe_neg(fe h,const fe f)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  crypto_int32 h0 = -f0;
  crypto_int32 h1 = -f1;
  crypto_int32 h2 = -f2;
  crypto_int32 h3 = -f3;
  crypto_int32 h4 = -f4;
  crypto_int32 h5 = -f5;
  crypto_int32 h6 = -f6;
  crypto_int32 h7 = -f7;
  crypto_int32 h8 = -f8;
  crypto_int32 h9 = -f9;
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

/*
h = f * f
Can overlap h with f.

Preconditions:
   |f| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.

Postconditions:
   |h| bounded by 1.01*2^25,1.01*2^24,1.01*2^25,1.01*2^24,etc.
*/

/*
See fe_mul.c for discussion of implementation strategy.
*/

void fe_sq(fe h,const fe f)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  crypto_int32 f0_2 = 2 * f0;
  crypto_int32 f1_2 = 2 * f1;
  crypto_int32 f2_2 = 2 * f2;
  crypto_int32 f3_2 = 2 * f3;
  crypto_int32 f4_2 = 2 * f4;
  crypto_int32 f5_2 = 2 * f5;
  crypto_int32 f6_2 = 2 * f6;
  crypto_int32 f7_2 = 2 * f7;
  crypto_int32 f5_38 = 38 * f5; /* 1.959375*2^30 */
  crypto_int32 f6_19 = 19 * f6; /* 1.959375*2^30 */
  crypto_int32 f7_38 = 38 * f7; /* 1.959375*2^30 */
  crypto_int32 f8_19 = 19 * f8; /* 1.959375*2^30 */
  crypto_int32 f9_38 = 38 * f9; /* 1.959375*2^30 */
  crypto_int64 f0f0    = f0   * (crypto_int64) f0;
  crypto_int64 f0f1_2  = f0_2 * (crypto_int64) f1;
  crypto_int64 f0f2_2  = f0_2 * (crypto_int64) f2;
  crypto_int64 f0f3_2  = f0_2 * (crypto_int64) f3;
  crypto_int64 f0f4_2  = f0_2 * (crypto_int64) f4;
  crypto_int64 f0f5_2  = f0_2 * (crypto_int64) f5;
  crypto_int64 f0f6_2  = f0_2 * (crypto_int64) f6;
  crypto_int64 f0f7_2  = f0_2 * (crypto_int64) f7;
  crypto_int64 f0f8_2  = f0_2 * (crypto_int64) f8;
  crypto_int64 f0f9_2  = f0_2 * (crypto_int64) f9;
  crypto_int64 f1f1_2  = f1_2 * (crypto_int64) f1;
  crypto_int64 f1f2_2  = f1_2 * (crypto_int64) f2;
  crypto_int64 f1f3_4  = f1_2 * (crypto_int64) f3_2;
  crypto_int64 f1f4_2  = f1_2 * (crypto_int64) f4;
  crypto_int64 f1f5_4  = f1_2 * (crypto_int64) f5_2;
  crypto_int64 f1f6_2  = f1_2 * (crypto_int64) f6;
  crypto_int64 f1f7_4  = f1_2 * (crypto_int64) f7_2;
  crypto_int64 f1f8_2  = f1_2 * (crypto_int64) f8;
  crypto_int64 f1f9_76 = f1_2 * (crypto_int64) f9_38;
  crypto_int64 f2f2    = f2   * (crypto_int64) f2;
  crypto_int64 f2f3_2  = f2_2 * (crypto_int64) f3;
  crypto_int64 f2f4_2  = f2_2 * (crypto_int64) f4;
  crypto_int64 f2f5_2  = f2_2 * (crypto_int64) f5;
  crypto_int64 f2f6_2  = f2_2 * (crypto_int64) f6;
  crypto_int64 f2f7_2  = f2_2 * (crypto_int64) f7;
  crypto_int64 f2f8_38 = f2_2 * (crypto_int64) f8_19;
  crypto_int64 f2f9_38 = f2   * (crypto_int64) f9_38;
  crypto_int64 f3f3_2  = f3_2 * (crypto_int64) f3;
  crypto_int64 f3f4_2  = f3_2 * (crypto_int64) f4;
  crypto_int64 f3f5_4  = f3_2 * (crypto_int64) f5_2;
  crypto_int64 f3f6_2  = f3_2 * (crypto_int64) f6;
  crypto_int64 f3f7_76 = f3_2 * (crypto_int64) f7_38;
  crypto_int64 f3f8_38 = f3_2 * (crypto_int64) f8_19;
  crypto_int64 f3f9_76 = f3_2 * (crypto_int64) f9_38;
  crypto_int64 f4f4    = f4   * (crypto_int64) f4;
  crypto_int64 f4f5_2  = f4_2 * (crypto_int64) f5;
  crypto_int64 f4f6_38 = f4_2 * (crypto_int64) f6_19;
  crypto_int64 f4f7_38 = f4   * (crypto_int64) f7_38;
  crypto_int64 f4f8_38 = f4_2 * (crypto_int64) f8_19;
  crypto_int64 f4f9_38 = f4   * (crypto_int64) f9_38;
  crypto_int64 f5f5_38 = f5   * (crypto_int64) f5_38;
  crypto_int64 f5f6_38 = f5_2 * (crypto_int64) f6_19;
  crypto_int64 f5f7_76 = f5_2 * (crypto_int64) f7_38;
  crypto_int64 f5f8_38 = f5_2 * (crypto_int64) f8_19;
  crypto_int64 f5f9_76 = f5_2 * (crypto_int64) f9_38;
  crypto_int64 f6f6_19 = f6   * (crypto_int64) f6_19;
  crypto_int64 f6f7_38 = f6   * (crypto_int64) f7_38;
  crypto_int64 f6f8_38 = f6_2 * (crypto_int64) f8_19;
  crypto_int64 f6f9_38 = f6   * (crypto_int64) f9_38;
  crypto_int64 f7f7_38 = f7   * (crypto_int64) f7_38;
  crypto_int64 f7f8_38 = f7_2 * (crypto_int64) f8_19;
  crypto_int64 f7f9_76 = f7_2 * (crypto_int64) f9_38;
  crypto_int64 f8f8_19 = f8   * (crypto_int64) f8_19;
  crypto_int64 f8f9_38 = f8   * (crypto_int64) f9_38;
  crypto_int64 f9f9_38 = f9   * (crypto_int64) f9_38;
  crypto_int64 h0 = f0f0  +f1f9_76+f2f8_38+f3f7_76+f4f6_38+f5f5_38;
  crypto_int64 h1 = f0f1_2+f2f9_38+f3f8_38+f4f7_38+f5f6_38;
  crypto_int64 h2 = f0f2_2+f1f1_2 +f3f9_76+f4f8_38+f5f7_76+f6f6_19;
  crypto_int64 h3 = f0f3_2+f1f2_2 +f4f9_38+f5f8_38+f6f7_38;
  crypto_int64 h4 = f0f4_2+f1f3_4 +f2f2   +f5f9_76+f6f8_38+f7f7_38;
  crypto_int64 h5 = f0f5_2+f1f4_2 +f2f3_2 +f6f9_38+f7f8_38;
  crypto_int64 h6 = f0f6_2+f1f5_4 +f2f4_2 +f3f3_2 +f7f9_76+f8f8_19;
  crypto_int64 h7 = f0f7_2+f1f6_2 +f2f5_2 +f3f4_2 +f8f9_38;
  crypto_int64 h8 = f0f8_2+f1f7_4 +f2f6_2 +f3f5_4 +f4f4   +f9f9_38;
  crypto_int64 h9 = f0f9_2+f1f8_2 +f2f7_2 +f3f6_2 +f4f5_2;
  crypto_int64 carry0;
  crypto_int64 carry1;
  crypto_int64 carry2;
  crypto_int64 carry3;
  crypto_int64 carry4;
  crypto_int64 carry5;
  crypto_int64 carry6;
  crypto_int64 carry7;
  crypto_int64 carry8;
  crypto_int64 carry9;

  carry0 = (h0 + (crypto_int64) (1L << 25)) >> 26; h1 += carry0; h0 -= carry0 * (1L << 26);
  carry4 = (h4 + (crypto_int64) (1L << 25)) >> 26; h5 += carry4; h4 -= carry4 * (1L << 26);

  carry1 = (h1 + (crypto_int64) (1L << 24)) >> 25; h2 += carry1; h1 -= carry1 * (1L << 25);
  carry5 = (h5 + (crypto_int64) (1L << 24)) >> 25; h6 += carry5; h5 -= carry5 * (1L << 25);

  carry2 = (h2 + (crypto_int64) (1L << 25)) >> 26; h3 += carry2; h2 -= carry2 * (1L << 26);
  carry6 = (h6 + (crypto_int64) (1L << 25)) >> 26; h7 += carry6; h6 -= carry6 * (1L << 26);

  carry3 = (h3 + (crypto_int64) (1L << 24)) >> 25; h4 += carry3; h3 -= carry3 * (1L << 25);
  carry7 = (h7 + (crypto_int64) (1L << 24)) >> 25; h8 += carry7; h7 -= carry7 * (1L << 25);

  carry4 = (h4 + (crypto_int64) (1L << 25)) >> 26; h5 += carry4; h4 -= carry4 * (1L << 26);
  carry8 = (h8 + (crypto_int64) (1L << 25)) >> 26; h9 += carry8; h8 -= carry8 * (1L << 26);

  carry9 = (h9 + (crypto_int64) (1L << 24)) >> 25; h0 += carry9 * 19; h9 -= carry9 * (1L << 25);

  carry0 = (h0 + (crypto_int64) (1L << 25)) >> 26; h1 += carry0; h0 -= carry0 * (1L << 26);

  h[0] = (crypto_int32) h0;
  h[1] = (crypto_int32) h1;
  h[2] = (crypto_int32) h2;
  h[3] = (crypto_int32) h3;
  h[4] = (crypto_int32) h4;
  h[5] = (crypto_int32) h5;
  h[6] = (crypto_int32) h6;
  h[7] = (crypto_int32) h7;
  h[8] = (crypto_int32) h8;
  h[9] = (crypto_int32) h9;
}

/*
h = 2 * f * f
Can overlap h with f.

Preconditions:
   |f| bounded by 1.65*2^26,1.65*2^25,1.65*2^26,1.65*2^25,etc.

Postconditions:
   |h| bounded by 1.01*2^25,1.01*2^24,1.01*2^25,1.01*2^24,etc.
*/

/*
See fe_mul.c for discussion of implementation strategy.
*/

void fe_sq2(fe h,const fe f)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  crypto_int32 f0_2 = 2 * f0;
  crypto_int32 f1_2 = 2 * f1;
  crypto_int32 f2_2 = 2 * f2;
  crypto_int32 f3_2 = 2 * f3;
  crypto_int32 f4_2 = 2 * f4;
  crypto_int32 f5_2 = 2 * f5;
  crypto_int32 f6_2 = 2 * f6;
  crypto_int32 f7_2 = 2 * f7;
  crypto_int32 f5_38 = 38 * f5; /* 1.959375*2^30 */
  crypto_int32 f6_19 = 19 * f6; /* 1.959375*2^30 */
  crypto_int32 f7_38 = 38 * f7; /* 1.959375*2^30 */
  crypto_int32 f8_19 = 19 * f8; /* 1.959375*2^30 */
  crypto_int32 f9_38 = 38 * f9; /* 1.959375*2^30 */
  crypto_int64 f0f0    = f0   * (crypto_int64) f0;
  crypto_int64 f0f1_2  = f0_2 * (crypto_int64) f1;
  crypto_int64 f0f2_2  = f0_2 * (crypto_int64) f2;
  crypto_int64 f0f3_2  = f0_2 * (crypto_int64) f3;
  crypto_int64 f0f4_2  = f0_2 * (crypto_int64) f4;
  crypto_int64 f0f5_2  = f0_2 * (crypto_int64) f5;
  crypto_int64 f0f6_2  = f0_2 * (crypto_int64) f6;
  crypto_int64 f0f7_2  = f0_2 * (crypto_int64) f7;
  crypto_int64 f0f8_2  = f0_2 * (crypto_int64) f8;
  crypto_int64 f0f9_2  = f0_2 * (crypto_int64) f9;
  crypto_int64 f1f1_2  = f1_2 * (crypto_int64) f1;
  crypto_int64 f1f2_2  = f1_2 * (crypto_int64) f2;
  crypto_int64 f1f3_4  = f1_2 * (crypto_int64) f3_2;
  crypto_int64 f1f4_2  = f1_2 * (crypto_int64) f4;
  crypto_int64 f1f5_4  = f1_2 * (crypto_int64) f5_2;
  crypto_int64 f1f6_2  = f1_2 * (crypto_int64) f6;
  crypto_int64 f1f7_4  = f1_2 * (crypto_int64) f7_2;
  crypto_int64 f1f8_2  = f1_2 * (crypto_int64) f8;
  crypto_int64 f1f9_76 = f1_2 * (crypto_int64) f9_38;
  crypto_int64 f2f2    = f2   * (crypto_int64) f2;
  crypto_int64 f2f3_2  = f2_2 * (crypto_int64) f3;
  crypto_int64 f2f4_2  = f2_2 * (crypto_int64) f4;
  crypto_int64 f2f5_2  = f2_2 * (crypto_int64) f5;
  crypto_int64 f2f6_2  = f2_2 * (crypto_int64) f6;
  crypto_int64 f2f7_2  = f2_2 * (crypto_int64) f7;
  crypto_int64 f2f8_38 = f2_2 * (crypto_int64) f8_19;
  crypto_int64 f2f9_38 = f2   * (crypto_int64) f9_38;
  crypto_int64 f3f3_2  = f3_2 * (crypto_int64) f3;
  crypto_int64 f3f4_2  = f3_2 * (crypto_int64) f4;
  crypto_int64 f3f5_4  = f3_2 * (crypto_int64) f5_2;
  crypto_int64 f3f6_2  = f3_2 * (crypto_int64) f6;
  crypto_int64 f3f7_76 = f3_2 * (crypto_int64) f7_38;
  crypto_int64 f3f8_38 = f3_2 * (crypto_int64) f8_19;
  crypto_int64 f3f9_76 = f3_2 * (crypto_int64) f9_38;
  crypto_int64 f4f4    = f4   * (crypto_int64) f4;
  crypto_int64 f4f5_2  = f4_2 * (crypto_int64) f5;
  crypto_int64 f4f6_38 = f4_2 * (crypto_int64) f6_19;
  crypto_int64 f4f7_38 = f4   * (crypto_int64) f7_38;
  crypto_int64 f4f8_38 = f4_2 * (crypto_int64) f8_19;
  crypto_int64 f4f9_38 = f4   * (crypto_int64) f9_38;
  crypto_int64 f5f5_38 = f5   * (crypto_int64) f5_38;
  crypto_int64 f5f6_38 = f5_2 * (crypto_int64) f6_19;
  crypto_int64 f5f7_76 = f5_2 * (crypto_int64) f7_38;
  crypto_int64 f5f8_38 = f5_2 * (crypto_int64) f8_19;
  crypto_int64 f5f9_76 = f5_2 * (crypto_int64) f9_38;
  crypto_int64 f6f6_19 = f6   * (crypto_int64) f6_19;
  crypto_int64 f6f7_38 = f6   * (crypto_int64) f7_38;
  crypto_int64 f6f8_38 = f6_2 * (crypto_int64) f8_19;
  crypto_int64 f6f9_38 = f6   * (crypto_int64) f9_38;
  crypto_int64 f7f7_38 = f7   * (crypto_int64) f7_38;
  crypto_int64 f7f8_38 = f7_2 * (crypto_int64) f8_19;
  crypto_int64 f7f9_76 = f7_2 * (crypto_int64) f9_38;
  crypto_int64 f8f8_19 = f8   * (crypto_int64) f8_19;
  crypto_int64 f8f9_38 = f8   * (crypto_int64) f9_38;
  crypto_int64 f9f9_38 = f9   * (crypto_int64) f9_38;
  crypto_int64 h0 = f0f0  +f1f9_76+f2f8_38+f3f7_76+f4f6_38+f5f5_38;
  crypto_int64 h1 = f0f1_2+f2f9_38+f3f8_38+f4f7_38+f5f6_38;
  crypto_int64 h2 = f0f2_2+f1f1_2 +f3f9_76+f4f8_38+f5f7_76+f6f6_19;
  crypto_int64 h3 = f0f3_2+f1f2_2 +f4f9_38+f5f8_38+f6f7_38;
  crypto_int64 h4 = f0f4_2+f1f3_4 +f2f2   +f5f9_76+f6f8_38+f7f7_38;
  crypto_int64 h5 = f0f5_2+f1f4_2 +f2f3_2 +f6f9_38+f7f8_38;
  crypto_int64 h6 = f0f6_2+f1f5_4 +f2f4_2 +f3f3_2 +f7f9_76+f8f8_19;
  crypto_int64 h7 = f0f7_2+f1f6_2 +f2f5_2 +f3f4_2 +f8f9_38;
  crypto_int64 h8 = f0f8_2+f1f7_4 +f2f6_2 +f3f5_4 +f4f4   +f9f9_38;
  crypto_int64 h9 = f0f9_2+f1f8_2 +f2f7_2 +f3f6_2 +f4f5_2;
  crypto_int64 carry0;
  crypto_int64 carry1;
  crypto_int64 carry2;
  crypto_int64 carry3;
  crypto_int64 carry4;
  crypto_int64 carry5;
  crypto_int64 carry6;
  crypto_int64 carry7;
  crypto_int64 carry8;
  crypto_int64 carry9;

  h0 += h0;
  h1 += h1;
  h2 += h2;
  h3 += h3;
  h4 += h4;
  h5 += h5;
  h6 += h6;
  h7 += h7;
  h8 += h8;
  h9 += h9;

  carry0 = (h0 + (crypto_int64) (1L << 25)) >> 26; h1 += carry0; h0 -= carry0 * (1L << 26);
  carry4 = (h4 + (crypto_int64) (1L << 25)) >> 26; h5 += carry4; h4 -= carry4 * (1L << 26);

  carry1 = (h1 + (crypto_int64) (1L << 24)) >> 25; h2 += carry1; h1 -= carry1 * (1L << 25);
  carry5 = (h5 + (crypto_int64) (1L << 24)) >> 25; h6 += carry5; h5 -= carry5 * (1L << 25);

  carry2 = (h2 + (crypto_int64) (1L << 25)) >> 26; h3 += carry2; h2 -= carry2 * (1L << 26);
  carry6 = (h6 + (crypto_int64) (1L << 25)) >> 26; h7 += carry6; h6 -= carry6 * (1L << 26);

  carry3 = (h3 + (crypto_int64) (1L << 24)) >> 25; h4 += carry3; h3 -= carry3 * (1L << 25);
  carry7 = (h7 + (crypto_int64) (1L << 24)) >> 25; h8 += carry7; h7 -= carry7 * (1L << 25);

  carry4 = (h4 + (crypto_int64) (1L << 25)) >> 26; h5 += carry4; h4 -= carry4 * (1L << 26);
  carry8 = (h8 + (crypto_int64) (1L << 25)) >> 26; h9 += carry8; h8 -= carry8 * (1L << 26);

  carry9 = (h9 + (crypto_int64) (1L << 24)) >> 25; h0 += carry9 * 19; h9 -= carry9 * (1L << 25);

  carry0 = (h0 + (crypto_int64) (1L << 25)) >> 26; h1 += carry0; h0 -= carry0 * (1L << 26);

  h[0] = (crypto_int32) h0;
  h[1] = (crypto_int32) h1;
  h[2] = (crypto_int32) h2;
  h[3] = (crypto_int32) h3;
  h[4] = (crypto_int32) h4;
  h[5] = (crypto_int32) h5;
  h[6] = (crypto_int32) h6;
  h[7] = (crypto_int32) h7;
  h[8] = (crypto_int32) h8;
  h[9] = (crypto_int32) h9;
}

void fe_invert(fe out,const fe z)
{
    fe t0;
    fe t1;
    fe t2;
    fe t3;
    int i;

    fe_sq(t0, z);
    for (i = 1; i < 1; ++i) {
        fe_sq(t0, t0);
    }
    fe_sq(t1, t0);
    for (i = 1; i < 2; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t1, z, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t2, t0);
    for (i = 1; i < 1; ++i) {
        fe_sq(t2, t2);
    }
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

void fe_pow22523(fe out,const fe z)
{
    fe t0;
    fe t1;
    fe t2;
    int i;

    fe_sq(t0, z);
    for (i = 1; i < 1; ++i) {
        fe_sq(t0, t0);
    }
    fe_sq(t1, t0);
    for (i = 1; i < 2; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t1, z, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t0, t0);
    for (i = 1; i < 1; ++i) {
        fe_sq(t0, t0);
    }
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
    for (i = 1; i < 2; ++i) {
        fe_sq(t0, t0);
    }
    fe_mul(out, t0, z);
}

/*
h = f - g
Can overlap h with f or g.

Preconditions:
   |f| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.
   |g| bounded by 1.1*2^25,1.1*2^24,1.1*2^25,1.1*2^24,etc.

Postconditions:
   |h| bounded by 1.1*2^26,1.1*2^25,1.1*2^26,1.1*2^25,etc.
*/

void fe_sub(fe h,const fe f,const fe g)
{
  crypto_int32 f0 = f[0];
  crypto_int32 f1 = f[1];
  crypto_int32 f2 = f[2];
  crypto_int32 f3 = f[3];
  crypto_int32 f4 = f[4];
  crypto_int32 f5 = f[5];
  crypto_int32 f6 = f[6];
  crypto_int32 f7 = f[7];
  crypto_int32 f8 = f[8];
  crypto_int32 f9 = f[9];
  crypto_int32 g0 = g[0];
  crypto_int32 g1 = g[1];
  crypto_int32 g2 = g[2];
  crypto_int32 g3 = g[3];
  crypto_int32 g4 = g[4];
  crypto_int32 g5 = g[5];
  crypto_int32 g6 = g[6];
  crypto_int32 g7 = g[7];
  crypto_int32 g8 = g[8];
  crypto_int32 g9 = g[9];
  crypto_int32 h0 = f0 - g0;
  crypto_int32 h1 = f1 - g1;
  crypto_int32 h2 = f2 - g2;
  crypto_int32 h3 = f3 - g3;
  crypto_int32 h4 = f4 - g4;
  crypto_int32 h5 = f5 - g5;
  crypto_int32 h6 = f6 - g6;
  crypto_int32 h7 = f7 - g7;
  crypto_int32 h8 = f8 - g8;
  crypto_int32 h9 = f9 - g9;
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

/*
r = p + q
*/

void ge_add(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q)
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

static void slide(signed char *r,const unsigned char *a)
{
  int i;
  int b;
  int k;

  for (i = 0;i < 256;++i)
    r[i] = 1 & (a[i >> 3] >> (i & 7));

  for (i = 0;i < 256;++i)
    if (r[i]) {
      for (b = 1;b <= 6 && i + b < 256;++b) {
        if (r[i + b]) {
          if (r[i] + (r[i + b] * (1 << b)) <= 15) {
            r[i] += r[i + b] << b; r[i + b] = 0;
          } else if (r[i] - (r[i + b] * (1 << b)) >= -15) {
            r[i] -= r[i + b] << b;
            for (k = i + b;k < 256;++k) {
              if (!r[k]) {
                r[k] = 1;
                break;
              }
              r[k] = 0;
            }
          } else
            break;
        }
      }
    }

}

static ge_precomp Bi[8] = {
#include "base2.h"
} ;

static const fe d = {
    -10913610,13857413,-15372611,6949391,114729,-8787816,-6275908,-3247719,-18696448,-12055116
};

static const fe sqrtm1 = {
    -32595792,-7943725,9377950,3500415,12389472,-272473,-25146209,-2005654,326686,11406482
};

int ge_frombytes_negate_vartime(ge_p3 *h,const unsigned char *s)
{
  fe u;
  fe v;
  fe v3;
  fe vxx;
  fe check;

  fe_frombytes(h->Y,s);
  fe_1(h->Z);
  fe_sq(u,h->Y);
  fe_mul(v,u,d);
  fe_sub(u,u,h->Z);       /* u = y^2-1 */
  fe_add(v,v,h->Z);       /* v = dy^2+1 */

  fe_sq(v3,v);
  fe_mul(v3,v3,v);        /* v3 = v^3 */
  fe_sq(h->X,v3);
  fe_mul(h->X,h->X,v);
  fe_mul(h->X,h->X,u);    /* x = uv^7 */

  fe_pow22523(h->X,h->X); /* x = (uv^7)^((q-5)/8) */
  fe_mul(h->X,h->X,v3);
  fe_mul(h->X,h->X,u);    /* x = uv^3(uv^7)^((q-5)/8) */

  fe_sq(vxx,h->X);
  fe_mul(vxx,vxx,v);
  fe_sub(check,vxx,u);    /* vx^2-u */
  if (fe_isnonzero(check)) {
    fe_add(check,vxx,u);  /* vx^2+u */
    if (fe_isnonzero(check)) return -1;
    fe_mul(h->X,h->X,sqrtm1);
  }

  if (fe_isnegative(h->X) == (s[31] >> 7))
    fe_neg(h->X,h->X);

  fe_mul(h->T,h->X,h->Y);
  return 0;
}

/*
r = p + q
*/

void ge_madd(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q)
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

void ge_msub(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q)
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

extern void ge_p1p1_to_p2(ge_p2 *r,const ge_p1p1 *p)
{
  fe_mul(r->X,p->X,p->T);
  fe_mul(r->Y,p->Y,p->Z);
  fe_mul(r->Z,p->Z,p->T);
}

/*
r = p
*/

extern void ge_p1p1_to_p3(ge_p3 *r,const ge_p1p1 *p)
{
  fe_mul(r->X,p->X,p->T);
  fe_mul(r->Y,p->Y,p->Z);
  fe_mul(r->Z,p->Z,p->T);
  fe_mul(r->T,p->X,p->Y);
}

void ge_p2_0(ge_p2 *h)
{
  fe_0(h->X);
  fe_1(h->Y);
  fe_1(h->Z);
}

/*
r = 2 * p
*/

void ge_p2_dbl(ge_p1p1 *r,const ge_p2 *p)
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

void ge_p3_0(ge_p3 *h)
{
  fe_0(h->X);
  fe_1(h->Y);
  fe_1(h->Z);
  fe_0(h->T);
}

/*
r = p
*/

static const fe d2 = {
    -21827239,-5839606,-30745221,13898782,229458,15978800,-12551817,-6495438,29715968,9444199
};

extern void ge_p3_to_cached(ge_cached *r,const ge_p3 *p)
{
  fe_add(r->YplusX,p->Y,p->X);
  fe_sub(r->YminusX,p->Y,p->X);
  fe_copy(r->Z,p->Z);
  fe_mul(r->T2d,p->T,d2);
}

/*
r = p
*/

extern void ge_p3_to_p2(ge_p2 *r,const ge_p3 *p)
{
  fe_copy(r->X,p->X);
  fe_copy(r->Y,p->Y);
  fe_copy(r->Z,p->Z);
}

void ge_p3_tobytes(unsigned char *s,const ge_p3 *h)
{
  fe recip;
  fe x;
  fe y;

  fe_invert(recip,h->Z);
  fe_mul(x,h->X,recip);
  fe_mul(y,h->Y,recip);
  fe_tobytes(s,y);
  s[31] ^= fe_isnegative(x) << 7;
}

/*
r = 2 * p
*/

void ge_p3_dbl(ge_p1p1 *r,const ge_p3 *p)
{
  ge_p2 q;
  ge_p3_to_p2(&q,p);
  ge_p2_dbl(r,&q);
}

void ge_precomp_0(ge_precomp *h)
{
  fe_1(h->yplusx);
  fe_1(h->yminusx);
  fe_0(h->xy2d);
}

static unsigned char equal(signed char b,signed char c)
{
  unsigned char ub = b;
  unsigned char uc = c;
  unsigned char x = ub ^ uc; /* 0: yes; 1..255: no */
  crypto_uint32 y = x; /* 0: yes; 1..255: no */
  y -= 1; /* 4294967295: yes; 0..254: no */
  y >>= 31; /* 1: yes; 0: no */
  return y;
}

static unsigned char negative(signed char b)
{
  crypto_uint64 x = b; /* 18446744073709551361..18446744073709551615: yes; 0..255: no */
  x >>= 63; /* 1: yes; 0: no */
  return x;
}

static void cmov(ge_precomp *t,ge_precomp *u,unsigned char b)
{
  fe_cmov(t->yplusx,u->yplusx,b);
  fe_cmov(t->yminusx,u->yminusx,b);
  fe_cmov(t->xy2d,u->xy2d,b);
}

/* base[i][j] = (j+1)*256^i*B */
static ge_precomp base[32][8] = {
#include "base.h"
} ;

static void ge_select(ge_precomp *t,int pos,signed char b)
{
  ge_precomp minust;
  unsigned char bnegative = negative(b);
  unsigned char babs = b - (((-bnegative) & b) << 1);

  ge_precomp_0(t);
  cmov(t,&base[pos][0],equal(babs,1));
  cmov(t,&base[pos][1],equal(babs,2));
  cmov(t,&base[pos][2],equal(babs,3));
  cmov(t,&base[pos][3],equal(babs,4));
  cmov(t,&base[pos][4],equal(babs,5));
  cmov(t,&base[pos][5],equal(babs,6));
  cmov(t,&base[pos][6],equal(babs,7));
  cmov(t,&base[pos][7],equal(babs,8));
  fe_copy(minust.yplusx,t->yminusx);
  fe_copy(minust.yminusx,t->yplusx);
  fe_neg(minust.xy2d,t->xy2d);
  cmov(t,&minust,bnegative);
}

/*
r = p - q
*/

void ge_sub(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q)
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

void ge_tobytes(unsigned char *s,const ge_p2 *h)
{
  fe recip;
  fe x;
  fe y;

  fe_invert(recip,h->Z);
  fe_mul(x,h->X,recip);
  fe_mul(y,h->Y,recip);
  fe_tobytes(s,y);
  s[31] ^= fe_isnegative(x) << 7;
}

/*
h = a * B
where a = a[0]+256*a[1]+...+256^31 a[31]
B is the Ed25519 base point (x,4/5) with x positive.

Preconditions:
  a[31] <= 127
*/

/*
r = a * A + b * B
where a = a[0]+256*a[1]+...+256^31 a[31].
and b = b[0]+256*b[1]+...+256^31 b[31].
B is the Ed25519 base point (x,4/5) with x positive.
*/

void ge_double_scalarmult_vartime(ge_p2 *r,const unsigned char *a,const ge_p3 *A,const unsigned char *b)
{
  signed char aslide[256];
  signed char bslide[256];
  ge_cached Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */
  ge_p1p1 t;
  ge_p3 u;
  ge_p3 A2;
  int i;

  slide(aslide,a);
  slide(bslide,b);

  ge_p3_to_cached(&Ai[0],A);
  ge_p3_dbl(&t,A); ge_p1p1_to_p3(&A2,&t);
  ge_add(&t,&A2,&Ai[0]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[1],&u);
  ge_add(&t,&A2,&Ai[1]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[2],&u);
  ge_add(&t,&A2,&Ai[2]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[3],&u);
  ge_add(&t,&A2,&Ai[3]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[4],&u);
  ge_add(&t,&A2,&Ai[4]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[5],&u);
  ge_add(&t,&A2,&Ai[5]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[6],&u);
  ge_add(&t,&A2,&Ai[6]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[7],&u);

  ge_p2_0(r);

  for (i = 255;i >= 0;--i) {
    if (aslide[i] || bslide[i]) break;
  }

  for (;i >= 0;--i) {
    ge_p2_dbl(&t,r);

    if (aslide[i] > 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_add(&t,&u,&Ai[aslide[i]/2]);
    } else if (aslide[i] < 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_sub(&t,&u,&Ai[(-aslide[i])/2]);
    }

    if (bslide[i] > 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_madd(&t,&u,&Bi[bslide[i]/2]);
    } else if (bslide[i] < 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_msub(&t,&u,&Bi[(-bslide[i])/2]);
    }

    ge_p1p1_to_p2(r,&t);
  }
}

void ge_scalarmult_vartime(ge_p3 *r,const unsigned char *a,const ge_p3 *A)
{
  signed char aslide[256];
  ge_cached Ai[8];
  ge_p1p1 t;
  ge_p3 u;
  ge_p3 A2;
  int i;

  slide(aslide,a);

  ge_p3_to_cached(&Ai[0],A);
  ge_p3_dbl(&t,A); ge_p1p1_to_p3(&A2,&t);
  ge_add(&t,&A2,&Ai[0]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[1],&u);
  ge_add(&t,&A2,&Ai[1]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[2],&u);
  ge_add(&t,&A2,&Ai[2]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[3],&u);
  ge_add(&t,&A2,&Ai[3]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[4],&u);
  ge_add(&t,&A2,&Ai[4]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[5],&u);
  ge_add(&t,&A2,&Ai[5]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[6],&u);
  ge_add(&t,&A2,&Ai[6]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[7],&u);

  ge_p3_0(r);

  for (i = 255;i >= 0;--i) {
    if (aslide[i]) break;
  }

  for (;i >= 0;--i) {
    ge_p3_dbl(&t,r);

    if (aslide[i] > 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_add(&t,&u,&Ai[aslide[i]/2]);
    } else if (aslide[i] < 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_sub(&t,&u,&Ai[(-aslide[i])/2]);
    }

   ge_p1p1_to_p3(r,&t);
  }
}

void ge_scalarmult_base(ge_p3 *h,const unsigned char *a)
{
  signed char e[64];
  signed char carry;
  ge_p1p1 r;
  ge_p2 s;
  ge_precomp t;
  int i;

  for (i = 0;i < 32;++i) {
    e[2 * i + 0] = (a[i] >> 0) & 15;
    e[2 * i + 1] = (a[i] >> 4) & 15;
  }
  /* each e[i] is between 0 and 15 */
  /* e[63] is between 0 and 7 */

  carry = 0;
  for (i = 0;i < 63;++i) {
    e[i] += carry;
    carry = e[i] + 8;
    carry >>= 4;
    e[i] -= carry << 4;
  }
  e[63] += carry;
  /* each e[i] is between -8 and 8 */

  ge_p3_0(h);
  for (i = 1;i < 64;i += 2) {
    ge_select(&t,i / 2,e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }

  ge_p3_dbl(&r,h);  ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p3(h,&r);

  for (i = 0;i < 64;i += 2) {
    ge_select(&t,i / 2,e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }
}

/*
Input:
  a[0]+256*a[1]+...+256^31*a[31] = a
  b[0]+256*b[1]+...+256^31*b[31] = b
  c[0]+256*c[1]+...+256^31*c[31] = c

Output:
  s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
*/

void sc_muladd(unsigned char *s,const unsigned char *a,const unsigned char *b,const unsigned char *c)
{
  crypto_int64 a0 = 2097151 & load_3(a);
  crypto_int64 a1 = 2097151 & (load_4(a + 2) >> 5);
  crypto_int64 a2 = 2097151 & (load_3(a + 5) >> 2);
  crypto_int64 a3 = 2097151 & (load_4(a + 7) >> 7);
  crypto_int64 a4 = 2097151 & (load_4(a + 10) >> 4);
  crypto_int64 a5 = 2097151 & (load_3(a + 13) >> 1);
  crypto_int64 a6 = 2097151 & (load_4(a + 15) >> 6);
  crypto_int64 a7 = 2097151 & (load_3(a + 18) >> 3);
  crypto_int64 a8 = 2097151 & load_3(a + 21);
  crypto_int64 a9 = 2097151 & (load_4(a + 23) >> 5);
  crypto_int64 a10 = 2097151 & (load_3(a + 26) >> 2);
  crypto_int64 a11 = (load_4(a + 28) >> 7);
  crypto_int64 b0 = 2097151 & load_3(b);
  crypto_int64 b1 = 2097151 & (load_4(b + 2) >> 5);
  crypto_int64 b2 = 2097151 & (load_3(b + 5) >> 2);
  crypto_int64 b3 = 2097151 & (load_4(b + 7) >> 7);
  crypto_int64 b4 = 2097151 & (load_4(b + 10) >> 4);
  crypto_int64 b5 = 2097151 & (load_3(b + 13) >> 1);
  crypto_int64 b6 = 2097151 & (load_4(b + 15) >> 6);
  crypto_int64 b7 = 2097151 & (load_3(b + 18) >> 3);
  crypto_int64 b8 = 2097151 & load_3(b + 21);
  crypto_int64 b9 = 2097151 & (load_4(b + 23) >> 5);
  crypto_int64 b10 = 2097151 & (load_3(b + 26) >> 2);
  crypto_int64 b11 = (load_4(b + 28) >> 7);
  crypto_int64 c0 = 2097151 & load_3(c);
  crypto_int64 c1 = 2097151 & (load_4(c + 2) >> 5);
  crypto_int64 c2 = 2097151 & (load_3(c + 5) >> 2);
  crypto_int64 c3 = 2097151 & (load_4(c + 7) >> 7);
  crypto_int64 c4 = 2097151 & (load_4(c + 10) >> 4);
  crypto_int64 c5 = 2097151 & (load_3(c + 13) >> 1);
  crypto_int64 c6 = 2097151 & (load_4(c + 15) >> 6);
  crypto_int64 c7 = 2097151 & (load_3(c + 18) >> 3);
  crypto_int64 c8 = 2097151 & load_3(c + 21);
  crypto_int64 c9 = 2097151 & (load_4(c + 23) >> 5);
  crypto_int64 c10 = 2097151 & (load_3(c + 26) >> 2);
  crypto_int64 c11 = (load_4(c + 28) >> 7);
  crypto_int64 s0;
  crypto_int64 s1;
  crypto_int64 s2;
  crypto_int64 s3;
  crypto_int64 s4;
  crypto_int64 s5;
  crypto_int64 s6;
  crypto_int64 s7;
  crypto_int64 s8;
  crypto_int64 s9;
  crypto_int64 s10;
  crypto_int64 s11;
  crypto_int64 s12;
  crypto_int64 s13;
  crypto_int64 s14;
  crypto_int64 s15;
  crypto_int64 s16;
  crypto_int64 s17;
  crypto_int64 s18;
  crypto_int64 s19;
  crypto_int64 s20;
  crypto_int64 s21;
  crypto_int64 s22;
  crypto_int64 s23;
  crypto_int64 carry0;
  crypto_int64 carry1;
  crypto_int64 carry2;
  crypto_int64 carry3;
  crypto_int64 carry4;
  crypto_int64 carry5;
  crypto_int64 carry6;
  crypto_int64 carry7;
  crypto_int64 carry8;
  crypto_int64 carry9;
  crypto_int64 carry10;
  crypto_int64 carry11;
  crypto_int64 carry12;
  crypto_int64 carry13;
  crypto_int64 carry14;
  crypto_int64 carry15;
  crypto_int64 carry16;
  crypto_int64 carry17;
  crypto_int64 carry18;
  crypto_int64 carry19;
  crypto_int64 carry20;
  crypto_int64 carry21;
  crypto_int64 carry22;

  s0 = c0 + a0*b0;
  s1 = c1 + a0*b1 + a1*b0;
  s2 = c2 + a0*b2 + a1*b1 + a2*b0;
  s3 = c3 + a0*b3 + a1*b2 + a2*b1 + a3*b0;
  s4 = c4 + a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0;
  s5 = c5 + a0*b5 + a1*b4 + a2*b3 + a3*b2 + a4*b1 + a5*b0;
  s6 = c6 + a0*b6 + a1*b5 + a2*b4 + a3*b3 + a4*b2 + a5*b1 + a6*b0;
  s7 = c7 + a0*b7 + a1*b6 + a2*b5 + a3*b4 + a4*b3 + a5*b2 + a6*b1 + a7*b0;
  s8 = c8 + a0*b8 + a1*b7 + a2*b6 + a3*b5 + a4*b4 + a5*b3 + a6*b2 + a7*b1 + a8*b0;
  s9 = c9 + a0*b9 + a1*b8 + a2*b7 + a3*b6 + a4*b5 + a5*b4 + a6*b3 + a7*b2 + a8*b1 + a9*b0;
  s10 = c10 + a0*b10 + a1*b9 + a2*b8 + a3*b7 + a4*b6 + a5*b5 + a6*b4 + a7*b3 + a8*b2 + a9*b1 + a10*b0;
  s11 = c11 + a0*b11 + a1*b10 + a2*b9 + a3*b8 + a4*b7 + a5*b6 + a6*b5 + a7*b4 + a8*b3 + a9*b2 + a10*b1 + a11*b0;
  s12 = a1*b11 + a2*b10 + a3*b9 + a4*b8 + a5*b7 + a6*b6 + a7*b5 + a8*b4 + a9*b3 + a10*b2 + a11*b1;
  s13 = a2*b11 + a3*b10 + a4*b9 + a5*b8 + a6*b7 + a7*b6 + a8*b5 + a9*b4 + a10*b3 + a11*b2;
  s14 = a3*b11 + a4*b10 + a5*b9 + a6*b8 + a7*b7 + a8*b6 + a9*b5 + a10*b4 + a11*b3;
  s15 = a4*b11 + a5*b10 + a6*b9 + a7*b8 + a8*b7 + a9*b6 + a10*b5 + a11*b4;
  s16 = a5*b11 + a6*b10 + a7*b9 + a8*b8 + a9*b7 + a10*b6 + a11*b5;
  s17 = a6*b11 + a7*b10 + a8*b9 + a9*b8 + a10*b7 + a11*b6;
  s18 = a7*b11 + a8*b10 + a9*b9 + a10*b8 + a11*b7;
  s19 = a8*b11 + a9*b10 + a10*b9 + a11*b8;
  s20 = a9*b11 + a10*b10 + a11*b9;
  s21 = a10*b11 + a11*b10;
  s22 = a11*b11;
  s23 = 0;

  carry0 = (s0 + (1L << 20)) >> 21; s1 += carry0; s0 -= carry0 * (1L << 21);
  carry2 = (s2 + (1L << 20)) >> 21; s3 += carry2; s2 -= carry2 * (1L << 21);
  carry4 = (s4 + (1L << 20)) >> 21; s5 += carry4; s4 -= carry4 * (1L << 21);
  carry6 = (s6 + (1L << 20)) >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry8 = (s8 + (1L << 20)) >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry10 = (s10 + (1L << 20)) >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);
  carry12 = (s12 + (1L << 20)) >> 21; s13 += carry12; s12 -= carry12 * (1L << 21);
  carry14 = (s14 + (1L << 20)) >> 21; s15 += carry14; s14 -= carry14 * (1L << 21);
  carry16 = (s16 + (1L << 20)) >> 21; s17 += carry16; s16 -= carry16 * (1L << 21);
  carry18 = (s18 + (1L << 20)) >> 21; s19 += carry18; s18 -= carry18 * (1L << 21);
  carry20 = (s20 + (1L << 20)) >> 21; s21 += carry20; s20 -= carry20 * (1L << 21);
  carry22 = (s22 + (1L << 20)) >> 21; s23 += carry22; s22 -= carry22 * (1L << 21);

  carry1 = (s1 + (1L << 20)) >> 21; s2 += carry1; s1 -= carry1 * (1L << 21);
  carry3 = (s3 + (1L << 20)) >> 21; s4 += carry3; s3 -= carry3 * (1L << 21);
  carry5 = (s5 + (1L << 20)) >> 21; s6 += carry5; s5 -= carry5 * (1L << 21);
  carry7 = (s7 + (1L << 20)) >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry9 = (s9 + (1L << 20)) >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry11 = (s11 + (1L << 20)) >> 21; s12 += carry11; s11 -= carry11 * (1L << 21);
  carry13 = (s13 + (1L << 20)) >> 21; s14 += carry13; s13 -= carry13 * (1L << 21);
  carry15 = (s15 + (1L << 20)) >> 21; s16 += carry15; s15 -= carry15 * (1L << 21);
  carry17 = (s17 + (1L << 20)) >> 21; s18 += carry17; s17 -= carry17 * (1L << 21);
  carry19 = (s19 + (1L << 20)) >> 21; s20 += carry19; s19 -= carry19 * (1L << 21);
  carry21 = (s21 + (1L << 20)) >> 21; s22 += carry21; s21 -= carry21 * (1L << 21);

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

  carry6 = (s6 + (1L << 20)) >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry8 = (s8 + (1L << 20)) >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry10 = (s10 + (1L << 20)) >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);
  carry12 = (s12 + (1L << 20)) >> 21; s13 += carry12; s12 -= carry12 * (1L << 21);
  carry14 = (s14 + (1L << 20)) >> 21; s15 += carry14; s14 -= carry14 * (1L << 21);
  carry16 = (s16 + (1L << 20)) >> 21; s17 += carry16; s16 -= carry16 * (1L << 21);

  carry7 = (s7 + (1L << 20)) >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry9 = (s9 + (1L << 20)) >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry11 = (s11 + (1L << 20)) >> 21; s12 += carry11; s11 -= carry11 * (1L << 21);
  carry13 = (s13 + (1L << 20)) >> 21; s14 += carry13; s13 -= carry13 * (1L << 21);
  carry15 = (s15 + (1L << 20)) >> 21; s16 += carry15; s15 -= carry15 * (1L << 21);

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

  carry0 = (s0 + (1L << 20)) >> 21; s1 += carry0; s0 -= carry0 * (1L << 21);
  carry2 = (s2 + (1L << 20)) >> 21; s3 += carry2; s2 -= carry2 * (1L << 21);
  carry4 = (s4 + (1L << 20)) >> 21; s5 += carry4; s4 -= carry4 * (1L << 21);
  carry6 = (s6 + (1L << 20)) >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry8 = (s8 + (1L << 20)) >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry10 = (s10 + (1L << 20)) >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);

  carry1 = (s1 + (1L << 20)) >> 21; s2 += carry1; s1 -= carry1 * (1L << 21);
  carry3 = (s3 + (1L << 20)) >> 21; s4 += carry3; s3 -= carry3 * (1L << 21);
  carry5 = (s5 + (1L << 20)) >> 21; s6 += carry5; s5 -= carry5 * (1L << 21);
  carry7 = (s7 + (1L << 20)) >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry9 = (s9 + (1L << 20)) >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry11 = (s11 + (1L << 20)) >> 21; s12 += carry11; s11 -= carry11 * (1L << 21);

  s0 += s12 * 666643;
  s1 += s12 * 470296;
  s2 += s12 * 654183;
  s3 -= s12 * 997805;
  s4 += s12 * 136657;
  s5 -= s12 * 683901;
  s12 = 0;

  carry0 = s0 >> 21; s1 += carry0; s0 -= carry0 * (1L << 21);
  carry1 = s1 >> 21; s2 += carry1; s1 -= carry1 * (1L << 21);
  carry2 = s2 >> 21; s3 += carry2; s2 -= carry2 * (1L << 21);
  carry3 = s3 >> 21; s4 += carry3; s3 -= carry3 * (1L << 21);
  carry4 = s4 >> 21; s5 += carry4; s4 -= carry4 * (1L << 21);
  carry5 = s5 >> 21; s6 += carry5; s5 -= carry5 * (1L << 21);
  carry6 = s6 >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry7 = s7 >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry8 = s8 >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry9 = s9 >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry10 = s10 >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);
  carry11 = s11 >> 21; s12 += carry11; s11 -= carry11 * (1L << 21);

  s0 += s12 * 666643;
  s1 += s12 * 470296;
  s2 += s12 * 654183;
  s3 -= s12 * 997805;
  s4 += s12 * 136657;
  s5 -= s12 * 683901;

  carry0 = s0 >> 21; s1 += carry0; s0 -= carry0 * (1L << 21);
  carry1 = s1 >> 21; s2 += carry1; s1 -= carry1 * (1L << 21);
  carry2 = s2 >> 21; s3 += carry2; s2 -= carry2 * (1L << 21);
  carry3 = s3 >> 21; s4 += carry3; s3 -= carry3 * (1L << 21);
  carry4 = s4 >> 21; s5 += carry4; s4 -= carry4 * (1L << 21);
  carry5 = s5 >> 21; s6 += carry5; s5 -= carry5 * (1L << 21);
  carry6 = s6 >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry7 = s7 >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry8 = s8 >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry9 = s9 >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry10 = s10 >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);

  s[0] = s0 >> 0;
  s[1] = s0 >> 8;
  s[2] = (s0 >> 16) | (s1 * (1L << 5));
  s[3] = s1 >> 3;
  s[4] = s1 >> 11;
  s[5] = (s1 >> 19) | (s2 * (1L << 2));
  s[6] = s2 >> 6;
  s[7] = (s2 >> 14) | (s3 * (1L << 7));
  s[8] = s3 >> 1;
  s[9] = s3 >> 9;
  s[10] = (s3 >> 17) | (s4 * (1L << 4));
  s[11] = s4 >> 4;
  s[12] = s4 >> 12;
  s[13] = (s4 >> 20) | (s5 * (1L << 1));
  s[14] = s5 >> 7;
  s[15] = (s5 >> 15) | (s6 * (1L << 6));
  s[16] = s6 >> 2;
  s[17] = s6 >> 10;
  s[18] = (s6 >> 18) | (s7 * (1L << 3));
  s[19] = s7 >> 5;
  s[20] = s7 >> 13;
  s[21] = s8 >> 0;
  s[22] = s8 >> 8;
  s[23] = (s8 >> 16) | (s9 * (1L << 5));
  s[24] = s9 >> 3;
  s[25] = s9 >> 11;
  s[26] = (s9 >> 19) | (s10 * (1L << 2));
  s[27] = s10 >> 6;
  s[28] = (s10 >> 14) | (s11 * (1L << 7));
  s[29] = s11 >> 1;
  s[30] = s11 >> 9;
  s[31] = s11 >> 17;
}

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/

void sc_reduce(unsigned char *s)
{
  crypto_int64 s0 = 2097151 & load_3(s);
  crypto_int64 s1 = 2097151 & (load_4(s + 2) >> 5);
  crypto_int64 s2 = 2097151 & (load_3(s + 5) >> 2);
  crypto_int64 s3 = 2097151 & (load_4(s + 7) >> 7);
  crypto_int64 s4 = 2097151 & (load_4(s + 10) >> 4);
  crypto_int64 s5 = 2097151 & (load_3(s + 13) >> 1);
  crypto_int64 s6 = 2097151 & (load_4(s + 15) >> 6);
  crypto_int64 s7 = 2097151 & (load_3(s + 18) >> 3);
  crypto_int64 s8 = 2097151 & load_3(s + 21);
  crypto_int64 s9 = 2097151 & (load_4(s + 23) >> 5);
  crypto_int64 s10 = 2097151 & (load_3(s + 26) >> 2);
  crypto_int64 s11 = 2097151 & (load_4(s + 28) >> 7);
  crypto_int64 s12 = 2097151 & (load_4(s + 31) >> 4);
  crypto_int64 s13 = 2097151 & (load_3(s + 34) >> 1);
  crypto_int64 s14 = 2097151 & (load_4(s + 36) >> 6);
  crypto_int64 s15 = 2097151 & (load_3(s + 39) >> 3);
  crypto_int64 s16 = 2097151 & load_3(s + 42);
  crypto_int64 s17 = 2097151 & (load_4(s + 44) >> 5);
  crypto_int64 s18 = 2097151 & (load_3(s + 47) >> 2);
  crypto_int64 s19 = 2097151 & (load_4(s + 49) >> 7);
  crypto_int64 s20 = 2097151 & (load_4(s + 52) >> 4);
  crypto_int64 s21 = 2097151 & (load_3(s + 55) >> 1);
  crypto_int64 s22 = 2097151 & (load_4(s + 57) >> 6);
  crypto_int64 s23 = (load_4(s + 60) >> 3);
  crypto_int64 carry0;
  crypto_int64 carry1;
  crypto_int64 carry2;
  crypto_int64 carry3;
  crypto_int64 carry4;
  crypto_int64 carry5;
  crypto_int64 carry6;
  crypto_int64 carry7;
  crypto_int64 carry8;
  crypto_int64 carry9;
  crypto_int64 carry10;
  crypto_int64 carry11;
  crypto_int64 carry12;
  crypto_int64 carry13;
  crypto_int64 carry14;
  crypto_int64 carry15;
  crypto_int64 carry16;

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

  carry6 = (s6 + (1L << 20)) >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry8 = (s8 + (1L << 20)) >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry10 = (s10 + (1L << 20)) >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);
  carry12 = (s12 + (1L << 20)) >> 21; s13 += carry12; s12 -= carry12 * (1L << 21);
  carry14 = (s14 + (1L << 20)) >> 21; s15 += carry14; s14 -= carry14 * (1L << 21);
  carry16 = (s16 + (1L << 20)) >> 21; s17 += carry16; s16 -= carry16 * (1L << 21);

  carry7 = (s7 + (1L << 20)) >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry9 = (s9 + (1L << 20)) >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry11 = (s11 + (1L << 20)) >> 21; s12 += carry11; s11 -= carry11 * (1L << 21);
  carry13 = (s13 + (1L << 20)) >> 21; s14 += carry13; s13 -= carry13 * (1L << 21);
  carry15 = (s15 + (1L << 20)) >> 21; s16 += carry15; s15 -= carry15 * (1L << 21);

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

  carry0 = (s0 + (1L << 20)) >> 21; s1 += carry0; s0 -= carry0 * (1L << 21);
  carry2 = (s2 + (1L << 20)) >> 21; s3 += carry2; s2 -= carry2 * (1L << 21);
  carry4 = (s4 + (1L << 20)) >> 21; s5 += carry4; s4 -= carry4 * (1L << 21);
  carry6 = (s6 + (1L << 20)) >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry8 = (s8 + (1L << 20)) >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry10 = (s10 + (1L << 20)) >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);

  carry1 = (s1 + (1L << 20)) >> 21; s2 += carry1; s1 -= carry1 * (1L << 21);
  carry3 = (s3 + (1L << 20)) >> 21; s4 += carry3; s3 -= carry3 * (1L << 21);
  carry5 = (s5 + (1L << 20)) >> 21; s6 += carry5; s5 -= carry5 * (1L << 21);
  carry7 = (s7 + (1L << 20)) >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry9 = (s9 + (1L << 20)) >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry11 = (s11 + (1L << 20)) >> 21; s12 += carry11; s11 -= carry11 * (1L << 21);

  s0 += s12 * 666643;
  s1 += s12 * 470296;
  s2 += s12 * 654183;
  s3 -= s12 * 997805;
  s4 += s12 * 136657;
  s5 -= s12 * 683901;
  s12 = 0;

  carry0 = s0 >> 21; s1 += carry0; s0 -= carry0 * (1L << 21);
  carry1 = s1 >> 21; s2 += carry1; s1 -= carry1 * (1L << 21);
  carry2 = s2 >> 21; s3 += carry2; s2 -= carry2 * (1L << 21);
  carry3 = s3 >> 21; s4 += carry3; s3 -= carry3 * (1L << 21);
  carry4 = s4 >> 21; s5 += carry4; s4 -= carry4 * (1L << 21);
  carry5 = s5 >> 21; s6 += carry5; s5 -= carry5 * (1L << 21);
  carry6 = s6 >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry7 = s7 >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry8 = s8 >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry9 = s9 >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry10 = s10 >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);
  carry11 = s11 >> 21; s12 += carry11; s11 -= carry11 * (1L << 21);

  s0 += s12 * 666643;
  s1 += s12 * 470296;
  s2 += s12 * 654183;
  s3 -= s12 * 997805;
  s4 += s12 * 136657;
  s5 -= s12 * 683901;

  carry0 = s0 >> 21; s1 += carry0; s0 -= carry0 * (1L << 21);
  carry1 = s1 >> 21; s2 += carry1; s1 -= carry1 * (1L << 21);
  carry2 = s2 >> 21; s3 += carry2; s2 -= carry2 * (1L << 21);
  carry3 = s3 >> 21; s4 += carry3; s3 -= carry3 * (1L << 21);
  carry4 = s4 >> 21; s5 += carry4; s4 -= carry4 * (1L << 21);
  carry5 = s5 >> 21; s6 += carry5; s5 -= carry5 * (1L << 21);
  carry6 = s6 >> 21; s7 += carry6; s6 -= carry6 * (1L << 21);
  carry7 = s7 >> 21; s8 += carry7; s7 -= carry7 * (1L << 21);
  carry8 = s8 >> 21; s9 += carry8; s8 -= carry8 * (1L << 21);
  carry9 = s9 >> 21; s10 += carry9; s9 -= carry9 * (1L << 21);
  carry10 = s10 >> 21; s11 += carry10; s10 -= carry10 * (1L << 21);

  s[0] = s0 >> 0;
  s[1] = s0 >> 8;
  s[2] = (s0 >> 16) | (s1 * (1L << 5));
  s[3] = s1 >> 3;
  s[4] = s1 >> 11;
  s[5] = (s1 >> 19) | (s2 * (1L << 2));
  s[6] = s2 >> 6;
  s[7] = (s2 >> 14) | (s3 * (1L << 7));
  s[8] = s3 >> 1;
  s[9] = s3 >> 9;
  s[10] = (s3 >> 17) | (s4 * (1L << 4));
  s[11] = s4 >> 4;
  s[12] = s4 >> 12;
  s[13] = (s4 >> 20) | (s5 * (1L << 1));
  s[14] = s5 >> 7;
  s[15] = (s5 >> 15) | (s6 * (1L << 6));
  s[16] = s6 >> 2;
  s[17] = s6 >> 10;
  s[18] = (s6 >> 18) | (s7 * (1L << 3));
  s[19] = s7 >> 5;
  s[20] = s7 >> 13;
  s[21] = s8 >> 0;
  s[22] = s8 >> 8;
  s[23] = (s8 >> 16) | (s9 * (1L << 5));
  s[24] = s9 >> 3;
  s[25] = s9 >> 11;
  s[26] = (s9 >> 19) | (s10 * (1L << 2));
  s[27] = s10 >> 6;
  s[28] = (s10 >> 14) | (s11 * (1L << 7));
  s[29] = s11 >> 1;
  s[30] = s11 >> 9;
  s[31] = s11 >> 17;
}
