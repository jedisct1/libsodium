#include "crypto_scalarmult.h"

#include "fe.h"
#include "fe51.h"
#include "ladder_base.h"

#define x2 var[0]
#define z2 var[1]

int crypto_scalarmult_base(unsigned char *q,const unsigned char *n)
{
  unsigned char e[32];
  unsigned int i;

  fe var[3]; 

  fe51 x_51;
  fe51 z_51;

  for (i = 0;i < 32;++i) e[i] = n[i];
  e[0] &= 248;
  e[31] &= 127;
  e[31] |= 64;

  ladder_base(var, e);

  z_51.v[0] = (z2[1] << 26) + z2[0];
  z_51.v[1] = (z2[3] << 26) + z2[2];
  z_51.v[2] = (z2[5] << 26) + z2[4];
  z_51.v[3] = (z2[7] << 26) + z2[6];
  z_51.v[4] = (z2[9] << 26) + z2[8];

  x_51.v[0] = (x2[1] << 26) + x2[0];
  x_51.v[1] = (x2[3] << 26) + x2[2];
  x_51.v[2] = (x2[5] << 26) + x2[4];
  x_51.v[3] = (x2[7] << 26) + x2[6];
  x_51.v[4] = (x2[9] << 26) + x2[8];

  fe51_invert(&z_51, &z_51);
  fe51_mul(&x_51, &x_51, &z_51);
  fe51_pack(q, &x_51);

  return 0;
}
