
#include <stddef.h>

#ifndef HAVE_TI_MODE

#include "utils.h"
#include "curve25519_ref10.h"
#include "../scalarmult_curve25519.h"
#include "fe.h"

static const unsigned char basepoint[32] = {9};

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

static int
crypto_scalarmult_curve25519_ref10_base(unsigned char *q,
                                        const unsigned char *n)
{
  return crypto_scalarmult_curve25519_ref10(q,n,basepoint);
}

struct crypto_scalarmult_curve25519_implementation
crypto_scalarmult_curve25519_ref10_implementation = {
        SODIUM_C99(.mult = ) crypto_scalarmult_curve25519_ref10,
        SODIUM_C99(.mult_base = ) crypto_scalarmult_curve25519_ref10_base
};

#endif
