
#include <string.h>

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
  crypto_hash_sha512_state hs;
  unsigned char h[64];
  unsigned char rcheck[32];
  unsigned int  i;
  unsigned char d = 0;
  ge_p3 A;
  ge_p2 R;

  if (smlen < 64) goto badsig;
  if (sm[63] & 224) goto badsig;
  if (ge_frombytes_negate_vartime(&A,pk) != 0) goto badsig;

  for (i = 0; i < 32; ++i) d |= pk[i];
  if (d == 0) return -1;

  crypto_hash_sha512_init(&hs);
  crypto_hash_sha512_update(&hs, sm, 32);
  crypto_hash_sha512_update(&hs, pk, 32);
  crypto_hash_sha512_update(&hs, sm + 64, smlen - 64);
  crypto_hash_sha512_final(&hs, h);
  sc_reduce(h);

  ge_double_scalarmult_vartime(&R,h,&A,sm+32);
  ge_tobytes(rcheck,&R);
  if (crypto_verify_32(rcheck,sm) == 0) {
    memmove(m, sm + 64,smlen - 64);
    *mlen = smlen - 64;
    return 0;
  }

badsig:
  *mlen = 0;
  memset(m, 0, smlen - 64);
  return -1;
}
