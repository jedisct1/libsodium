/*
version 20080914
D. J. Bernstein
Public domain.
*/

#include "crypto_core_hsalsa20.h"
#include "crypto_stream_salsa20.h"
#include "crypto_stream.h"

static const unsigned char sigma[16] = "expand 32-byte k";

int crypto_stream(
        unsigned char *c,unsigned long long clen,
  const unsigned char *n,
  const unsigned char *k
)
{
  unsigned char subkey[32];
  crypto_core_hsalsa20(subkey,n,k,sigma);
  return crypto_stream_salsa20(c,clen,n + 16,subkey);
}
