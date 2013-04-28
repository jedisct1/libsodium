/*
20080910
D. J. Bernstein
Public domain.
*/

#ifdef HAVE_FENV_H
# include <fenv.h>
#endif
#include <stdint.h>
#include <stdlib.h>

#include "api.h"
#include "crypto_onetimeauth_poly1305_53.h"
#include "utils.h"

#ifdef HAVE_FENV_H
# pragma STDC FENV_ACCESS ON
#endif

typedef uint8_t  uchar;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef int64_t  int64;
typedef uint64_t uint64;

static const double poly1305_53_constants[] = {
  0.00000000558793544769287109375 /* alpham80 = 3 2^(-29) */
, 24.0 /* alpham48 = 3 2^3 */
, 103079215104.0 /* alpham16 = 3 2^35 */
, 6755399441055744.0 /* alpha0 = 3 2^51 */
, 1770887431076116955136.0 /* alpha18 = 3 2^69 */
, 29014219670751100192948224.0 /* alpha32 = 3 2^83 */
, 7605903601369376408980219232256.0 /* alpha50 = 3 2^101 */
, 124615124604835863084731911901282304.0 /* alpha64 = 3 2^115 */
, 32667107224410092492483962313449748299776.0 /* alpha82 = 3 2^133 */
, 535217884764734955396857238543560676143529984.0 /* alpha96 = 3 2^147 */
, 35076039295941670036888435985190792471742381031424.0 /* alpha112 = 3 2^163 */
, 9194973245195333150150082162901855101712434733101613056.0 /* alpha130 = 3 2^181 */
, 0.0000000000000000000000000000000000000036734198463196484624023016788195177431833298649127735047148490821200539357960224151611328125 /* scale = 5 2^(-130) */
, 6755408030990331.0 /* offset0 = alpha0 + 2^33 - 5 */
, 29014256564239239022116864.0 /* offset1 = alpha32 + 2^65 - 2^33 */
, 124615283061160854719918951570079744.0 /* offset2 = alpha64 + 2^97 - 2^65 */
, 535219245894202480694386063513315216128475136.0 /* offset3 = alpha96 + 2^130 - 2^97 */
} ;

