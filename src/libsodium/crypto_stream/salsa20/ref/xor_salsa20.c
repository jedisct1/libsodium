/*
version 20080913
D. J. Bernstein
Public domain.
*/

#include "crypto_core_salsa20.h"
#include "crypto_stream.h"

typedef unsigned int uint32;

static const unsigned char sigma[16] = "expand 32-byte k";

int crypto_stream_xor(
        unsigned char *c,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  unsigned char in[16];
  unsigned char block[64];
  int i;
  unsigned int u;

  if (!mlen) return 0;

  for (i = 0;i < 8;++i) in[i] = n[i];
  for (i = 8;i < 16;++i) in[i] = 0;

  while (mlen >= 64) {
    crypto_core_salsa20(block,in,k,sigma);
    for (i = 0;i < 64;++i) c[i] = m[i] ^ block[i];

    u = 1;
    for (i = 8;i < 16;++i) {
      u += (unsigned int) in[i];
      in[i] = u;
      u >>= 8;
    }

    mlen -= 64;
    c += 64;
    m += 64;
  }

  if (mlen) {
    crypto_core_salsa20(block,in,k,sigma);
    for (i = 0;i < mlen;++i) c[i] = m[i] ^ block[i];
  }
  return 0;
}
