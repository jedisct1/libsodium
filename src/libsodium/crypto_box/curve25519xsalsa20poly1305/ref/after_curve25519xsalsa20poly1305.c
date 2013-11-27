#include "api.h"
#include "crypto_secretbox_xsalsa20poly1305.h"

int crypto_box_afternm(
  unsigned char *c,
  const unsigned char *m,uint64_t mlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  return crypto_secretbox_xsalsa20poly1305(c,m,mlen,n,k);
}

int crypto_box_open_afternm(
  unsigned char *m,
  const unsigned char *c,uint64_t clen,
  const unsigned char *n,
  const unsigned char *k
)
{
  return crypto_secretbox_xsalsa20poly1305_open(m,c,clen,n,k);
}
