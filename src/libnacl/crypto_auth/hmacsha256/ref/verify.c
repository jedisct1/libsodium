#include "crypto_verify_32.h"
#include "crypto_auth.h"

int crypto_auth_verify(const unsigned char *h,const unsigned char *in,unsigned long long inlen,const unsigned char *k)
{
  unsigned char correct[32];
  crypto_auth(correct,in,inlen,k);
  return crypto_verify_32(h,correct);
}
