/*
version 20080912
D. J. Bernstein
Public domain.
*/

#include <stdint.h>
#include <stdlib.h>

#include "crypto_core_salsa20.h"
#include "private/common.h"

#define ROUNDS 20
#define U32C(v) (v##U)

static uint32_t rotate(uint32_t u,int c)
{
  return (u << c) | (u >> (32 - c));
}

int crypto_core_salsa20(
        unsigned char *out,
  const unsigned char *in,
  const unsigned char *k,
  const unsigned char *c
)
{
  uint32_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
  uint32_t j0, j1, j2, j3, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14, j15;
  int i;

  if (c == NULL) {
    j0 = x0 = U32C(0x61707865);
    j5 = x5 = U32C(0x3320646e);
    j10 = x10 = U32C(0x79622d32);
    j15 = x15 = U32C(0x6b206574);
  } else {
    j0 = x0 = LOAD32_LE(c + 0);
    j5 = x5 = LOAD32_LE(c + 4);
    j10 = x10 = LOAD32_LE(c + 8);
    j15 = x15 = LOAD32_LE(c + 12);
  }
  j1 = x1 = LOAD32_LE(k + 0);
  j2 = x2 = LOAD32_LE(k + 4);
  j3 = x3 = LOAD32_LE(k + 8);
  j4 = x4 = LOAD32_LE(k + 12);
  j6 = x6 = LOAD32_LE(in + 0);
  j7 = x7 = LOAD32_LE(in + 4);
  j8 = x8 = LOAD32_LE(in + 8);
  j9 = x9 = LOAD32_LE(in + 12);
  j11 = x11 = LOAD32_LE(k + 16);
  j12 = x12 = LOAD32_LE(k + 20);
  j13 = x13 = LOAD32_LE(k + 24);
  j14 = x14 = LOAD32_LE(k + 28);

  for (i = ROUNDS;i > 0;i -= 2) {
     x4 ^= rotate( x0+x12, 7);
     x8 ^= rotate( x4+ x0, 9);
    x12 ^= rotate( x8+ x4,13);
     x0 ^= rotate(x12+ x8,18);
     x9 ^= rotate( x5+ x1, 7);
    x13 ^= rotate( x9+ x5, 9);
     x1 ^= rotate(x13+ x9,13);
     x5 ^= rotate( x1+x13,18);
    x14 ^= rotate(x10+ x6, 7);
     x2 ^= rotate(x14+x10, 9);
     x6 ^= rotate( x2+x14,13);
    x10 ^= rotate( x6+ x2,18);
     x3 ^= rotate(x15+x11, 7);
     x7 ^= rotate( x3+x15, 9);
    x11 ^= rotate( x7+ x3,13);
    x15 ^= rotate(x11+ x7,18);
     x1 ^= rotate( x0+ x3, 7);
     x2 ^= rotate( x1+ x0, 9);
     x3 ^= rotate( x2+ x1,13);
     x0 ^= rotate( x3+ x2,18);
     x6 ^= rotate( x5+ x4, 7);
     x7 ^= rotate( x6+ x5, 9);
     x4 ^= rotate( x7+ x6,13);
     x5 ^= rotate( x4+ x7,18);
    x11 ^= rotate(x10+ x9, 7);
     x8 ^= rotate(x11+x10, 9);
     x9 ^= rotate( x8+x11,13);
    x10 ^= rotate( x9+ x8,18);
    x12 ^= rotate(x15+x14, 7);
    x13 ^= rotate(x12+x15, 9);
    x14 ^= rotate(x13+x12,13);
    x15 ^= rotate(x14+x13,18);
  }

  x0 += j0;
  x1 += j1;
  x2 += j2;
  x3 += j3;
  x4 += j4;
  x5 += j5;
  x6 += j6;
  x7 += j7;
  x8 += j8;
  x9 += j9;
  x10 += j10;
  x11 += j11;
  x12 += j12;
  x13 += j13;
  x14 += j14;
  x15 += j15;

  STORE32_LE(out + 0,x0);
  STORE32_LE(out + 4,x1);
  STORE32_LE(out + 8,x2);
  STORE32_LE(out + 12,x3);
  STORE32_LE(out + 16,x4);
  STORE32_LE(out + 20,x5);
  STORE32_LE(out + 24,x6);
  STORE32_LE(out + 28,x7);
  STORE32_LE(out + 32,x8);
  STORE32_LE(out + 36,x9);
  STORE32_LE(out + 40,x10);
  STORE32_LE(out + 44,x11);
  STORE32_LE(out + 48,x12);
  STORE32_LE(out + 52,x13);
  STORE32_LE(out + 56,x14);
  STORE32_LE(out + 60,x15);

  return 0;
}
