#include "crypto_sign.h"
#include "crypto_hash_sha512.h"
#include "ge.h"
#include "sc.h"

int crypto_sign(
  unsigned char *sm,unsigned long long *smlen,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *sk
)
{
  unsigned char az[64];
  unsigned char r[64];
  unsigned char hram[64];
  ge_p3 R;
  unsigned long long i;

  crypto_hash_sha512(az,sk,32);
  az[0] &= 248;
  az[31] &= 63;
  az[31] |= 64;

  *smlen = mlen + 64;
  for (i = 0;i < mlen;++i) sm[64 + i] = m[i];
  for (i = 0;i < 32;++i) sm[32 + i] = az[32 + i];
  crypto_hash_sha512(r,sm + 32,mlen + 32);
  for (i = 0;i < 32;++i) sm[32 + i] = sk[32 + i];

  sc_reduce(r);
  ge_scalarmult_base(&R,r);
  ge_p3_tobytes(sm,&R);

  crypto_hash_sha512(hram,sm,mlen + 64);
  sc_reduce(hram);
  sc_muladd(sm + 32,hram,az,r);

  return 0;
}
