#include "api.h"
#include "crypto_onetimeauth_poly1305_donna.h"
#include "crypto_verify_16.h"

int crypto_onetimeauth_verify(const unsigned char *h,const unsigned char *in,uint64_t inlen,const unsigned char *k)
{
  unsigned char correct[16];
  crypto_onetimeauth(correct,in,inlen,k);
  return crypto_verify_16(h,correct);
}
