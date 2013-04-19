#include "api.h"
#include "crypto_hash_sha512.h"
#include "crypto_verify_32.h"
#include "ge.h"
#include "sc.h"

int crypto_sign_open(
  unsigned char *m,unsigned long long *mlen,
  const unsigned char *sm,unsigned long long smlen,
  const unsigned char *pk
)
{
  unsigned char h[64];
  unsigned char checkr[32];
  ge_p3 A;
  ge_p2 R;
  unsigned long long i;

  *mlen = -1;
  if (smlen < 64) return -1;
  if (sm[63] & 224) return -1;
  if (ge_frombytes_negate_vartime(&A,pk) != 0) return -1;

  for (i = 0;i < smlen;++i) m[i] = sm[i];
  for (i = 0;i < 32;++i) m[32 + i] = pk[i];
  crypto_hash_sha512(h,m,smlen);
  sc_reduce(h);

  ge_double_scalarmult_vartime(&R,h,&A,sm + 32);
  ge_tobytes(checkr,&R);
  if (crypto_verify_32(checkr,sm) != 0) {
    for (i = 0;i < smlen;++i) m[i] = 0;
    return -1;
  }

  for (i = 0;i < smlen - 64;++i) m[i] = sm[64 + i];
  for (i = smlen - 64;i < smlen;++i) m[i] = 0;
  *mlen = smlen - 64;
  return 0;
}