int crypto_onetimeauth(unsigned char *out,const unsigned char *m,unsigned long long l,const unsigned char *k)
{
  register const unsigned char *r = k;
  register const unsigned char *s = k + 16;
  double r0high_stack;
  double r1high_stack;
  double r1low_stack;
  double sr1high_stack;
  double r2low_stack;
  double sr2high_stack;
  double r0low_stack;
  double sr1low_stack;
  double r2high_stack;
  double sr2low_stack;
  double r3high_stack;
  double sr3high_stack;
  double r3low_stack;
  double sr3low_stack;
  int64 d0;
  int64 d1;
  int64 d2;
  int64 d3;
  register double scale;
  register double alpha0;
  register double alpha32;
  register double alpha64;
  register double alpha96;
  register double alpha130;
  register double h0;
  register double h1;
  register double h2;
  register double h3;
  register double h4;
  register double h5;
  register double h6;
  register double h7;
  register double y7;
  register double y6;
  register double y1;
  register double y0;
  register double y5;
  register double y4;
  register double x7;
  register double x6;
  register double x1;
  register double x0;
  register double y3;
  register double y2;
  register double r3low;
  register double r0low;
  register double r3high;
  register double r0high;
  register double sr1low;
  register double x5;
  register double r3lowx0;
  register double sr1high;
  register double x4;
  register double r0lowx6;
  register double r1low;
  register double x3;
  register double r3highx0;
  register double r1high;
  register double x2;
  register double r0highx6;
  register double sr2low;
  register double r0lowx0;
  register double sr2high;
  register double sr1lowx6;
  register double r2low;
  register double r0highx0;
  register double r2high;
  register double sr1highx6;
  register double sr3low;
  register double r1lowx0;
  register double sr3high;
  register double sr2lowx6;
  register double r1highx0;
  register double sr2highx6;
  register double r2lowx0;
  register double sr3lowx6;
  register double r2highx0;
  register double sr3highx6;
  register double r1highx4;
  register double r1lowx4;
  register double r0highx4;
  register double r0lowx4;
  register double sr3highx4;
  register double sr3lowx4;
  register double sr2highx4;
  register double sr2lowx4;
  register double r0lowx2;
  register double r0highx2;
  register double r1lowx2;
  register double r1highx2;
  register double r2lowx2;
  register double r2highx2;
  register double sr3lowx2;
  register double sr3highx2;
  register double z0;
  register double z1;
  register double z2;
  register double z3;
  register int64 r0;
  register int64 r1;
  register int64 r2;
  register int64 r3;
  register uint32 r00;
  register uint32 r01;
  register uint32 r02;
  register uint32 r03;
  register uint32 r10;
  register uint32 r11;
  register uint32 r12;
  register uint32 r13;
  register uint32 r20;
  register uint32 r21;
  register uint32 r22;
  register uint32 r23;
  register uint32 r30;
  register uint32 r31;
  register uint32 r32;
  register uint32 r33;
  register int64 m0;
  register int64 m1;
  register int64 m2;
  register int64 m3;
  register uint32 m00;
  register uint32 m01;
  register uint32 m02;
  register uint32 m03;
  register uint32 m10;
  register uint32 m11;
  register uint32 m12;
  register uint32 m13;
  register uint32 m20;
  register uint32 m21;
  register uint32 m22;
  register uint32 m23;
  register uint32 m30;
  register uint32 m31;
  register uint32 m32;
  register uint64 m33;
  register char *constants;
  register int32 lbelow2;
  register int32 lbelow3;
  register int32 lbelow4;
  register int32 lbelow5;
  register int32 lbelow6;
  register int32 lbelow7;
  register int32 lbelow8;
  register int32 lbelow9;
  register int32 lbelow10;
  register int32 lbelow11;
  register int32 lbelow12;
  register int32 lbelow13;
  register int32 lbelow14;
  register int32 lbelow15;
  register double alpham80;
  register double alpham48;
  register double alpham16;
  register double alpha18;
  register double alpha50;
  register double alpha82;
  register double alpha112;
  register double offset0;
  register double offset1;
  register double offset2;
  register double offset3;
  register uint32 s00;
  register uint32 s01;
  register uint32 s02;
  register uint32 s03;
  register uint32 s10;
  register uint32 s11;
  register uint32 s12;
  register uint32 s13;
  register uint32 s20;
  register uint32 s21;
  register uint32 s22;
  register uint32 s23;
  register uint32 s30;
  register uint32 s31;
  register uint32 s32;
  register uint32 s33;
  register uint64 bits32;
  register uint64 f;
  register uint64 f0;
  register uint64 f1;
  register uint64 f2;
  register uint64 f3;
  register uint64 f4;
  register uint64 g;
  register uint64 g0;
  register uint64 g1;
  register uint64 g2;
  register uint64 g3;
  register uint64 g4;

#ifdef HAVE_FENV_H
  const int previous_rounding_mode = fegetround();
  if (previous_rounding_mode != FE_TONEAREST) {
      if (fesetround(FE_TONEAREST) != 0) {
          return -1;
      }
  }
#endif

  r00 = *(uchar *) (r + 0);
  constants = (char *) &poly1305_53_constants;

  r01 = *(uchar *) (r + 1);

  r02 = *(uchar *) (r + 2);
  r0 = 2151;

  r03 = *(uchar *) (r + 3); r03 &= 15;
  r0 <<= 51;

  r10 = *(uchar *) (r + 4); r10 &= 252;
  r01 <<= 8;
  r0 += r00;

  r11 = *(uchar *) (r + 5);
  r02 <<= 16;
  r0 += r01;

  r12 = *(uchar *) (r + 6);
  r03 <<= 24;
  r0 += r02;

  r13 = *(uchar *) (r + 7); r13 &= 15;
  r1 = 2215;
  r0 += r03;

  d0 = r0;
  r1 <<= 51;
  r2 = 2279;

  r20 = *(uchar *) (r + 8); r20 &= 252;
  r11 <<= 8;
  r1 += r10;

  r21 = *(uchar *) (r + 9);
  r12 <<= 16;
  r1 += r11;

  r22 = *(uchar *) (r + 10);
  r13 <<= 24;
  r1 += r12;

  r23 = *(uchar *) (r + 11); r23 &= 15;
  r2 <<= 51;
  r1 += r13;

  d1 = r1;
  r21 <<= 8;
  r2 += r20;

  r30 = *(uchar *) (r + 12); r30 &= 252;
  r22 <<= 16;
  r2 += r21;

  r31 = *(uchar *) (r + 13);
  r23 <<= 24;
  r2 += r22;

  r32 = *(uchar *) (r + 14);
  r2 += r23;
  r3 = 2343;

  d2 = r2;
  r3 <<= 51;
  alpha32 = *(double *) (constants + 40);

  r33 = *(uchar *) (r + 15); r33 &= 15;
  r31 <<= 8;
  r3 += r30;

  r32 <<= 16;
  r3 += r31;

  r33 <<= 24;
  r3 += r32;

  r3 += r33;
  h0 = alpha32 - alpha32;

  d3 = r3;
  h1 = alpha32 - alpha32;

  alpha0 = *(double *) (constants + 24);
  h2 = alpha32 - alpha32;

  alpha64 = *(double *) (constants + 56);
  h3 = alpha32 - alpha32;

  alpha18 = *(double *) (constants + 32);
  h4 = alpha32 - alpha32;

  r0low = *(double *) &d0;
  h5 = alpha32 - alpha32;

  r1low = *(double *) &d1;
  h6 = alpha32 - alpha32;

  r2low = *(double *) &d2;
  h7 = alpha32 - alpha32;

  alpha50 = *(double *) (constants + 48);
  r0low -= alpha0;

  alpha82 = *(double *) (constants + 64);
  r1low -= alpha32;

  scale = *(double *) (constants + 96);
  r2low -= alpha64;

  alpha96 = *(double *) (constants + 72);
  r0high = r0low + alpha18;

  r3low = *(double *) &d3;

  alpham80 = *(double *) (constants + 0);
  r1high = r1low + alpha50;
  sr1low = scale * r1low;

  alpham48 = *(double *) (constants + 8);
  r2high = r2low + alpha82;
  sr2low = scale * r2low;

  r0high -= alpha18;
  r0high_stack = r0high;

  r3low -= alpha96;

  r1high -= alpha50;
  r1high_stack = r1high;

  sr1high = sr1low + alpham80;

  alpha112 = *(double *) (constants + 80);
  r0low -= r0high;

  alpham16 = *(double *) (constants + 16);
  r2high -= alpha82;
  sr3low = scale * r3low;

  alpha130 = *(double *) (constants + 88);
  sr2high = sr2low + alpham48;

  r1low -= r1high;
  r1low_stack = r1low;

  sr1high -= alpham80;
  sr1high_stack = sr1high;

  r2low -= r2high;
  r2low_stack = r2low;

  sr2high -= alpham48;
  sr2high_stack = sr2high;

  r3high = r3low + alpha112;
  r0low_stack = r0low;

  sr1low -= sr1high;
  sr1low_stack = sr1low;

  sr3high = sr3low + alpham16;
  r2high_stack = r2high;

  sr2low -= sr2high;
  sr2low_stack = sr2low;

  r3high -= alpha112;
  r3high_stack = r3high;


  sr3high -= alpham16;
  sr3high_stack = sr3high;


  r3low -= r3high;
  r3low_stack = r3low;


  sr3low -= sr3high;
  sr3low_stack = sr3low;

if (l < 16) goto addatmost15bytes;

  m00 = *(uchar *) (m + 0);
  m0 = 2151;

  m0 <<= 51;
  m1 = 2215;
  m01 = *(uchar *) (m + 1);

  m1 <<= 51;
  m2 = 2279;
  m02 = *(uchar *) (m + 2);

  m2 <<= 51;
  m3 = 2343;
  m03 = *(uchar *) (m + 3);

  m10 = *(uchar *) (m + 4);
  m01 <<= 8;
  m0 += m00;

  m11 = *(uchar *) (m + 5);
  m02 <<= 16;
  m0 += m01;

  m12 = *(uchar *) (m + 6);
  m03 <<= 24;
  m0 += m02;

  m13 = *(uchar *) (m + 7);
  m3 <<= 51;
  m0 += m03;

  m20 = *(uchar *) (m + 8);
  m11 <<= 8;
  m1 += m10;

  m21 = *(uchar *) (m + 9);
  m12 <<= 16;
  m1 += m11;

  m22 = *(uchar *) (m + 10);
  m13 <<= 24;
  m1 += m12;

  m23 = *(uchar *) (m + 11);
  m1 += m13;

  m30 = *(uchar *) (m + 12);
  m21 <<= 8;
  m2 += m20;

  m31 = *(uchar *) (m + 13);
  m22 <<= 16;
  m2 += m21;

  m32 = *(uchar *) (m + 14);
  m23 <<= 24;
  m2 += m22;

  m33 = *(uchar *) (m + 15);
  m2 += m23;

  d0 = m0;
  m31 <<= 8;
  m3 += m30;

  d1 = m1;
  m32 <<= 16;
  m3 += m31;

  d2 = m2;
  m33 += 256;

  m33 <<= 24;
  m3 += m32;

  m3 += m33;
  d3 = m3;

  m += 16;
  l -= 16;

  z0 = *(double *) &d0;

  z1 = *(double *) &d1;

  z2 = *(double *) &d2;

  z3 = *(double *) &d3;

  z0 -= alpha0;

  z1 -= alpha32;

  z2 -= alpha64;

  z3 -= alpha96;

  h0 += z0;

  h1 += z1;

  h3 += z2;

  h5 += z3;

if (l < 16) goto multiplyaddatmost15bytes;

multiplyaddatleast16bytes:;

  m2 = 2279;
  m20 = *(uchar *) (m + 8);
  y7 = h7 + alpha130;

  m2 <<= 51;
  m3 = 2343;
  m21 = *(uchar *) (m + 9);
  y6 = h6 + alpha130;

  m3 <<= 51;
  m0 = 2151;
  m22 = *(uchar *) (m + 10);
  y1 = h1 + alpha32;

  m0 <<= 51;
  m1 = 2215;
  m23 = *(uchar *) (m + 11);
  y0 = h0 + alpha32;

  m1 <<= 51;
  m30 = *(uchar *) (m + 12);
  y7 -= alpha130;

  m21 <<= 8;
  m2 += m20;
  m31 = *(uchar *) (m + 13);
  y6 -= alpha130;

  m22 <<= 16;
  m2 += m21;
  m32 = *(uchar *) (m + 14);
  y1 -= alpha32;

  m23 <<= 24;
  m2 += m22;
  m33 = *(uchar *) (m + 15);
  y0 -= alpha32;

  m2 += m23;
  m00 = *(uchar *) (m + 0);
  y5 = h5 + alpha96;

  m31 <<= 8;
  m3 += m30;
  m01 = *(uchar *) (m + 1);
  y4 = h4 + alpha96;

  m32 <<= 16;
  m02 = *(uchar *) (m + 2);
  x7 = h7 - y7;
  y7 *= scale;

  m33 += 256;
  m03 = *(uchar *) (m + 3);
  x6 = h6 - y6;
  y6 *= scale;

  m33 <<= 24;
  m3 += m31;
  m10 = *(uchar *) (m + 4);
  x1 = h1 - y1;

  m01 <<= 8;
  m3 += m32;
  m11 = *(uchar *) (m + 5);
  x0 = h0 - y0;

  m3 += m33;
  m0 += m00;
  m12 = *(uchar *) (m + 6);
  y5 -= alpha96;

  m02 <<= 16;
  m0 += m01;
  m13 = *(uchar *) (m + 7);
  y4 -= alpha96;

  m03 <<= 24;
  m0 += m02;
  d2 = m2;
  x1 += y7;

  m0 += m03;
  d3 = m3;
  x0 += y6;

  m11 <<= 8;
  m1 += m10;
  d0 = m0;
  x7 += y5;

  m12 <<= 16;
  m1 += m11;
  x6 += y4;

  m13 <<= 24;
  m1 += m12;
  y3 = h3 + alpha64;

  m1 += m13;
  d1 = m1;
  y2 = h2 + alpha64;

  x0 += x1;

  x6 += x7;

  y3 -= alpha64;
  r3low = r3low_stack;

  y2 -= alpha64;
  r0low = r0low_stack;

  x5 = h5 - y5;
  r3lowx0 = r3low * x0;
  r3high = r3high_stack;

  x4 = h4 - y4;
  r0lowx6 = r0low * x6;
  r0high = r0high_stack;

  x3 = h3 - y3;
  r3highx0 = r3high * x0;
  sr1low = sr1low_stack;

  x2 = h2 - y2;
  r0highx6 = r0high * x6;
  sr1high = sr1high_stack;

  x5 += y3;
  r0lowx0 = r0low * x0;
  r1low = r1low_stack;

  h6 = r3lowx0 + r0lowx6;
  sr1lowx6 = sr1low * x6;
  r1high = r1high_stack;

  x4 += y2;
  r0highx0 = r0high * x0;
  sr2low = sr2low_stack;

  h7 = r3highx0 + r0highx6;
  sr1highx6 = sr1high * x6;
  sr2high = sr2high_stack;

  x3 += y1;
  r1lowx0 = r1low * x0;
  r2low = r2low_stack;

  h0 = r0lowx0 + sr1lowx6;
  sr2lowx6 = sr2low * x6;
  r2high = r2high_stack;

  x2 += y0;
  r1highx0 = r1high * x0;
  sr3low = sr3low_stack;

  h1 = r0highx0 + sr1highx6;
  sr2highx6 = sr2high * x6;
  sr3high = sr3high_stack;

  x4 += x5;
  r2lowx0 = r2low * x0;
  z2 = *(double *) &d2;

  h2 = r1lowx0 + sr2lowx6;
  sr3lowx6 = sr3low * x6;

  x2 += x3;
  r2highx0 = r2high * x0;
  z3 = *(double *) &d3;

  h3 = r1highx0 + sr2highx6;
  sr3highx6 = sr3high * x6;

  r1highx4 = r1high * x4;
  z2 -= alpha64;

  h4 = r2lowx0 + sr3lowx6;
  r1lowx4 = r1low * x4;

  r0highx4 = r0high * x4;
  z3 -= alpha96;

  h5 = r2highx0 + sr3highx6;
  r0lowx4 = r0low * x4;

  h7 += r1highx4;
  sr3highx4 = sr3high * x4;

  h6 += r1lowx4;
  sr3lowx4 = sr3low * x4;

  h5 += r0highx4;
  sr2highx4 = sr2high * x4;

  h4 += r0lowx4;
  sr2lowx4 = sr2low * x4;

  h3 += sr3highx4;
  r0lowx2 = r0low * x2;

  h2 += sr3lowx4;
  r0highx2 = r0high * x2;

  h1 += sr2highx4;
  r1lowx2 = r1low * x2;

  h0 += sr2lowx4;
  r1highx2 = r1high * x2;

  h2 += r0lowx2;
  r2lowx2 = r2low * x2;

  h3 += r0highx2;
  r2highx2 = r2high * x2;

  h4 += r1lowx2;
  sr3lowx2 = sr3low * x2;

  h5 += r1highx2;
  sr3highx2 = sr3high * x2;
  alpha0 = *(double *) (constants + 24);

  m += 16;
  h6 += r2lowx2;

  l -= 16;
  h7 += r2highx2;

  z1 = *(double *) &d1;
  h0 += sr3lowx2;

  z0 = *(double *) &d0;
  h1 += sr3highx2;

  z1 -= alpha32;

  z0 -= alpha0;

  h5 += z3;

  h3 += z2;

  h1 += z1;

  h0 += z0;

if (l >= 16) goto multiplyaddatleast16bytes;

multiplyaddatmost15bytes:;

  y7 = h7 + alpha130;

  y6 = h6 + alpha130;

  y1 = h1 + alpha32;

  y0 = h0 + alpha32;

  y7 -= alpha130;

  y6 -= alpha130;

  y1 -= alpha32;

  y0 -= alpha32;

  y5 = h5 + alpha96;

  y4 = h4 + alpha96;

  x7 = h7 - y7;
  y7 *= scale;

  x6 = h6 - y6;
  y6 *= scale;

  x1 = h1 - y1;

  x0 = h0 - y0;

  y5 -= alpha96;

  y4 -= alpha96;

  x1 += y7;

  x0 += y6;

  x7 += y5;

  x6 += y4;

  y3 = h3 + alpha64;

  y2 = h2 + alpha64;

  x0 += x1;

  x6 += x7;

  y3 -= alpha64;
  r3low = r3low_stack;

  y2 -= alpha64;
  r0low = r0low_stack;

  x5 = h5 - y5;
  r3lowx0 = r3low * x0;
  r3high = r3high_stack;

  x4 = h4 - y4;
  r0lowx6 = r0low * x6;
  r0high = r0high_stack;

  x3 = h3 - y3;
  r3highx0 = r3high * x0;
  sr1low = sr1low_stack;

  x2 = h2 - y2;
  r0highx6 = r0high * x6;
  sr1high = sr1high_stack;

  x5 += y3;
  r0lowx0 = r0low * x0;
  r1low = r1low_stack;

  h6 = r3lowx0 + r0lowx6;
  sr1lowx6 = sr1low * x6;
  r1high = r1high_stack;

  x4 += y2;
  r0highx0 = r0high * x0;
  sr2low = sr2low_stack;

  h7 = r3highx0 + r0highx6;
  sr1highx6 = sr1high * x6;
  sr2high = sr2high_stack;

  x3 += y1;
  r1lowx0 = r1low * x0;
  r2low = r2low_stack;

  h0 = r0lowx0 + sr1lowx6;
  sr2lowx6 = sr2low * x6;
  r2high = r2high_stack;

  x2 += y0;
  r1highx0 = r1high * x0;
  sr3low = sr3low_stack;

  h1 = r0highx0 + sr1highx6;
  sr2highx6 = sr2high * x6;
  sr3high = sr3high_stack;

  x4 += x5;
  r2lowx0 = r2low * x0;

  h2 = r1lowx0 + sr2lowx6;
  sr3lowx6 = sr3low * x6;

  x2 += x3;
  r2highx0 = r2high * x0;

  h3 = r1highx0 + sr2highx6;
  sr3highx6 = sr3high * x6;

  r1highx4 = r1high * x4;

  h4 = r2lowx0 + sr3lowx6;
  r1lowx4 = r1low * x4;

  r0highx4 = r0high * x4;

  h5 = r2highx0 + sr3highx6;
  r0lowx4 = r0low * x4;

  h7 += r1highx4;
  sr3highx4 = sr3high * x4;

  h6 += r1lowx4;
  sr3lowx4 = sr3low * x4;

  h5 += r0highx4;
  sr2highx4 = sr2high * x4;

  h4 += r0lowx4;
  sr2lowx4 = sr2low * x4;

  h3 += sr3highx4;
  r0lowx2 = r0low * x2;

  h2 += sr3lowx4;
  r0highx2 = r0high * x2;

  h1 += sr2highx4;
  r1lowx2 = r1low * x2;

  h0 += sr2lowx4;
  r1highx2 = r1high * x2;

  h2 += r0lowx2;
  r2lowx2 = r2low * x2;

  h3 += r0highx2;
  r2highx2 = r2high * x2;

  h4 += r1lowx2;
  sr3lowx2 = sr3low * x2;

  h5 += r1highx2;
  sr3highx2 = sr3high * x2;

  h6 += r2lowx2;

  h7 += r2highx2;

  h0 += sr3lowx2;

  h1 += sr3highx2;

addatmost15bytes:;

if (l == 0) goto nomorebytes;

  lbelow2 = l - 2;

  lbelow3 = l - 3;

  lbelow2 >>= 31;
  lbelow4 = l - 4;

  m00 = *(uchar *) (m + 0);
  lbelow3 >>= 31;
  m += lbelow2;

  m01 = *(uchar *) (m + 1);
  lbelow4 >>= 31;
  m += lbelow3;

  m02 = *(uchar *) (m + 2);
  m += lbelow4;
  m0 = 2151;

  m03 = *(uchar *) (m + 3);
  m0 <<= 51;
  m1 = 2215;

  m0 += m00;
  m01 &= ~lbelow2;

  m02 &= ~lbelow3;
  m01 -= lbelow2;

  m01 <<= 8;
  m03 &= ~lbelow4;

  m0 += m01;
  lbelow2 -= lbelow3;

  m02 += lbelow2;
  lbelow3 -= lbelow4;

  m02 <<= 16;
  m03 += lbelow3;

  m03 <<= 24;
  m0 += m02;

  m0 += m03;
  lbelow5 = l - 5;

  lbelow6 = l - 6;
  lbelow7 = l - 7;

  lbelow5 >>= 31;
  lbelow8 = l - 8;

  lbelow6 >>= 31;
  m += lbelow5;

  m10 = *(uchar *) (m + 4);
  lbelow7 >>= 31;
  m += lbelow6;

  m11 = *(uchar *) (m + 5);
  lbelow8 >>= 31;
  m += lbelow7;

  m12 = *(uchar *) (m + 6);
  m1 <<= 51;
  m += lbelow8;

  m13 = *(uchar *) (m + 7);
  m10 &= ~lbelow5;
  lbelow4 -= lbelow5;

  m10 += lbelow4;
  lbelow5 -= lbelow6;

  m11 &= ~lbelow6;
  m11 += lbelow5;

  m11 <<= 8;
  m1 += m10;

  m1 += m11;
  m12 &= ~lbelow7;

  lbelow6 -= lbelow7;
  m13 &= ~lbelow8;

  m12 += lbelow6;
  lbelow7 -= lbelow8;

  m12 <<= 16;
  m13 += lbelow7;

  m13 <<= 24;
  m1 += m12;

  m1 += m13;
  m2 = 2279;

  lbelow9 = l - 9;
  m3 = 2343;

  lbelow10 = l - 10;
  lbelow11 = l - 11;

  lbelow9 >>= 31;
  lbelow12 = l - 12;

  lbelow10 >>= 31;
  m += lbelow9;

  m20 = *(uchar *) (m + 8);
  lbelow11 >>= 31;
  m += lbelow10;

  m21 = *(uchar *) (m + 9);
  lbelow12 >>= 31;
  m += lbelow11;

  m22 = *(uchar *) (m + 10);
  m2 <<= 51;
  m += lbelow12;

  m23 = *(uchar *) (m + 11);
  m20 &= ~lbelow9;
  lbelow8 -= lbelow9;

  m20 += lbelow8;
  lbelow9 -= lbelow10;

  m21 &= ~lbelow10;
  m21 += lbelow9;

  m21 <<= 8;
  m2 += m20;

  m2 += m21;
  m22 &= ~lbelow11;

  lbelow10 -= lbelow11;
  m23 &= ~lbelow12;

  m22 += lbelow10;
  lbelow11 -= lbelow12;

  m22 <<= 16;
  m23 += lbelow11;

  m23 <<= 24;
  m2 += m22;

  m3 <<= 51;
  lbelow13 = l - 13;

  lbelow13 >>= 31;
  lbelow14 = l - 14;

  lbelow14 >>= 31;
  m += lbelow13;
  lbelow15 = l - 15;

  m30 = *(uchar *) (m + 12);
  lbelow15 >>= 31;
  m += lbelow14;

  m31 = *(uchar *) (m + 13);
  m += lbelow15;
  m2 += m23;

  m32 = *(uchar *) (m + 14);
  m30 &= ~lbelow13;
  lbelow12 -= lbelow13;

  m30 += lbelow12;
  lbelow13 -= lbelow14;

  m3 += m30;
  m31 &= ~lbelow14;

  m31 += lbelow13;
  m32 &= ~lbelow15;

  m31 <<= 8;
  lbelow14 -= lbelow15;

  m3 += m31;
  m32 += lbelow14;
  d0 = m0;

  m32 <<= 16;
  m33 = lbelow15 + 1;
  d1 = m1;

  m33 <<= 24;
  m3 += m32;
  d2 = m2;

  m3 += m33;
  d3 = m3;

  alpha0 = *(double *) (constants + 24);

  z3 = *(double *) &d3;

  z2 = *(double *) &d2;

  z1 = *(double *) &d1;

  z0 = *(double *) &d0;

  z3 -= alpha96;

  z2 -= alpha64;

  z1 -= alpha32;

  z0 -= alpha0;

  h5 += z3;

  h3 += z2;

  h1 += z1;

  h0 += z0;

  y7 = h7 + alpha130;

  y6 = h6 + alpha130;

  y1 = h1 + alpha32;

  y0 = h0 + alpha32;

  y7 -= alpha130;

  y6 -= alpha130;

  y1 -= alpha32;

  y0 -= alpha32;

  y5 = h5 + alpha96;

  y4 = h4 + alpha96;

  x7 = h7 - y7;
  y7 *= scale;

  x6 = h6 - y6;
  y6 *= scale;

  x1 = h1 - y1;

  x0 = h0 - y0;

  y5 -= alpha96;

  y4 -= alpha96;

  x1 += y7;

  x0 += y6;

  x7 += y5;

  x6 += y4;

  y3 = h3 + alpha64;

  y2 = h2 + alpha64;

  x0 += x1;

  x6 += x7;

  y3 -= alpha64;
  r3low = r3low_stack;

  y2 -= alpha64;
  r0low = r0low_stack;

  x5 = h5 - y5;
  r3lowx0 = r3low * x0;
  r3high = r3high_stack;

  x4 = h4 - y4;
  r0lowx6 = r0low * x6;
  r0high = r0high_stack;

  x3 = h3 - y3;
  r3highx0 = r3high * x0;
  sr1low = sr1low_stack;

  x2 = h2 - y2;
  r0highx6 = r0high * x6;
  sr1high = sr1high_stack;

  x5 += y3;
  r0lowx0 = r0low * x0;
  r1low = r1low_stack;

  h6 = r3lowx0 + r0lowx6;
  sr1lowx6 = sr1low * x6;
  r1high = r1high_stack;

  x4 += y2;
  r0highx0 = r0high * x0;
  sr2low = sr2low_stack;

  h7 = r3highx0 + r0highx6;
  sr1highx6 = sr1high * x6;
  sr2high = sr2high_stack;

  x3 += y1;
  r1lowx0 = r1low * x0;
  r2low = r2low_stack;

  h0 = r0lowx0 + sr1lowx6;
  sr2lowx6 = sr2low * x6;
  r2high = r2high_stack;

  x2 += y0;
  r1highx0 = r1high * x0;
  sr3low = sr3low_stack;

  h1 = r0highx0 + sr1highx6;
  sr2highx6 = sr2high * x6;
  sr3high = sr3high_stack;

  x4 += x5;
  r2lowx0 = r2low * x0;

  h2 = r1lowx0 + sr2lowx6;
  sr3lowx6 = sr3low * x6;

  x2 += x3;
  r2highx0 = r2high * x0;

  h3 = r1highx0 + sr2highx6;
  sr3highx6 = sr3high * x6;

  r1highx4 = r1high * x4;

  h4 = r2lowx0 + sr3lowx6;
  r1lowx4 = r1low * x4;

  r0highx4 = r0high * x4;

  h5 = r2highx0 + sr3highx6;
  r0lowx4 = r0low * x4;

  h7 += r1highx4;
  sr3highx4 = sr3high * x4;

  h6 += r1lowx4;
  sr3lowx4 = sr3low * x4;

  h5 += r0highx4;
  sr2highx4 = sr2high * x4;

  h4 += r0lowx4;
  sr2lowx4 = sr2low * x4;

  h3 += sr3highx4;
  r0lowx2 = r0low * x2;

  h2 += sr3lowx4;
  r0highx2 = r0high * x2;

  h1 += sr2highx4;
  r1lowx2 = r1low * x2;

  h0 += sr2lowx4;
  r1highx2 = r1high * x2;

  h2 += r0lowx2;
  r2lowx2 = r2low * x2;

  h3 += r0highx2;
  r2highx2 = r2high * x2;

  h4 += r1lowx2;
  sr3lowx2 = sr3low * x2;

  h5 += r1highx2;
  sr3highx2 = sr3high * x2;

  h6 += r2lowx2;

  h7 += r2highx2;

  h0 += sr3lowx2;

  h1 += sr3highx2;


nomorebytes:;

  offset0 = *(double *) (constants + 104);
  y7 = h7 + alpha130;

  offset1 = *(double *) (constants + 112);
  y0 = h0 + alpha32;

  offset2 = *(double *) (constants + 120);
  y1 = h1 + alpha32;

  offset3 = *(double *) (constants + 128);
  y2 = h2 + alpha64;

  y7 -= alpha130;

  y3 = h3 + alpha64;

  y4 = h4 + alpha96;

  y5 = h5 + alpha96;

  x7 = h7 - y7;
  y7 *= scale;

  y0 -= alpha32;

  y1 -= alpha32;

  y2 -= alpha64;

  h6 += x7;

  y3 -= alpha64;

  y4 -= alpha96;

  y5 -= alpha96;

  y6 = h6 + alpha130;

  x0 = h0 - y0;

  x1 = h1 - y1;

  x2 = h2 - y2;

  y6 -= alpha130;

  x0 += y7;

  x3 = h3 - y3;

  x4 = h4 - y4;

  x5 = h5 - y5;

  x6 = h6 - y6;

  y6 *= scale;

  x2 += y0;

  x3 += y1;

  x4 += y2;

  x0 += y6;

  x5 += y3;

  x6 += y4;

  x2 += x3;

  x0 += x1;

  x4 += x5;

  x6 += y5;

  x2 += offset1;
  *(double *) &d1 = x2;

  x0 += offset0;
  *(double *) &d0 = x0;

  x4 += offset2;
  *(double *) &d2 = x4;

  x6 += offset3;
  *(double *) &d3 = x6;




  f0 = d0;

  f1 = d1;
  bits32 = -1;

  f2 = d2;
  bits32 >>= 32;

  f3 = d3;
  f = f0 >> 32;

  f0 &= bits32;
  f &= 255;

  f1 += f;
  g0 = f0 + 5;

  g = g0 >> 32;
  g0 &= bits32;

  f = f1 >> 32;
  f1 &= bits32;

  f &= 255;
  g1 = f1 + g;

  g = g1 >> 32;
  f2 += f;

  f = f2 >> 32;
  g1 &= bits32;

  f2 &= bits32;
  f &= 255;

  f3 += f;
  g2 = f2 + g;

  g = g2 >> 32;
  g2 &= bits32;

  f4 = f3 >> 32;
  f3 &= bits32;

  f4 &= 255;
  g3 = f3 + g;

  g = g3 >> 32;
  g3 &= bits32;

  g4 = f4 + g;

  g4 = g4 - 4;
  s00 = *(uchar *) (s + 0);

  f = (int64) g4 >> 63;
  s01 = *(uchar *) (s + 1);

  f0 &= f;
  g0 &= ~f;
  s02 = *(uchar *) (s + 2);

  f1 &= f;
  f0 |= g0;
  s03 = *(uchar *) (s + 3);

  g1 &= ~f;
  f2 &= f;
  s10 = *(uchar *) (s + 4);

  f3 &= f;
  g2 &= ~f;
  s11 = *(uchar *) (s + 5);

  g3 &= ~f;
  f1 |= g1;
  s12 = *(uchar *) (s + 6);

  f2 |= g2;
  f3 |= g3;
  s13 = *(uchar *) (s + 7);

  s01 <<= 8;
  f0 += s00;
  s20 = *(uchar *) (s + 8);

  s02 <<= 16;
  f0 += s01;
  s21 = *(uchar *) (s + 9);

  s03 <<= 24;
  f0 += s02;
  s22 = *(uchar *) (s + 10);

  s11 <<= 8;
  f1 += s10;
  s23 = *(uchar *) (s + 11);

  s12 <<= 16;
  f1 += s11;
  s30 = *(uchar *) (s + 12);

  s13 <<= 24;
  f1 += s12;
  s31 = *(uchar *) (s + 13);

  f0 += s03;
  f1 += s13;
  s32 = *(uchar *) (s + 14);

  s21 <<= 8;
  f2 += s20;
  s33 = *(uchar *) (s + 15);

  s22 <<= 16;
  f2 += s21;

  s23 <<= 24;
  f2 += s22;

  s31 <<= 8;
  f3 += s30;

  s32 <<= 16;
  f3 += s31;

  s33 <<= 24;
  f3 += s32;

  f2 += s23;
  f3 += s33;

  *(uchar *) (out + 0) = f0;
  f0 >>= 8;
  *(uchar *) (out + 1) = f0;
  f0 >>= 8;
  *(uchar *) (out + 2) = f0;
  f0 >>= 8;
  *(uchar *) (out + 3) = f0;
  f0 >>= 8;
  f1 += f0;

  *(uchar *) (out + 4) = f1;
  f1 >>= 8;
  *(uchar *) (out + 5) = f1;
  f1 >>= 8;
  *(uchar *) (out + 6) = f1;
  f1 >>= 8;
  *(uchar *) (out + 7) = f1;
  f1 >>= 8;
  f2 += f1;

  *(uchar *) (out + 8) = f2;
  f2 >>= 8;
  *(uchar *) (out + 9) = f2;
  f2 >>= 8;
  *(uchar *) (out + 10) = f2;
  f2 >>= 8;
  *(uchar *) (out + 11) = f2;
  f2 >>= 8;
  f3 += f2;

  *(uchar *) (out + 12) = f3;
  f3 >>= 8;
  *(uchar *) (out + 13) = f3;
  f3 >>= 8;
  *(uchar *) (out + 14) = f3;
  f3 >>= 8;
  *(uchar *) (out + 15) = f3;

#ifdef HAVE_FENV_H
  if (previous_rounding_mode != FE_TONEAREST &&
      fesetround(previous_rounding_mode) != 0) {
      abort();
  }
#endif

  return 0;
}

const char *
crypto_onetimeauth_poly1305_implementation_name(void)
{
    return "53";
}

struct crypto_onetimeauth_poly1305_implementation
crypto_onetimeauth_poly1305_53_implementation = {
    _SODIUM_C99(.implementation_name =) crypto_onetimeauth_poly1305_implementation_name,
    _SODIUM_C99(.onetimeauth =) crypto_onetimeauth,
    _SODIUM_C99(.onetimeauth_verify =) crypto_onetimeauth_verify
};
