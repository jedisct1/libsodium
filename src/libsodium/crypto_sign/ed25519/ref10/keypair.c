#include "randombytes.h"
#include "crypto_sign.h"
#include "crypto_hash_sha512.h"
#include "ge.h"

int crypto_sign_keypair(unsigned char *pk,unsigned char *sk)
{
  unsigned char h[64];
  ge_p3 A;
  int i;

  randombytes(sk,32);
  crypto_hash_sha512(h,sk,32);
  h[0] &= 248;
  h[31] &= 63;
  h[31] |= 64;

  ge_scalarmult_base(&A,h);
  ge_p3_tobytes(pk,&A);

  for (i = 0;i < 32;++i) sk[32 + i] = pk[i];
  return 0;
}
